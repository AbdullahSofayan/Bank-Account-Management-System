#include <stdio.h>    // Provides functions for input/output operations (e.g., printf, scanf, fopen).
#include <stdlib.h>   // Provides functions for memory allocation, process control, conversions, etc. (e.g., malloc, exit).
#include <string.h>   // Provides functions to handle strings (e.g., strlen, strcpy, strcmp).
#include <ctype.h>    // Provides character handling functions (e.g., isalpha, isdigit).
#include "header.h"
// Define the maximum length for names and email strings.
#define MAX_NAME_LENGTH 100   // Maximum length for a holder's name.
#define MAX_EMAIL_LENGTH 100  // Maximum length for an email address.
#define MAX_OPERATIONS 100    // Maximum number of operations (e.g., deposits/withdrawals) per account.
#define FILENAME "accounts.dat" // Name of the binary file used to store account information.

int isValidName(char* name) {
    for (int i = 0; i < strlen(name); i++) { // Iterate through each character in the name.
        if (!isalpha(name[i]) && name[i] != ' ') { // Check if character is alphabetic or a space.
            return 0; // Return 0 if the name contains invalid characters.
        }
    }
    return 1; // Return 1 if the name is valid.
}

// Implementation of the function to validate an email address.
int isValidEmail(char* email) {
    int atCount = 0, dotCount = 0; // Initialize counters for '@' and '.' symbols.
    for (int i = 0; i < strlen(email); i++) { // Iterate through each character in the email.
        if (email[i] == '@') atCount++;  // Count '@' symbols.
        if (email[i] == '.') dotCount++; // Count '.' symbols.
    }
    return (atCount == 1 && dotCount >= 1); // Return true if one '@' and at least one '.' are present.
}

// Implementation of the function to validate a monetary amount.
int isValidAmount(double amount) {
    return amount > 0; // Return true if the amount is positive.
}

// Implementation of the function to add a new account.
void addAccount() {
    struct Account newAccount; // Create a new account structure.
    
    printf("Enter account number: "); // Prompt the user for an account number.
    scanf("%ld", &newAccount.accountNumber); // Read the account number.
    
    if (accountExists(newAccount.accountNumber)) { // Check if the account already exists.
        printf("Account number already exists.\n");
        return; // Exit the function if the account exists.
    }
    
    getchar(); // Consume leftover newline from input buffer.
    printf("Enter account holder name: "); // Prompt for the account holder's name.
    fgets(newAccount.holderName, MAX_NAME_LENGTH, stdin); // Read the holder's name.
    newAccount.holderName[strcspn(newAccount.holderName, "\n")] = 0; // Remove newline from input.
    
    if (!isValidName(newAccount.holderName)) { // Validate the holder's name.
        printf("Invalid name. Only alphabetic characters are allowed.\n");
        return; // Exit the function if the name is invalid.
    }

    printf("Enter email address: "); // Prompt for the account holder's email.
    fgets(newAccount.email, MAX_EMAIL_LENGTH, stdin); // Read the email address.
    newAccount.email[strcspn(newAccount.email, "\n")] = 0; // Remove newline from input.
    
    if (!isValidEmail(newAccount.email)) { // Validate the email address.
        printf("Invalid email format.\n");
        return; // Exit the function if the email is invalid.
    }

    printf("Enter initial balance: "); // Prompt for the initial balance.
    scanf("%lf", &newAccount.balance); // Read the balance.
    
    if (!isValidAmount(newAccount.balance)) { // Validate the balance.
        printf("Invalid balance amount. It must be positive.\n");
        return; // Exit the function if the balance is invalid.
    }

    newAccount.numOperations = 0; // Initialize the number of operations to zero.

    saveAccount(&newAccount); // Save the new account to the file.
}

// Function to save an account to the file
void saveAccount(struct Account* acc) {
    FILE* file = fopen(FILENAME, "ab"); // Open file in append binary mode
    if (file == NULL) { // Check if the file is opened successfully
        printf("Error opening file.\n");
        return;
    }
    
    fwrite(acc, sizeof(struct Account), 1, file); // Write account data to the file
    fclose(file); // Close the file
}

// Function to check if an account exists based on account number
int accountExists(long accountNumber) {
    FILE* file = fopen(FILENAME, "rb"); // Open file in read binary mode
    if (file == NULL) { // If file doesn't exist, return false
        return 0;
    }
    
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) { // Iterate through all accounts
        if (acc.accountNumber == accountNumber) { // Check if account number matches
            fclose(file);
            return 1; // Account exists
        }
    }
    
    fclose(file); // Close the file
    return 0; // Account doesn't exist
}

// Function to update account details
void updateAccount() {
    long accountNumber;
    printf("Enter account number to update: ");
    scanf("%ld", &accountNumber);

    FILE* file = fopen(FILENAME, "rb+"); // Open file in read/write binary mode
    if (file == NULL) { // Check if the file is opened successfully
        printf("Error opening file.\n");
        return;
    }

    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) { // Search for the account in the file
        if (acc.accountNumber == accountNumber) {
            // Update the account's holder name and email
            getchar(); // Clear the newline
            printf("Enter new account holder name: ");
            fgets(acc.holderName, MAX_NAME_LENGTH, stdin);
            acc.holderName[strcspn(acc.holderName, "\n")] = 0; // Remove newline character

            if (!isValidName(acc.holderName)) { // Validate the name
                printf("Invalid name. Only alphabetic characters are allowed.\n");
                fclose(file);
                return;
            }

            printf("Enter new email address: ");
            fgets(acc.email, MAX_EMAIL_LENGTH, stdin);
            acc.email[strcspn(acc.email, "\n")] = 0; // Remove newline character

            if (!isValidEmail(acc.email)) { // Validate the email
                printf("Invalid email format.\n");
                fclose(file);
                return;
            }

            // Move the file pointer back to overwrite the account record
            fseek(file, -sizeof(struct Account), SEEK_CUR);

            fwrite(&acc, sizeof(struct Account), 1, file); // Write updated account
            printf("Account updated successfully.\n");
            fclose(file);
            return;
        }
    }

    printf("Account not found.\n");
    fclose(file);
}

// Function to delete an account by account number
void deleteAccount() {
    long accountNumber;
    printf("Enter account number to delete: ");
    scanf("%ld", &accountNumber);
    
    FILE* file = fopen(FILENAME, "rb"); // Open file in read binary mode
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    FILE* tempFile = fopen("temp.dat", "wb"); // Open temporary file to store accounts
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        fclose(file);
        return;
    }
    
    struct Account acc;
    int found = 0;
    while (fread(&acc, sizeof(struct Account), 1, file)) { // Iterate through accounts
        if (acc.accountNumber != accountNumber) {
            fwrite(&acc, sizeof(struct Account), 1, tempFile); // Write non-matching accounts
        } else {
            found = 1; // Mark account for deletion
        }
    }
    
    fclose(file);
    fclose(tempFile);
    
    if (found) {
        remove(FILENAME); // Remove original file
        rename("temp.dat", FILENAME); // Replace with temporary file
        printf("Account deleted successfully.\n");
    } else {
        printf("Account not found.\n");
    }
}

// Function to delete all accounts of a specific holder
void deleteHolderAccounts() {
    char holderName[MAX_NAME_LENGTH];
    int found = 0;

    getchar(); // Consume newline
    printf("Enter the holder name to delete all accounts: ");
    fgets(holderName, MAX_NAME_LENGTH, stdin);
    holderName[strcspn(holderName, "\n")] = 0; // Remove newline

    if (!isValidName(holderName)) { // Validate name
        printf("Invalid name. Only alphabetic characters are allowed.\n");
        return;
    }

    FILE *file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    FILE *tempFile = fopen("temp.dat", "wb");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        fclose(file);
        return;
    }

    struct Account acc;

    while (fread(&acc, sizeof(struct Account), 1, file)) { // Iterate through accounts
        if (strcmp(acc.holderName, holderName) != 0) {
            fwrite(&acc, sizeof(struct Account), 1, tempFile); // Write non-matching accounts
        } else {
            found = 1; // Mark accounts for deletion
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        remove(FILENAME); // Remove original file
        rename("temp.dat", FILENAME); // Replace with temporary file
        printf("All accounts for holder '%s' have been deleted.\n", holderName);
    } else {
        remove("temp.dat"); // No accounts matched, delete temp file
        printf("No accounts found for holder '%s'.\n", holderName);
    }
}




// Function to search for an account by account number
void searchAccount() {
    long accountNumber;
    
    // Prompt the user to enter an account number
    printf("Enter account number to search: ");
    scanf("%ld", &accountNumber);

    // Open the file containing account data in binary read mode
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    struct Account acc;

    // Read each account record from the file and search for the matching account number
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            // If found, display the account details
            displayAccount(&acc);
            fclose(file);
            return;
        }
    }

    // If no match is found, print a message
    printf("Account not found.\n");
    fclose(file);
}

// Function to display account details
void displayAccount(struct Account* acc) {
    printf("Account Number: %ld\n", acc->accountNumber);
    printf("Holder Name: %s\n", acc->holderName);
    printf("Email: %s\n", acc->email);
    printf("Balance: %.2f\n", acc->balance);
    printf("Number of Operations: %d\n", acc->numOperations);

    // Loop through and display each operation on the account
    for (int i = 0; i < acc->numOperations; i++) {
        printf("Operation %d: %c %.2f\n", i + 1, acc->operations[i].operation, acc->operations[i].amount);
    }
}

// Function to display all accounts
void displayAllAccounts() {
    // Open the file containing account data in binary read mode
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    struct Account acc;

    // Read and display each account in the file
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        displayAccount(&acc);
        printf("\n"); // Separate accounts with a newline
    }

    fclose(file);
}

// Function to add an operation (e.g., deposit or withdrawal) to an account
void addOperation(long accountNumber, char operation, double amount) {
    if (amount <= 0) {
        printf("Invalid amount. It must be a positive value.\n");
        return;
    }

    FILE* file = fopen(FILENAME, "rb+");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    struct Account acc;
    int found = 0;

    // Search for the account with the specified account number
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            found = 1;

            if (operation == 'd') {
                // Deposit: Increase the balance
                acc.balance += amount;
                printf("Deposit successful! New balance: %.2f\n", acc.balance);
            } else if (operation == 'w') {
                // Withdrawal: Ensure balance is sufficient
                if (amount > acc.balance) {
                    printf("Withdrawal failed! Insufficient balance. Current balance: %.2f\n", acc.balance);
                    fclose(file);
                    return;
                }
                acc.balance -= amount;
                printf("Withdrawal successful! New balance: %.2f\n", acc.balance);
            } else {
                printf("Invalid operation. Use 'd' for deposit or 'w' for withdrawal.\n");
                fclose(file);
                return;
            }

            // Add the operation details
            if (acc.numOperations < MAX_OPERATIONS) {
                acc.operations[acc.numOperations].operation = operation;
                acc.operations[acc.numOperations].amount = amount;
                acc.numOperations++;
            } else {
                printf("Operation limit reached for this account.\n");
                fclose(file);
                return;
            }

            // Move the file pointer back to overwrite the account record
            fseek(file, -sizeof(struct Account), SEEK_CUR);

            // Write the updated account back to the file
            fwrite(&acc, sizeof(struct Account), 1, file);
            fclose(file);
            return;
        }
    }

    if (!found) {
        printf("Account not found.\n");
    }

    fclose(file);
}


// Function to clear the input buffer in case of invalid input
void clearInputBuffer() {
    while (getchar() != '\n'); // Consume invalid input until newline
}

// Function to read a valid integer choice from the user
int getValidChoice() {
    int choice;
    while (1) {
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number between 1 and 8.\n");
            clearInputBuffer(); // Clear input buffer
        } else {
            if (choice >= 1 && choice <= 8) {
                return choice;
            } else {
                printf("Invalid choice! Please enter a number between 1 and 8.\n");
            }
        }
    }
}

// Main function
int main() {
    int choice;

    while (1) {
        // Display the menu to the user
        printf("\nBank Account Management System\n");
        printf("1. Add Account\n");
        printf("2. Update Account\n");
        printf("3. Delete Account\n");
        printf("4. Search Account\n");
        printf("5. Display All Accounts\n");
        printf("6. Add Operation (Deposit/Withdraw)\n");
        printf("7. Delete Holder Accounts\n");
        printf("8. Exit\n\n");

        // Prompt user for menu choice
        printf("Enter your choice: ");
        choice = getValidChoice(); // Validate input

        // Handle user choice
        switch (choice) {
            case 1:
                addAccount(); // Add a new account
                break;
            case 2:
                updateAccount(); // Update an existing account
                break;
            case 3:
                deleteAccount(); // Delete an account
                break;
            case 4:
                searchAccount(); // Search for an account by account number
                break;
            case 5:
                displayAllAccounts(); // Display all accounts
                break;
            case 6: {
                long accountNumber;
                char operation;
                double amount;

                // Prompt for account number
                printf("Enter account number: ");
                if (scanf("%ld", &accountNumber) != 1) {
                    printf("Invalid account number.\n");
                    clearInputBuffer();
                    break;
                }
                clearInputBuffer();

                // Prompt for operation type
                printf("Enter operation (d for deposit, w for withdraw): ");
                if (scanf("%c", &operation) != 1 || (operation != 'd' && operation != 'w')) {
                    printf("Invalid operation. Only 'd' for deposit and 'w' for withdraw are allowed.\n");
                    clearInputBuffer();
                    break;
                }
                clearInputBuffer();

                // Prompt for amount
                printf("Enter amount: ");
                if (scanf("%lf", &amount) != 1 || !isValidAmount(amount)) {
                    printf("Invalid amount. It must be positive.\n");
                    clearInputBuffer();
                    break;
                }

                addOperation(accountNumber, operation, amount); // Add operation
                break;
            }
            case 7:
                deleteHolderAccounts(); // Delete all accounts for a holder name
                break;
            case 8:
                printf("Exiting program.\n");
                exit(0); // Exit program
                break;
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;
}
