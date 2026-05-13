// =============================================
// Wallet.h
// Manages a user's balance and transaction history.
// Depends on: Transaction.h
// =============================================

#ifndef WALLET_H
#define WALLET_H

#include <iostream>
#include <fstream>
#include <string>
#include "Transaction.h"
using namespace std;

class Wallet
{
private:
    double  balance;
    string  walletID;
    
public:
 static const int MAX_TX = 100;
    Transaction* history[MAX_TX]; 
    int     transactionCount;
    //Creates new trasaction : 
    void addTransaction(string type, double amount, string desc, string time)
    {
        if (transactionCount < MAX_TX)
        {
            history[transactionCount++] = new Transaction(type, amount, balance, desc, time);
        }
    }
    //Constructor:
    Wallet(string id, double bal)
    {
        walletID         = id;
        balance          = bal;
        transactionCount = 0;
        for (int i = 0; i < MAX_TX; i++)
        {
            history[i] = nullptr;
        }
    }

    //Destructor:
    ~Wallet()
    {
        for (int i = 0; i < transactionCount; i++)
        {
            delete history[i];
        }
    }

    //Getters:
    double getBalance()
    { 
        return balance; 
    }
    string getWalletID()
    { 
        return walletID; 
    }
    int getTransactionCount()
    { 
        return transactionCount; 
    }
    
    Transaction* getTransaction(int i)
    { 
        return history[i]; 
    }

    //Deposit:
    void deposit(double amount, string desc, string time)
    {
        if (amount <= 0)
        { 
            cout << "Invalid amount!" << endl ; 
            return; 
        }
        balance += amount;
        addTransaction("DEPOSIT", amount, desc, time);
        cout << "Rs." << amount << " deposited. Balance: Rs." << balance << endl;
    }

    // Withdraw:
    bool withdraw(double amount, string desc, string time)
    {
        if (amount <= 0)
        { 
            cout << "Invalid amount!" << endl;
            return false; 
        }
        if (amount > balance)
        { 
            cout << "Insufficient balance!" << endl ; 
            return false; 
        }
        balance -= amount;
        addTransaction("WITHDRAW", amount, desc, time);
        cout << "Rs." << amount << " withdrawn. Balance: Rs." << balance << endl;
        return true;
    }

    //Send money to another Wallet
    bool sendMoney(Wallet& receiver, double amount, string time)
    {
        if (amount <= 0 || amount > balance)
        {
            cout << "Transfer failed!" << endl ;
            return false;
        }
        balance -= amount;
        addTransaction("SENT", amount, "To:" + receiver.walletID, time);

        receiver.balance += amount;
        receiver.addTransaction("RECEIVED", amount, "From:" + walletID, time);

        cout << "Rs." << amount << " sent to " << receiver.walletID << endl;
        return true;
    }

    //Load a transaction from file: 
    void loadTransaction(string type, double amount, double bal, string desc, string time) 
    {
        if (transactionCount < MAX_TX) 
        {
            history[transactionCount++] = new Transaction(type, amount, bal, desc, time);
        }
    }

    void saveTransactions(ofstream& file)
    {
        for (int i = 0; i < transactionCount; i++)
        {
            file << walletID << "|";
            history[i]->saveToFile(file);
        }
    }

    // // ---- Print mini statement to console ----
    // void printStatement() const {
    //     cout << "\n╔══════════════════════════════════════════════╗\n";
    //     cout << "║  Mini Statement — " << walletID << "\n";
    //     cout << "╠══════════════════════════════════════════════╣\n";
    //     if (transactionCount == 0)
    //         cout << "│  No transactions yet.\n";
    //     for (int i = 0; i < transactionCount; i++)
    //         history[i]->display();
    //     cout << "╠══════════════════════════════════════════════╣\n";
    //     cout << "║ Balance: Rs." << balance << "\n";
    //     cout << "╚══════════════════════════════════════════════╝\n";
    // }
};

#endif // WALLET_H
