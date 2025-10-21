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

    ofstream outFile("data/users.txt");
    if (!outFile.is_open()) {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return vector<User>();
    }

    for (int i = 0; i < 1000; ++i) {
        string name = "User_" + to_string(i + 1);
        string key = "PUBKEY_";
        for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetter(rndGen));
        uint64_t  balance = rndBalance(rndGen);

        User u(name, key, balance);
        users.emplace_back(u);
        outFile << u.getName() << "," << u.getKey() << "," << u.getBalance() << endl;
        cout << "Vardas: " << u.getName() << " | Raktas: " << u.getKey() << " | Balansas: " << u.getBalance() << endl;
    }
    cout << "Is viso vartotoju sugeneruota: " << users.size() << endl;
    outFile.close();
    return users;
}

vector<Transaction> GenerateTransaction(vector<User>& users) {
    vector<Transaction> transactions;
    transactions.reserve(10000);

    ofstream outFile("data/transactions.txt");
    if (!outFile.is_open()) {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return vector<Transaction>();
    }

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
        outFile << t.getID() << "," << t.getSender() << "," << t.getReceiver() << "," << t.getAmount() << endl;
        cout << t.getID() << " | " << t.getSender() << " -> " << t.getReceiver() << " : " << t.getAmount() << endl;

        ++generated;
    }
    cout << "Is viso transakciju sugeneruota: " << transactions.size() << endl;
    outFile.close();
    return transactions;
}

void MineBlock(vector<Transaction>& pendingTransactions, vector<Block> chain, int difficulty) {
    if (pendingTransactions.empty()) {
        cout << "Nera transakciju bloko formavimui" << endl;
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

     cout << "Iskastas Blokas #" << chain.size() << "hash'as = " << currentBlock.getHash() << "..." << " transakcijos = " << blockTransactions.size() << " laikas = " << duration.count() << " sekundziu" << endl;
}

int main()
{
    vector<User> users = GenerateUsers();
    //uint64_t totalBefore = 0;
    //for (auto& u : users) totalBefore += u.getBalance();

    vector<Transaction> transactions = GenerateTransaction(users);

    vector<Block> chain;
    int difficulty = 3;
    while (!transactions.empty()) {
        MineBlock(transactions, chain, difficulty);
    }
    cout << "Liko neapdorotu tansakciju: " << transactions.size();

    //uint64_t totalAfter = 0;
    //for (auto& u : users) totalAfter += u.getBalance();

    //cout << "Viso valiutos prieš transakcijas: " << totalBefore << endl;
    //cout << "Viso valiutos po transakcijų: " << totalAfter << endl;
    return 0;
}