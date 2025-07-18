# 🏦 Bank Account Management System (C Language)

A simple console-based banking system written in C that allows you to manage bank accounts. This includes adding, updating, deleting, searching, and performing operations like deposit and withdrawal.

---

## 📁 Project Files

📁 .vscode/ → VSCode project config  
📄 header.h → Header file with definitions and constants  
📄 project.c → Source code of the system  
📄 project.exe → ✅ Precompiled executable (Windows)  
📄 accounts → Binary data file (created on first run)  
📄 README.md → Project documentation  

---

## 🚀 How to Run

### 💻 On Windows (using the precompiled `project.exe`)

You can run the executable directly from the terminal:

```bash
./project.exe
```

## 📋 Features
✅ Add new accounts

✏️ Update existing accounts

❌ Delete specific accounts or all accounts by holder name

🔍 Search accounts by account number

📃 Display all stored accounts

💰 Deposit and withdraw money

🗃️ Data stored in a binary .dat file (named accounts)

## 📦 Data Storage
All account data is stored in a binary file named accounts

Temporary file temp.dat is used for safe deletions

Data persists across runs (no external database needed)




## 🧠 Input Validation
Names must contain only letters and spaces

Emails must contain one @ and at least one .

Balances and transaction amounts must be positive

## 🙋‍♂️ Author
Abdullah Alsofayan  
Educational C project focused on file handling, structs, and binary operations.
