#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class User
{
private:
    
public:
    string name;
    string publicKey;
    double balance;

    User(const string& n, const string& pk, double&& b);
    ~User();
};