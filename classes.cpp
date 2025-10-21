#include "main.h"

//Hash'avimo funkcija
string HashFun(const string& str) {
    unsigned long long seed[8] = {0x5FAF3C1BULL, 0x6E8D3B27ULL, 0xA1C5E97FULL, 0x4B7D2E95ULL, 0xF2A39C68ULL, 0x3E9B5A7CULL, 0x9D74C5A1ULL, 0x7C1A5F3EULL};
    for (size_t i = 0; i < str.size(); ++i) {
        unsigned char cByte = str[i];
        size_t ind = i % 8;
        seed[ind] ^= ((seed[(ind + 1) % 8] << 7) | (seed[(ind + 7) % 8] >> 3));
        seed[ind] += cByte * 131 + (seed[(ind + 3) % 8] ^ seed[(ind + 5) % 8]);
    }

    for (int i = 0; i < 64; ++i) {
        size_t ind = i % 8;
        seed[ind] ^= ((seed[(ind + 1) % 8] << ((i * 7) % 61)) | (seed[(ind + 7) % 8] >> ((i * 5) % 53)));
        seed[ind] += (seed[(ind + 3) % 8] ^ seed[(ind + 5) % 8]) + (0x9E3779B97F4A7C15ULL ^ (i * 0xA1C52E95ULL));
    }

    unsigned long long out4[4];
    for (int i = 0; i < 4; ++i) {
        out4[i] = seed[i] ^ (seed[i + 4] << 1) ^ (seed[(i + 2) % 8] >> 1);
    }

    char out [65];
    snprintf(out, sizeof(out), "%016llx%016llx%016llx%016llx", out4[0], out4[1], out4[2], out4[3]);
    return string(out);
}

// User
User::User(const string& name, const string& publicKey, uint64_t balance)
: name_(name), publicKey_(publicKey), balance_(balance) {}
User::~User() {}

const string& User::getName() const { return name_; }
const string& User::getKey() const { return publicKey_; }
uint64_t User::getBalance() const { return balance_; }

bool User::canSend(uint64_t amount) const { return balance_ >= amount; }
void User::send(uint64_t amount) { if (canSend(amount)) balance_ -= amount; }
void User::receive(uint64_t amount) { balance_ += amount; }

// Transaction
Transaction::Transaction(const string& sender, const string& receiver, uint64_t amount)
: sender_(sender), receiver_(receiver), amount_(amount) {
    string data = sender + receiver + to_string(amount);
    transactionID_ = HashFun(data);
}
Transaction::~Transaction() {}

const string& Transaction::getID() const { return transactionID_; }
const string& Transaction::getSender() const { return sender_; }
const string& Transaction::getReceiver() const { return receiver_; }
uint64_t Transaction::getAmount() const {return amount_; }

//Blockchain
Block::Block(const string& prevHash, const vector<Transaction> transactions, int difficulty)
: prevHash_(prevHash), transactions_(transactions), difficulty_(difficulty), nonce_(0) {
    timestamp_ = to_string(time(nullptr));
    merkleRootHash_ = calculateMerkleRoot();
}
Block::~Block() {};

string Block::getHash() const { return blockHash_; }
string Block::getPrevHash() const { return prevHash_; }
const vector<Transaction> Block::getTransactions() const { return transactions_; }

string Block::calculateMerkleRoot() const {
    string all;
    for (const auto& t : transactions_ )
    {
        all += t.getID();
    }
    return HashFun(all);
}

void Block::mineBlock() {
    string prefix(difficulty_, '0');
    while(true) {
        string data = prevHash_ + timestamp_ + merkleRootHash_ + version_ + to_string(nonce_) + to_string(difficulty_);
        blockHash_ = HashFun(data);
        if (blockHash_.substr(0, difficulty_) == prefix) break;
        nonce_++;
    }
}