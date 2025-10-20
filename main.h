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