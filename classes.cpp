#include "main.h"

// User
User::User(const string& name, const string& publicKey, uint64_t balance)
: name_(name), publicKey_(publicKey), balance_(balance) {}
User::~User() {}

const string& User::getName() const { return name_; }
const string& User::getKey() const { return publicKey_; }
uint64_t User::getBalance() const { return balance_; }

// Transaction
Transaction::Transaction(const string& sender, const string& receiver, uint64_t amount)
: sender_(sender), receiver_(receiver), amount_(amount) {}
Transaction::~Transaction() {}

const string& Transaction::getID() const { return transactionID_; }
const string& Transaction::getSender() const { return sender_; }
const string& Transaction::getReceiver() const { return receiver_; }
uint64_t Transaction::getAmount() const {return amount_;};
