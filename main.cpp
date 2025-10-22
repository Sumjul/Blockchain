#include "main.h"

// Randomizers
mt19937 rndGen(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(0, 999);
uniform_int_distribution<int> rndLetter('A', 'Z');

// Functions
vector<User> GenerateUsers() {
    vector<User> users;
    users.reserve(1000);
    ostringstream log;
    log << "=== Vartotoju generavimas ===" << endl;

    for (int i = 0; i < 1000; ++i) {
        string name = "User_" + to_string(i + 1);
        string key = "PUBKEY_";
        for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetter(rndGen));
        uint64_t  balance = rndBalance(rndGen);

        User u(name, key, balance);
        users.emplace_back(u);
        log << "Vardas: " << u.getName() << " | Raktas: " << u.getKey() << " | Balansas: " << u.getBalance() << endl;
    }
    log << "Is viso vartotoju sugeneruota: " << users.size() << endl;

//    cout << log.str();
    ofstream outFile("data/users.txt");
    if (outFile.is_open()) {
        outFile << log.str();
        outFile.close();
    } else {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return vector<User>();
    }
    return users;
}

vector<Transaction> GenerateTransaction(vector<User>& users) {
    vector<Transaction> transactions;
    transactions.reserve(10000);
    ostringstream log;
    log << "=== Transakciju generavimas ===" << endl;

    int generated = 0;
    while (generated < 10000) {
        int senderInd = rndUser(rndGen);
        int receiverInd = rndUser(rndGen);
        while (receiverInd == senderInd) receiverInd = rndUser(rndGen);

        uint64_t maxAmount = users[senderInd].getBalance();
        if (maxAmount == 0) continue;
        uint64_t amount = uniform_int_distribution<uint64_t>(1, maxAmount)(rndGen);
        users[senderInd].send(amount);
        users[receiverInd].receive(amount);

        Transaction t(users[senderInd].getKey(), users[receiverInd].getKey(), amount);
        transactions.emplace_back(t);
        
        log << t.getID() <<  " | " << t.getSender() << " -> " << t.getReceiver() << " : " << t.getAmount() << endl;
        ++generated;
    }
    log << "Is viso transakciju sugeneruota: " << transactions.size() << endl;
    
//    cout << log.str();
    ofstream outFile("data/transactions.txt");
    if (outFile.is_open()) {
        outFile << log.str();
        outFile.close();
    } else {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return vector<Transaction>();
    }
    return transactions;
}

void MineBlock(vector<Transaction>& pendingTransactions, vector<Block>& chain, vector<User>& users, int difficulty) {
    if (pendingTransactions.empty()) {
        cerr << "Nera transakciju bloko formavimui" << endl;
        return;
    }

    size_t minSize = min<size_t>(100, pendingTransactions.size());
    vector<Transaction> blockTransactions;
    blockTransactions.reserve(minSize);
    for (size_t i = 0; i < minSize; ++i) {
        blockTransactions.push_back(pendingTransactions.back());
        pendingTransactions.pop_back();
    } 
    string prevHash = chain.empty() ? string("0") : chain.back().getHash();

    Block currentBlock(prevHash, blockTransactions, difficulty);
    auto start = chrono::high_resolution_clock::now();
    currentBlock.mineBlock();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    chain.push_back(currentBlock);

    for (const auto& t : blockTransactions) {
        for (auto& u : users) {
            if (u.getName() == t.getSender()) u.setBalance(u.getBalance() - t.getAmount());
            if (u.getName() == t.getReceiver()) u.setBalance(u.getBalance() + t.getAmount());
        }
    }

    ostringstream log;
    log << "==============================" << endl;
    log << "Blokas #" << chain.size() << endl;
    log << "Versija: " << currentBlock.getVersion() << endl;
    log << "Ankstesnio bloko hash: " << prevHash << endl;
    log << "Hash: " << currentBlock.getHash() << endl;
    log << "Nonce: " << currentBlock.getNonce() << endl;
    log << "Difficulty: " << difficulty << endl;
    for (const auto& t : blockTransactions) {
        log << t.getID() << " | Siuntėjas: " << t.getSender() << " -> Gavėjas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
    }
    log << "==============================" << endl << endl;

//    cout << log.str();
    ofstream outFile("data/blocks.txt", ios::app);
    if (outFile.is_open()) {
        outFile << log.str();
        outFile.close();
    } else {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return;
    }
}

int main()
{
    vector<User> users = GenerateUsers();
    vector<Transaction> transactions = GenerateTransaction(users);

    vector<Block> chain;
    int difficulty = 3;
    ofstream outFile("data/blocks.txt");
    outFile.close();
    while (transactions.size() > 0) {
        MineBlock(transactions, chain, users, difficulty);
    }

    cout << "Liko neapdorotu tansakciju: " << transactions.size();
    return 0;
}