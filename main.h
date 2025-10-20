#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
using namespace std;

class User {
private:
    string name_;
    string publicKey_;
    uint64_t balance_;

public:
    User(const string& name, const string& publicKey, uint64_t&& balance);
    ~User();

    const string& getName();
    const string& getKey();
    uint64_t getBalance() const;
};