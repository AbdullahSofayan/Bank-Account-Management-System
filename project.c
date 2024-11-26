#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_OPERATIONS 100
#define FILENAME "accounts.dat"

// Define the structure for each operation (deposit or withdrawal)
struct Operation {
    char operation; // 'd' for deposit, 'w' for withdrawal
    double amount;
};

// Define the structure for the bank account
struct Account {
    long accountNumber;
    double balance;
    char holderName[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int numOperations;
    struct Operation operations[MAX_OPERATIONS];
};

// Function prototypes
int isValidName(char* name);
int isValidEmail(char* email);
int isValidAmount(double amount);
void addAccount();
void updateAccount();
void deleteAccount();
void deleteHolderAccounts();
void searchAccount();
void displayAllAccounts();
void addOperation(long accountNumber, char operation, double amount);
void saveAccount(struct Account* acc);
int accountExists(long accountNumber);
void loadAccounts();
void displayAccount(struct Account* acc);

int isValidName(char* name) {
    for (int i = 0; i < strlen(name); i++) {
        if (!isalpha(name[i]) && name[i] != ' ') {
            return 0; // Invalid name (non-alphabetical character found)
        }
    }
    return 1;
}

int isValidEmail(char* email) {
    int atCount = 0, dotCount = 0;
    for (int i = 0; i < strlen(email); i++) {
        if (email[i] == '@') atCount++;
        if (email[i] == '.') dotCount++;
    }
    return (atCount == 1 && dotCount >= 1);
}

int isValidAmount(double amount) {
    return amount > 0;
}

void addAccount() {
    struct Account newAccount;
    
    printf("Enter account number: ");
    scanf("%ld", &newAccount.accountNumber);
    
    if (accountExists(newAccount.accountNumber)) {
        printf("Account number already exists.\n");
        return;
    }
    
    getchar(); // Consume newline
    printf("Enter account holder name: ");
    fgets(newAccount.holderName, MAX_NAME_LENGTH, stdin);
    newAccount.holderName[strcspn(newAccount.holderName, "\n")] = 0; // Remove newline
    
    if (!isValidName(newAccount.holderName)) {
        printf("Invalid name. Only alphabetic characters are allowed.\n");
        return;
    }

    printf("Enter email address: ");
    fgets(newAccount.email, MAX_EMAIL_LENGTH, stdin);
    newAccount.email[strcspn(newAccount.email, "\n")] = 0; // Remove newline
    
    if (!isValidEmail(newAccount.email)) {
        printf("Invalid email format.\n");
        return;
    }

    printf("Enter initial balance: ");
    scanf("%lf", &newAccount.balance);
    
    if (!isValidAmount(newAccount.balance)) {
        printf("Invalid balance amount. It must be positive.\n");
        return;
    }

    newAccount.numOperations = 0;

    saveAccount(&newAccount);
}

void saveAccount(struct Account* acc) {
    FILE* file = fopen(FILENAME, "ab");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    fwrite(acc, sizeof(struct Account), 1, file);
    fclose(file);
}

int accountExists(long accountNumber) {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        return 0;
    }
    
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0;
}

void updateAccount() {
    long accountNumber;
    printf("Enter account number to update: ");
    scanf("%ld", &accountNumber);

    FILE* file = fopen(FILENAME, "rb+");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            // Update only account number or holder name
            getchar(); // Consume newline
            printf("Enter new account holder name: ");
            fgets(acc.holderName, MAX_NAME_LENGTH, stdin);
            acc.holderName[strcspn(acc.holderName, "\n")] = 0; // Remove newline

            if (!isValidName(acc.holderName)) {
                printf("Invalid name. Only alphabetic characters are allowed.\n");
                fclose(file);
                return;
            }

            printf("Enter new email address: ");
            fgets(acc.email, MAX_EMAIL_LENGTH, stdin);
            acc.email[strcspn(acc.email, "\n")] = 0; // Remove newline

            if (!isValidEmail(acc.email)) {
                printf("Invalid email format.\n");
                fclose(file);
                return;
            }

            // Get the current position of the file pointer
            long currentPos = ftell(file);
            if (currentPos == -1) {
                printf("Error getting file position.\n");
                fclose(file);
                return;
            }

            // Move the file pointer back to the correct position to write the modified account
            fseek(file, currentPos - sizeof(struct Account), SEEK_SET);

            // Write the modified account back to the file
            fwrite(&acc, sizeof(struct Account), 1, file);
            printf("Account updated successfully.\n");
            fclose(file);
            return;
        }
    }

    printf("Account not found.\n");
    fclose(file);
}


void deleteAccount() {
    long accountNumber;
    printf("Enter account number to delete: ");
    scanf("%ld", &accountNumber);
    
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    FILE* tempFile = fopen("temp.dat", "wb");
    if (tempFile == NULL) {
        printf("Error opening temporary file.\n");
        fclose(file);
        return;
    }
    
    struct Account acc;
    int found = 0;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber != accountNumber) {
            fwrite(&acc, sizeof(struct Account), 1, tempFile);
        } else {
            found = 1;
        }
    }
    
    fclose(file);
    fclose(tempFile);
    
    if (found) {
        remove(FILENAME);
        rename("temp.dat", FILENAME);
        printf("Account deleted successfully.\n");
    } else {
        printf("Account not found.\n");
    }
}

void deleteHolderAccounts() {
    char holderName[MAX_NAME_LENGTH];
    int found = 0;

    // Prompt the user for the holder's name
    getchar(); // Consume the leftover newline
    printf("Enter the holder name to delete all accounts: ");
    fgets(holderName, MAX_NAME_LENGTH, stdin);
    holderName[strcspn(holderName, "\n")] = 0; // Remove trailing newline

    // Validate the holder's name
    if (!isValidName(holderName)) {
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

    // Iterate through all accounts, writing only non-matching ones to the temp file
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (strcmp(acc.holderName, holderName) != 0) {
            fwrite(&acc, sizeof(struct Account), 1, tempFile);
        } else {
            found = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found) {
        // Replace the original file with the temp file
        remove(FILENAME);
        rename("temp.dat", FILENAME);
        printf("All accounts for holder '%s' have been deleted.\n", holderName);
    } else {
        // If no accounts matched, remove the temp file
        remove("temp.dat");
        printf("No accounts found for holder '%s'.\n", holderName);
    }
}


void searchAccount() {
    long accountNumber;
    printf("Enter account number to search: ");
    scanf("%ld", &accountNumber);
    
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            displayAccount(&acc);
            fclose(file);
            return;
        }
    }
    
    printf("Account not found.\n");
    fclose(file);
}

void displayAccount(struct Account* acc) {
    printf("Account Number: %ld\n", acc->accountNumber);
    printf("Holder Name: %s\n", acc->holderName);
    printf("Email: %s\n", acc->email);
    printf("Balance: %.2f\n", acc->balance);
    printf("Number of Operations: %d\n", acc->numOperations);
    for (int i = 0; i < acc->numOperations; i++) {
        printf("Operation %d: %c %.2f\n", i + 1, acc->operations[i].operation, acc->operations[i].amount);
    }
}

void displayAllAccounts() {
    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        displayAccount(&acc);
        printf("\n");
    }
    
    fclose(file);
}

void addOperation(long accountNumber, char operation, double amount) {
    FILE* file = fopen(FILENAME, "rb+");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }
    
    struct Account acc;
    while (fread(&acc, sizeof(struct Account), 1, file)) {
        if (acc.accountNumber == accountNumber) {
            if (acc.numOperations < MAX_OPERATIONS) {
                acc.operations[acc.numOperations].operation = operation;
                acc.operations[acc.numOperations].amount = amount;
                acc.numOperations++;
                
                // Move file pointer to the correct position to update the record
                // Using ftell to get the current position of the file pointer
                long currentPos = ftell(file);
                if (currentPos == -1) {
                    printf("Error getting file position.\n");
                    fclose(file);
                    return;
                }
                
                // Move the file pointer back to the correct position to write the modified account
                fseek(file, currentPos - sizeof(struct Account), SEEK_SET);
                
                // Write the modified account back to the file
                fwrite(&acc, sizeof(struct Account), 1, file);
                printf("Operation added successfully.\n");
                fclose(file);
                return;
            }
        }
    }
    
    printf("Account not found.\n");
    fclose(file);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Assuming the structures and functions (like addAccount, updateAccount, etc.) are already defined above.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Assuming the structures and functions (like addAccount, updateAccount, etc.) are already defined above.

// Function to clear the input buffer in case of invalid input
void clearInputBuffer() {
    while (getchar() != '\n'); // Consume invalid input until newline
}

// Function to read a valid integer choice
int getValidChoice() {
    int choice;
    while (1) {
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a number between 1 and 7.\n");
            clearInputBuffer(); // Clear input buffer
        } else {
            // Check if the choice is within the valid range
            if (choice >= 1 && choice <= 8) {
                return choice;
            } else {
                printf("Invalid choice! Please enter a number between 1 and 7.\n");
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

        // Validate user input for menu choice
        printf("Enter your choice: ");
        choice = getValidChoice(); // Get a valid choice

        // Handle the user's choice
        switch (choice) {
            case 1:
                addAccount();  // Add a new account
                break;
            case 2:
                updateAccount();  // Update an existing account
                break;
            case 3:
                deleteAccount();  // Delete an account
                break;
            case 4:
                searchAccount();  // Search for an account by account number
                break;
            case 5:
                displayAllAccounts();  // Display all accounts
                break;
            case 6:
                {
                    long accountNumber;
                    char operation;
                    double amount;

                    // Ask for account number, operation type, and amount
                    printf("Enter account number: ");
                    if (scanf("%ld", &accountNumber) != 1) {
                        printf("Invalid account number.\n");
                        clearInputBuffer(); // Clear input buffer
                        break;
                    }
                    clearInputBuffer(); // Consume newline

                    printf("Enter operation (d for deposit, w for withdraw): ");
                    if (scanf("%c", &operation) != 1 || (operation != 'd' && operation != 'w')) {
                        printf("Invalid operation. Only 'd' for deposit and 'w' for withdraw are allowed.\n");
                        clearInputBuffer(); // Clear input buffer
                        break;
                    }

                    clearInputBuffer(); // Consume newline

                    printf("Enter amount: ");
                    if (scanf("%lf", &amount) != 1 || !isValidAmount(amount)) {
                        printf("Invalid amount. It must be positive.\n");
                        clearInputBuffer(); // Clear input buffer
                        break;
                    }

                    addOperation(accountNumber, operation, amount);  // Add operation (deposit or withdraw)
                }
                break;
            case 7:
                deleteHolderAccounts();  // Delete all accounts for a holder name
                break;
            case 8:
                 printf("Exiting program.\n");
                exit(0);  // Exit the program
                break;
            default:
                printf("Invalid choice, please try again.\n");
        }
    }

    return 0;  // Return 0 to indicate successful completion
}


