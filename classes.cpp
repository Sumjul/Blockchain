#include "main.h"

User::User(const string& name, const string& publicKey, uint64_t balance)
: name_(name), publicKey_(publicKey), balance_(balance) {}

User::~User() {}

const string& User::getName() const { return name_; }
const string& User::getKey() const { return publicKey_; }
uint64_t User::getBalance() const { return balance_;}