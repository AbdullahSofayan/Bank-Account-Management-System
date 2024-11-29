


#ifndef HEADER_H
#define HEADER_H

// Constants
#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_OPERATIONS 100

// Structures
struct Operation {
    char operation;  // 'd' for deposit, 'w' for withdrawal
    double amount;   // Transaction amount
};

struct Account {
    long accountNumber;                  // Unique account number
    double balance;                      // Account balance
    char holderName[MAX_NAME_LENGTH];    // Name of the account holder
    char email[MAX_EMAIL_LENGTH];       // Email address of the account holder
    int numOperations;                  // Number of transactions
    struct Operation operations[MAX_OPERATIONS]; // Array to store operations
};

// Function Prototypes
void saveAccount(struct Account* acc);
void displayAccount(struct Account* acc);
int isValidName(char* name);
int isValidEmail(char* email);
int isValidAmount(double amount);
int accountExists(long accountNumber);
void addAccount();
void updateAccount();
void deleteAccount();
void searchAccount();
void displayAllAccounts();

#endif // HEADER_H
