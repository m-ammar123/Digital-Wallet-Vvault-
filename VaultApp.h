#ifndef VAULTAPP_H
#define VAULTAPP_H

#include <iostream>
#include <string>
#include <vector>
#include "FileManager.h"
#include <ctime>
using namespace std;

class VaultApp : public FileManager
{
private:
    static const int MAX_USERS = 20;
    User*       users[MAX_USERS];
    int         userCount;
    User*       loggedIn;   
    FileManager fileManager;

    //finding user by phone number : 
    User* findUser(string phone)
    {
        for (int i = 0; i < userCount; i++)
        {
            if (users[i]->phone == phone)
            {
                return users[i];
            }
        }
        return nullptr;
    }

    //Time default time : 
    string getTime()
    { 
        time_t now = time(0);
        char* dt = ctime(&now);
        return dt;
        // return "2026-05-05 12:00"; 
    }

    //SAving user funtion : 
    void saveAll()
    {
        fileManager.saveUsers(users, userCount);
        fileManager.saveTransactions(users, userCount);
    }

public:
    //Constructor : 
    VaultApp() : FileManager("users.txt", "transactions.txt") , fileManager("users.txt", "transactions.txt")

    {
    userCount = 0;
    loggedIn  = nullptr;
    for (int i = 0; i < MAX_USERS; i++)
    {
        // Making array empty : 
        users[i] = nullptr;
    }
    //calling LOading data from filemanager.h
    userCount = loadUsers(users, MAX_USERS);
    loadTransactions(users, userCount);
    }

    //Making user defined data type : 
    struct ProfileInfo 
    {
        string name, phone, cnic, walletID;
        double balance;
    };

    //Registering user : 
    string registerUser(string name, string phone, string cnic, string pin, double initial)
    {
        if (userCount >= MAX_USERS)
        {
            return "Max users reached";
        }
        if (findUser(phone))
        {
            return "Phone already registered";
        }

        string walletID = "W-" + phone.substr(0,4) + "-" + to_string(userCount+1);
        Wallet* w = new Wallet(walletID, 0);
        User*   u = new User(name, phone, cnic, pin, w);
        users[userCount++] = u;
        w->deposit(initial, "Initial deposit", getTime());
        saveAll();
        return "";   // empty = success
    }

bool verifyLogin(string phone, string pin)
{
    User* u = findUser(phone);
    return u && u->verifyPIN(pin);
}

string getUserName(string phone) 
{
    User* u = findUser(phone);
    return u ? u->name : "";
}

ProfileInfo getProfile(string phone) 
{
    User* u = findUser(phone);
    return { u->name, u->phone, u->cnic, u->wallet->getWalletID(), u->wallet->getBalance() };
}

double getBalance(string phone)
{
    User* u = findUser(phone);
    return u ? u->wallet->getBalance() : 0;
}

string deposit(string phone, double amount)
{
    User* u = findUser(phone);
    if (!u)
    {
        return "User not found";
    }
    if (amount <= 0)
    {
        return "Invalid amount";
    }
    u->wallet->deposit(amount, "Web deposit", getTime());
    saveAll();
    return "";
}

string withdraw(string phone, double amount)
{
    User* u = findUser(phone);
    if (!u) 
    {
        return "User not found";
    }
    if (amount <= 0)
    {
        return "Invalid amount";
    }
    if (amount > u->wallet->getBalance())
    {
        return "Insufficient balance";
    }
    u->wallet->withdraw(amount, "Web withdrawal", getTime());
    saveAll();
    return "";
}

string sendMoney(string fromPhone, string toPhone, double amount)
{
    User* from = findUser(fromPhone);
    User* to   = findUser(toPhone);
    if (!from)
    {
        return "Sender not found";
    }
    if (!to)
    {
        return "Recipient not found";
    }
    if (amount <= 0)
    {
        return "Invalid amount";
    }
    if (amount > from->wallet->getBalance())
    {
        return "Insufficient balance";
    }
    from->wallet->sendMoney(*(to->wallet), amount, getTime());
    saveAll();
    return "";
}

vector<Transaction*> getStatement(string phone) 
{
    User* u = findUser(phone);
    vector<Transaction*> result;
    if (!u) return result;
    Wallet* w = u->wallet;
    for (int i = 0; i < w->getTransactionCount(); i++)
    {
        result.push_back(w->history[i]);   // make history[] public or add getter
    }
    return result;
}
};

#endif
