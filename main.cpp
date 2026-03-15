#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>

using namespace std;

const int MAX_HISTORY   = 5;
const int MAX_ACCOUNTS  = 10;
const string SAVE_FILE  = "bank_data.txt";
const string ADMIN_PASS = "admin123";

struct Transaction {
    string type;
    double amount;
    double balanceAfter;
};

struct Account {
    int id;
    string name;
    double balance;
    bool active;
    int pin;
    Transaction history[MAX_HISTORY];
    int historyCount;
    int historyStart;
};

Account accounts[MAX_ACCOUNTS];
int accountCount = 0;
int nextId       = 1001;

void printLine() {
    cout << "----------------------------------------\n";
}

void printMoney(double amount) {
    cout << "$" << fixed << setprecision(2) << amount;
}

int findAccount(int id) {
    for (int i = 0; i < accountCount; i++)
        if (accounts[i].id == id && accounts[i].active)
            return i;
    return -1;
}

void addTransaction(Account& acc, string type, double amount) {
    int slot;
    if (acc.historyCount < MAX_HISTORY) {
        slot = acc.historyCount++;
    } else {
        slot = acc.historyStart;
        acc.historyStart = (acc.historyStart + 1) % MAX_HISTORY;
    }
    acc.history[slot].type         = type;
    acc.history[slot].amount       = amount;
    acc.history[slot].balanceAfter = acc.balance;
}

bool verifyPin(Account& acc) {
    int attempts = 3;
    while (attempts > 0) {
        int entered;
        cout << "  Enter PIN (" << attempts << " attempt" << (attempts > 1 ? "s" : "") << " left): ";
        cin >> entered;
        if (entered == acc.pin) return true;
        attempts--;
        if (attempts > 0) cout << "  Wrong PIN. Try again.\n";
    }
    cout << "  Too many wrong attempts. Access denied.\n";
    return false;
}

bool verifyAdmin() {
    int attempts = 3;
    while (attempts > 0) {
        string entered;
        cout << "  Enter admin password (" << attempts << " attempt" << (attempts > 1 ? "s" : "") << " left): ";
        cin >> entered;
        if (entered == ADMIN_PASS) return true;
        attempts--;
        if (attempts > 0) cout << "  Wrong password. Try again.\n";
    }
    cout << "  Access denied.\n";
    return false;
}

void saveData() {
    ofstream file(SAVE_FILE);
    if (!file) { cout << "  Error: could not save data.\n"; return; }
    file << accountCount << "\n" << nextId << "\n";
    for (int i = 0; i < accountCount; i++) {
        Account& a = accounts[i];
        file << a.id << "\n" << a.name << "\n" << a.balance << "\n"
             << a.active << "\n" << a.pin << "\n"
             << a.historyCount << "\n" << a.historyStart << "\n";
        for (int j = 0; j < a.historyCount; j++) {
            int idx = (a.historyStart + j) % MAX_HISTORY;
            file << a.history[idx].type << "\n"
                 << a.history[idx].amount << "\n"
                 << a.history[idx].balanceAfter << "\n";
        }
    }
    file.close();
}

void loadData() {
    ifstream file(SAVE_FILE);
    if (!file) return;
    file >> accountCount >> nextId;
    for (int i = 0; i < accountCount; i++) {
        Account& a = accounts[i];
        file >> a.id;
        file.ignore();
        getline(file, a.name);
        file >> a.balance >> a.active >> a.pin >> a.historyCount >> a.historyStart;
        for (int j = 0; j < a.historyCount; j++) {
            int idx = (a.historyStart + j) % MAX_HISTORY;
            file.ignore();
            getline(file, a.history[idx].type);
            file >> a.history[idx].amount >> a.history[idx].balanceAfter;
        }
    }
    file.close();
}

void adminPanel() {
    printLine();
    cout << "  ADMIN PANEL\n";
    printLine();
    if (!verifyAdmin()) return;

    int choice;
    do {
        cout << "\n";
        printLine();
        cout << "       ADMIN PANEL\n";
        printLine();
        cout << "  1. View All Accounts\n";
        cout << "  2. View Account Details\n";
        cout << "  3. Delete Account\n";
        cout << "  4. Bank Summary\n";
        cout << "  0. Back to Main Menu\n";
        printLine();
        cout << "  Choose an option: ";
        cin >> choice;

        if (choice == 1) {
            printLine();
            cout << "  ALL ACCOUNTS (ADMIN VIEW)\n";
            printLine();
            if (accountCount == 0) { cout << "  No accounts.\n"; continue; }
            cout << "  " << left << setw(8) << "ID" << setw(20) << "Name"
                 << setw(12) << "Balance" << setw(8) << "PIN" << "Status\n";
            printLine();
            for (int i = 0; i < accountCount; i++) {
                cout << "  " << setw(8)  << accounts[i].id
                     << setw(20) << accounts[i].name
                     << "$" << setw(11) << fixed << setprecision(2) << accounts[i].balance
                     << setw(8)  << accounts[i].pin
                     << (accounts[i].active ? "Active" : "Closed") << "\n";
            }
        }

        else if (choice == 2) {
            int id;
            cout << "  Enter Account ID: ";
            cin >> id;
            int index = -1;
            for (int i = 0; i < accountCount; i++)
                if (accounts[i].id == id) { index = i; break; }
            if (index == -1) { cout << "  Account not found!\n"; continue; }
            Account& acc = accounts[index];
            cout << "\n";
            cout << "  Account ID : " << acc.id << "\n";
            cout << "  Name       : " << acc.name << "\n";
            cout << "  Balance    : "; printMoney(acc.balance); cout << "\n";
            cout << "  PIN        : " << acc.pin << "\n";
            cout << "  Status     : " << (acc.active ? "Active" : "Closed") << "\n";
            cout << "\n  -- Last " << acc.historyCount << " Transaction(s) --\n";
            if (acc.historyCount == 0) {
                cout << "  No transactions yet.\n";
            } else {
                cout << "  " << left << setw(16) << "Type" << setw(12) << "Amount" << "Balance After\n";
                cout << "  ----------------------------------------\n";
                for (int i = 0; i < acc.historyCount; i++) {
                    int idx = (acc.historyStart + i) % MAX_HISTORY;
                    cout << "  " << setw(16) << acc.history[idx].type
                         << "$" << setw(11) << fixed << setprecision(2) << acc.history[idx].amount
                         << "$" << fixed << setprecision(2) << acc.history[idx].balanceAfter << "\n";
                }
            }
        }

        else if (choice == 3) {
            int id;
            cout << "  Enter Account ID to delete: ";
            cin >> id;
            int index = -1;
            for (int i = 0; i < accountCount; i++)
                if (accounts[i].id == id) { index = i; break; }
            if (index == -1) { cout << "  Account not found!\n"; continue; }
            char confirm;
            cout << "  Delete account " << id << " (" << accounts[index].name << ")? (y/n): ";
            cin >> confirm;
            if (confirm == 'y' || confirm == 'Y') {
                accounts[index].active = false;
                saveData();
                cout << "  Account " << id << " deleted.\n";
            } else {
                cout << "  Cancelled.\n";
            }
        }

        else if (choice == 4) {
            printLine();
            cout << "  BANK SUMMARY\n";
            printLine();
            int active = 0, closed = 0;
            double totalBalance = 0;
            double highest = -1, lowest = -1;
            string richestName, poorestName;
            for (int i = 0; i < accountCount; i++) {
                if (accounts[i].active) {
                    active++;
                    totalBalance += accounts[i].balance;
                    if (highest < 0 || accounts[i].balance > highest) {
                        highest     = accounts[i].balance;
                        richestName = accounts[i].name;
                    }
                    if (lowest < 0 || accounts[i].balance < lowest) {
                        lowest     = accounts[i].balance;
                        poorestName = accounts[i].name;
                    }
                } else {
                    closed++;
                }
            }
            cout << "  Active Accounts  : " << active << "\n";
            cout << "  Closed Accounts  : " << closed << "\n";
            cout << "  Total Deposits   : "; printMoney(totalBalance); cout << "\n";
            if (active > 0) {
                cout << "  Average Balance  : "; printMoney(totalBalance / active); cout << "\n";
                cout << "  Highest Balance  : "; printMoney(highest);
                cout << "  (" << richestName << ")\n";
                cout << "  Lowest Balance   : "; printMoney(lowest);
                cout << "  (" << poorestName << ")\n";
            }
        }

        else if (choice != 0) {
            cout << "  Invalid option.\n";
        }

    } while (choice != 0);
}

void createAccount() {
    printLine();
    cout << "  CREATE NEW ACCOUNT\n";
    printLine();
    if (accountCount >= MAX_ACCOUNTS) { cout << "  Sorry, bank is full!\n"; return; }
    Account newAcc;
    newAcc.id           = nextId++;
    newAcc.active       = true;
    newAcc.historyCount = 0;
    newAcc.historyStart = 0;
    cout << "  Enter your name: ";
    cin.ignore();
    getline(cin, newAcc.name);
    cout << "  Set a 4-digit PIN: ";
    cin >> newAcc.pin;
    cout << "  Opening deposit amount: $";
    cin >> newAcc.balance;
    if (newAcc.balance < 0) { cout << "  Invalid. Setting to $0.\n"; newAcc.balance = 0; }
    accounts[accountCount] = newAcc;
    addTransaction(accounts[accountCount], "Account Opened", newAcc.balance);
    accountCount++;
    saveData();
    cout << "\n  Account created!\n";
    cout << "  Your Account ID : " << newAcc.id << "\n";
    cout << "  Opening Balance : "; printMoney(newAcc.balance); cout << "\n";
    cout << "  Keep your PIN safe!\n";
}

void deposit() {
    printLine();
    cout << "  DEPOSIT\n";
    printLine();
    int id;
    cout << "  Enter Account ID: ";
    cin >> id;
    int index = findAccount(id);
    if (index == -1) { cout << "  Account not found!\n"; return; }
    if (!verifyPin(accounts[index])) return;
    double amount;
    cout << "  Enter deposit amount: $";
    cin >> amount;
    if (amount <= 0) { cout << "  Invalid amount.\n"; return; }
    accounts[index].balance += amount;
    addTransaction(accounts[index], "Deposit", amount);
    saveData();
    cout << "\n  Deposit successful!\n";
    cout << "  Deposited   : "; printMoney(amount);                  cout << "\n";
    cout << "  New Balance : "; printMoney(accounts[index].balance); cout << "\n";
}

void withdraw() {
    printLine();
    cout << "  WITHDRAW\n";
    printLine();
    int id;
    cout << "  Enter Account ID: ";
    cin >> id;
    int index = findAccount(id);
    if (index == -1) { cout << "  Account not found!\n"; return; }
    if (!verifyPin(accounts[index])) return;
    double amount;
    cout << "  Enter withdrawal amount: $";
    cin >> amount;
    if (amount <= 0) { cout << "  Invalid amount.\n"; return; }
    if (amount > accounts[index].balance) {
        cout << "  Insufficient funds! Available: "; printMoney(accounts[index].balance); cout << "\n";
        return;
    }
    accounts[index].balance -= amount;
    addTransaction(accounts[index], "Withdraw", amount);
    saveData();
    cout << "\n  Withdrawal successful!\n";
    cout << "  Withdrawn   : "; printMoney(amount);                  cout << "\n";
    cout << "  New Balance : "; printMoney(accounts[index].balance); cout << "\n";
}

void transfer() {
    printLine();
    cout << "  TRANSFER MONEY\n";
    printLine();
    int fromId;
    cout << "  Enter YOUR Account ID: ";
    cin >> fromId;
    int fromIndex = findAccount(fromId);
    if (fromIndex == -1) { cout << "  Account not found!\n"; return; }
    if (!verifyPin(accounts[fromIndex])) return;
    int toId;
    cout << "  Enter RECIPIENT Account ID: ";
    cin >> toId;
    if (toId == fromId) { cout << "  Cannot transfer to the same account!\n"; return; }
    int toIndex = findAccount(toId);
    if (toIndex == -1) { cout << "  Recipient not found!\n"; return; }
    double amount;
    cout << "  Enter transfer amount: $";
    cin >> amount;
    if (amount <= 0) { cout << "  Invalid amount.\n"; return; }
    if (amount > accounts[fromIndex].balance) {
        cout << "  Insufficient funds! Available: "; printMoney(accounts[fromIndex].balance); cout << "\n";
        return;
    }
    accounts[fromIndex].balance -= amount;
    accounts[toIndex].balance   += amount;
    addTransaction(accounts[fromIndex], "Transfer Out", amount);
    addTransaction(accounts[toIndex],   "Transfer In",  amount);
    saveData();
    cout << "\n  Transfer successful!\n";
    cout << "  Sent to     : " << accounts[toIndex].name << "\n";
    cout << "  Amount      : "; printMoney(amount);                      cout << "\n";
    cout << "  Your Balance: "; printMoney(accounts[fromIndex].balance); cout << "\n";
}

void viewAccount() {
    printLine();
    cout << "  ACCOUNT DETAILS\n";
    printLine();
    int id;
    cout << "  Enter Account ID: ";
    cin >> id;
    int index = findAccount(id);
    if (index == -1) { cout << "  Account not found!\n"; return; }
    if (!verifyPin(accounts[index])) return;
    Account& acc = accounts[index];
    cout << "\n";
    cout << "  Account ID : " << acc.id   << "\n";
    cout << "  Name       : " << acc.name << "\n";
    cout << "  Balance    : "; printMoney(acc.balance); cout << "\n";
    cout << "  Status     : Active\n";
    cout << "\n  -- Last " << acc.historyCount << " Transaction(s) --\n";
    if (acc.historyCount == 0) {
        cout << "  No transactions yet.\n";
    } else {
        cout << "  " << left << setw(16) << "Type" << setw(12) << "Amount" << "Balance After\n";
        cout << "  ----------------------------------------\n";
        for (int i = 0; i < acc.historyCount; i++) {
            int idx = (acc.historyStart + i) % MAX_HISTORY;
            cout << "  " << setw(16) << acc.history[idx].type
                 << "$" << setw(11) << fixed << setprecision(2) << acc.history[idx].amount
                 << "$" << fixed << setprecision(2) << acc.history[idx].balanceAfter << "\n";
        }
    }
}

void listAllAccounts() {
    printLine();
    cout << "  ALL ACCOUNTS\n";
    printLine();
    if (accountCount == 0) { cout << "  No accounts yet.\n"; return; }
    cout << "  " << left << setw(8) << "ID" << setw(20) << "Name" << "Balance\n";
    printLine();
    for (int i = 0; i < accountCount; i++) {
        if (accounts[i].active) {
            cout << "  " << setw(8)  << accounts[i].id
                 << setw(20) << accounts[i].name
                 << "$" << fixed << setprecision(2) << accounts[i].balance << "\n";
        }
    }
}

void closeAccount() {
    printLine();
    cout << "  CLOSE ACCOUNT\n";
    printLine();
    int id;
    cout << "  Enter Account ID to close: ";
    cin >> id;
    int index = findAccount(id);
    if (index == -1) { cout << "  Account not found!\n"; return; }
    if (!verifyPin(accounts[index])) return;
    char confirm;
    cout << "  Are you sure? (y/n): ";
    cin >> confirm;
    if (confirm == 'y' || confirm == 'Y') {
        accounts[index].active = false;
        saveData();
        cout << "  Account " << id << " closed.\n";
    } else {
        cout << "  Cancelled.\n";
    }
}

void showMenu() {
    cout << "\n";
    printLine();
    cout << "       SIMPLE BANKING SYSTEM\n";
    printLine();
    cout << "  1. Create Account\n";
    cout << "  2. Deposit\n";
    cout << "  3. Withdraw\n";
    cout << "  4. View Account + History\n";
    cout << "  5. List All Accounts\n";
    cout << "  6. Transfer Money\n";
    cout << "  7. Close Account\n";
    cout << "  8. Admin Panel\n";
    cout << "  0. Exit\n";
    printLine();
    cout << "  Choose an option: ";
}

int main() {
    loadData();
    int choice;
    cout << "  Welcome to Simple Bank!\n";
    do {
        showMenu();
        cin >> choice;
        switch (choice) {
            case 1: createAccount();   break;
            case 2: deposit();         break;
            case 3: withdraw();        break;
            case 4: viewAccount();     break;
            case 5: listAllAccounts(); break;
            case 6: transfer();        break;
            case 7: closeAccount();    break;
            case 8: adminPanel();      break;
            case 0: cout << "\n  Goodbye!\n"; break;
            default: cout << "  Invalid option.\n";
        }
    } while (choice != 0);
    return 0;
}