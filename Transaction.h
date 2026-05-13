#ifndef TRANSACTION_H    
#define TRANSACTION_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

class Transaction
{
private:
    string type;
    double amount;
    double balanceAfter;
    string description;
    string timestamp;

public:
    //Constructor : 
    Transaction(string t, double amt, double bal,string desc, string time)
    {
        type         = t;
        amount       = amt;
        balanceAfter = bal;
        description  = desc;
        timestamp    = time;
    }

    //
    // void display()
    // {
    //     cout << "│ " << timestamp
    //         << " │ " << type
    //          << "\t│ Rs." << amount
    //          << "\t│ Bal:Rs." << balanceAfter
    //          << "\t│ " << description << endl;
    // }

    void saveToFile(ofstream& file)
    {
        file << type         << "|"
             << amount       << "|"
             << balanceAfter << "|"
             << description  << "|"
             << timestamp    << "\n";
    }

    //Getters : 
    string getType() 
    { 
        return type; 
    }
    double getAmount() 
    { 
        return amount; 
    }
    double getBalanceAfter()
    { 
        return balanceAfter; 
    }
    string getDescription()
    { 
        return description; 
    }
    string getTimestamp()
    { 
        return timestamp; 
    }
};

#endif
