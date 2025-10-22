#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <random>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <sstream>
using namespace std;

string HashFun(const string&);

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
    void setBalance(double newBalance);

    bool canSend(uint64_t amount) const;
    void send(uint64_t amount);
    void receive(uint64_t amount);
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

class Block {
private:
    string prevHash_;
    string merkleRootHash_;
    int nonce_;
    int difficulty_;
    string timestamp_;
    const string version_ = "v0.1";
    
    string blockHash_;
    vector<Transaction> transactions_;

public:
    Block(const string& prevHash, const vector<Transaction> transactions, int difficulty);
    ~Block();

    string getHash() const;
    string getPrevHash() const;
    string getVersion() const;
    int getNonce() const;
    const vector<Transaction> getTransactions() const;

    string calculateMerkleRoot() const;
    void mineBlock();
};