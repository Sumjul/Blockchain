#include "main.h"

User::User(const string& n, const string& pk, double&& b)
: name(n), publicKey(pk), balance(b) {};

User::~User() {};