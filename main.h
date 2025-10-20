#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <random>
#include <fstream>
using namespace std;

class User {
private:
    string name_;
    string publicKey_;
    uint64_t balance_;

public:
    User(const string& name, const string& publicKey, uint64_t balance);
    ~User();

    const string& getName() const;
    const string& getKey() const;
    uint64_t getBalance() const;
};

class Transaction {
private:
    string transactionID_;
    string sender_;
    string receiver_;
    uint64_t amount_;

public:
    Transaction(const string& sender, const string& receiver, uint64_t amount);
    ~Transaction();

    const string& getID() const;
    const string& getSender() const;
    const string& getReceiver() const;
    uint64_t getAmount() const;
};
