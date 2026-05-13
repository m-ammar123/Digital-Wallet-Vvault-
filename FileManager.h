#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "User.h"   
using namespace std;

class FileManager 
{
private:
    string usersFile;         
    string transactionsFile;  
public:
    //Constructor:
    FileManager() {}
    FileManager(string uf, string tf)
    {
        usersFile        = uf;
        transactionsFile = tf;
    }

    //Savin users in teh format of name|phone|cnic|pin|walletID|balance :
    void saveUsers(User* users[], int count)
    {
        //If file dosnt found : 
        ofstream file(usersFile);
        if (!file.is_open())
        {
            cout << "Could not open " << usersFile << endl;
            return;
        }

        //Saving users information : 
        for (int i = 0; i < count; i++) 
        {
            file << users[i]->name                  << "|"
                 << users[i]->phone                 << "|"
                 << users[i]->cnic                  << "|"
                 << users[i]->pin                   << "|"
                 << users[i]->wallet->getWalletID() << "|"
                 << users[i]->wallet->getBalance()  << "\n";
        }
        file.close();
        cout << "Users saved to " << usersFile << endl;
    }

    //Saving transactions in the form of walletID|type|amount|balanceAfter|description|timestamp : 
    void saveTransactions(User* users[], int count)
    {
        ofstream file(transactionsFile);
        //If file is not found  : 
        if (!file.is_open())
        {
            cout << "Could not open " << transactionsFile << endl;
            return;
        }

        //Saving transactions : 
        for (int i = 0; i < count; i++)
        {
            users[i]->wallet->saveTransactions(file);
        }
        file.close();
        cout << "Transactions saved to " << transactionsFile << endl;
    }

    //Loading saved users : 
    int loadUsers(User* users[], int maxUsers)
    {
        ifstream file(usersFile);
        //If file not found : 
        if (!file.is_open())
        {
            cout << "No users file found" << endl;
            return 0;
        }

        int count = 0;
        string line;
        //Getting data : 
        while (getline(file, line) && count < maxUsers)
        {
            if (line.empty()) continue;

            stringstream ss(line);
            string name, phone, cnic, pin, walletID, balStr;

            getline(ss, name,     '|');
            getline(ss, phone,    '|');
            getline(ss, cnic,     '|');
            getline(ss, pin,      '|');
            getline(ss, walletID, '|');
            getline(ss, balStr,   '|');

            //Adiing data to programme :
            double balance;
            if (balStr.empty())
            {
                balance = 0 ;
            }
            else
            {
                balance = stod(balStr);
            }

            Wallet* w  = new Wallet(walletID, balance);
            users[count++] = new User(name, phone, cnic, pin, w);
        }
        file.close();
        cout << count << " user(s) loaded from " << usersFile << endl;
        return count;
    }

    //Loading transactions from file : 
    void loadTransactions(User* users[], int userCount)
    {
        ifstream file(transactionsFile);
        //If file doesnt exist : 
        if (!file.is_open()) 
        {
            return;
        }

        string line;
        while (getline(file, line))
        {
            if (line.empty()) continue;

            stringstream ss(line);
            string walletID, type, amtStr, balStr, desc, time;

            getline(ss, walletID, '|');
            getline(ss, type,     '|');
            getline(ss, amtStr,   '|');
            getline(ss, balStr,   '|');
            getline(ss, desc,     '|');
            getline(ss, time,     '|');

            double amount = stod(amtStr);
            double bal    = stod(balStr);

            // Match this transaction to the correct wallet
            for (int i = 0; i < userCount; i++)
            {
                if (users[i]->wallet->getWalletID() == walletID)
                {
                    users[i]->wallet->loadTransaction(type, amount, bal, desc, time);
                    break;
                }
            }
        }
        file.close();
        cout << "Transaction history loaded" << endl;
    }
};

#endif
