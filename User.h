#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include "Wallet.h"
using namespace std;

class User
{
public:
    // Public so FileManager can access
    string  name;
    string  phone;
    string  cnic;
    string  pin;
    Wallet* wallet;

    //Constructor
    User(string n, string ph, string id, string p, Wallet* w)
    {
        name   = n;
        phone  = ph;
        cnic   = id;
        pin    = p;
        wallet = w;
    }

    //Deastructor
    ~User() { }

    //Verifying pin :
    bool verifyPIN(string entered)
    {
        return entered == pin;
    }

    //
    // void displayInfo() const {
    //     cout << "\n┌─────────────────────────────┐\n";
    //     cout << "│   👤 vVault Account          │\n";
    //     cout << "├─────────────────────────────┤\n";
    //     cout << "│ Name    : " << name                    << "\n";
    //     cout << "│ Phone   : " << phone                   << "\n";
    //     cout << "│ CNIC    : " << cnic                    << "\n";
    //     cout << "│ Wallet  : " << wallet->getWalletID()   << "\n";
    //     cout << "│ Balance : Rs." << wallet->getBalance() << "\n";
    //     cout << "└─────────────────────────────┘\n";
    // }
};

#endif
