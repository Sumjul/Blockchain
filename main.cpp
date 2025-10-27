#include "main.h"

// Randomizers
mt19937 rndGen(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(0, 999);
uniform_int_distribution<int> rndLetter('A', 'Z');


// Console loging
void ConsoleUserStats(const vector<User>& users) {
    uint64_t totalBalance = 0, maxBalance = 0, minBalance = UINT64_MAX;
    for (const auto& u : users) {
        totalBalance += u.getBalance();
        maxBalance = max(maxBalance, u.getBalance());
        minBalance = min(minBalance, u.getBalance());
    }
    double avgBalance = static_cast<double>(totalBalance) / users.size();

    cout << "=== Vartotoju generavimas ===" << endl;
    cout << " - Sugeneruota vartotoju: " << users.size() << endl;
    cout << " - Bendras balansas: " << totalBalance << endl;
    cout << " - Vidutinis balansas: " << (uint64_t)avgBalance << endl;
    cout << " - Maksimalus balansas: " << maxBalance << endl;
    cout << " - Minimalus balansas: " << minBalance << endl;
    cout << "=============================" << endl << endl;
}

void ConsoleTransactionStats(const vector<Transaction>& transactions) {
    uint64_t totalAmount = 0, maxAmount = 0, minAmount = UINT64_MAX;
    for (const auto& t : transactions) {
        totalAmount += t.getAmount();
        maxAmount = max(maxAmount, t.getAmount());
        minAmount = min(minAmount, t.getAmount());
    }
    double avgAmount = transactions.empty() ? 0 : static_cast<double>(totalAmount) / transactions.size();

    cout << "=== Transakciju generavimas ===" << endl;
    cout << " - Sugeneruota transakciju: " << transactions.size() << endl;
    cout << " - Bendra pervesta suma: " << totalAmount << endl;
    cout << " - Vidutine suma: " << (uint64_t)avgAmount << endl;
    cout << " - Didziausia suma: " << maxAmount << endl;
    cout << " - Maziausia suma: " << minAmount << endl;
    cout << "===============================" << endl << endl;
}

// Writing to file
void ToFileUsers(const vector<User>& users) {
    ofstream outFile("data/users.txt");
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo vartotojams!" << endl;
        return;
    } else {
    outFile << "=== Vartotoju generavimas ===" << endl;
        for (const auto& u : users) {
            outFile << "Vardas: " << u.getName() << " | Raktas: " << u.getKey() << " | Balansas: " << u.getBalance() << endl;
        }
    outFile << "Sugeneruota vartotoju: " << users.size() << endl;
    outFile.close();
    }
}

void ToFileTransactions(const vector<Transaction>& transactions) {
    ofstream outFile("data/transactions.txt");
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo transakcijoms!" << endl;
        return;
    } else {
        outFile << "=== Transakciju generavimas ===" << endl;
        for (const auto& t : transactions) {
            outFile << t.getID() << " | Siuntejas: " << t.getSender() << " -> Gavejas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
        }
        outFile << "Sugeneruota transakciju: " << transactions.size() << endl;
        outFile.close();
    }
}

void ToFileBlocks (const Block& block, const vector<Transaction>& confirmedTransactions, int size, int difficulty) {
    ofstream outFile("data/blocks.txt", ios::app);
        if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo blokams!" << endl;
        return;
    } else {
        outFile  << "==============================" << endl;
        outFile  << "Blokas #" << size << endl;
        outFile  << "Versija: " << block.getVersion() << endl;
        outFile  << "Ankstesnio bloko hash: " << block.getPrevHash() << endl;
        outFile  << "Bloko hash: " << block.getHash() << endl;
        outFile  << "Nonce: " << block.getNonce() << endl;
        outFile  << "Sudetingumas: " << difficulty << endl;
        time_t t = block.getTime();
        outFile << "Laikas: " << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S") << endl;
        outFile << "--- Transakcijos ---" << endl;
        for (const auto& t : confirmedTransactions) {
            outFile << t.getID() << " | Siuntejas: " << t.getSender() << " -> Gavejas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
        }
        outFile << "==============================" << endl << endl;
        outFile.close();
    }
}

// Main functions
vector<User> GenerateUsers() {
    
    vector<User> users;
    users.reserve(1000);
   
    for (int i = 0; i < 1000; ++i) {
        string name = "User_" + to_string(i + 1);
        string key = "PUBKEY_";
        for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetter(rndGen));
        uint64_t  balance = rndBalance(rndGen);
        User u(name, key, balance);
        users.push_back(u);
    }

    ConsoleUserStats(users);
    ToFileUsers(users);
    return users;
}

vector<Transaction> GenerateTransactions(vector<User>& users) {
    vector<Transaction> transactions;
    transactions.reserve(10000);

    int generated = 0;
    while (generated < 10000) {
        int senderInd = rndUser(rndGen);
        int receiverInd = rndUser(rndGen);
        while (receiverInd == senderInd) receiverInd = rndUser(rndGen);

        uint64_t maxPossible = users[senderInd].getBalance();
        if (maxPossible == 0) continue;
        
        uint64_t amount = uniform_int_distribution<uint64_t>(1, maxPossible)(rndGen);
        Transaction t(users[senderInd].getKey(), users[receiverInd].getKey(), amount);
        transactions.push_back(t);
        ++generated;
    }

    ConsoleTransactionStats(transactions);
    ToFileTransactions(transactions);
    return transactions;
}

void MineBlock(vector<Transaction>& pendingTransactions, vector<Block>& chain, vector<User>& users, int difficulty) {
    if (pendingTransactions.empty()) return;
    
    size_t minSize = min<size_t>(100, pendingTransactions.size());
    shuffle(pendingTransactions.begin(), pendingTransactions.end(), rndGen);

    vector<Transaction> confirmedTransactions;
    confirmedTransactions.reserve(minSize);
    for (size_t i = 0; i < minSize; ++i) {
        confirmedTransactions.push_back(pendingTransactions.back());
        pendingTransactions.pop_back();
    }

    string prevHash = chain.empty() ? to_string(0) : chain.back().getHash();
    Block currentBlock(prevHash, confirmedTransactions, difficulty);

    auto start = chrono::high_resolution_clock::now();
    currentBlock.mineBlock();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    uint64_t totalAmount = 0;
    for (const auto& t : confirmedTransactions) {
        for (auto& u : users) {
            if (u.getKey() == t.getSender()) u.setBalance(u.getBalance() - t.getAmount());
            if (u.getKey() == t.getReceiver()) u.setBalance(u.getBalance() + t.getAmount());
        }
        totalAmount += t.getAmount();
    }
    chain.push_back(currentBlock);

    // Console log
    cout << " --- Iškastas blokas #" << chain.size() << " ---" << endl;
    cout << " - Kasimo laikas: " << duration.count() << " s." << endl;
    cout << " - Sudetingumas: " << difficulty << " | Nonce: " << currentBlock.getNonce() << endl;
    cout << " - Transakciju skaicius: " << confirmedTransactions.size() << " | Suma: " << totalAmount << endl;
    cout << "-----------------------------" << endl << endl;

    ToFileBlocks(currentBlock, confirmedTransactions, chain.size(), difficulty);
}

bool isChainValid (const vector<Block>& chain) {
    for (size_t i = 0; i < chain.size(); ++i) {
        const Block& current = chain[i];
        
        string merkle = current.calculateMerkleRoot();
        string data = current.getPrevHash() + merkle + to_string(current.getNonce()) + to_string(current.getDifficulty()) + to_string(current.getTime()) + current.getVersion();
        string calculatedHash = HashFun(data);

        if (current.getHash() != calculatedHash) {
            cerr << "Blokas #" << i + 1 << " turi neteisinga hash!" << endl;
            return false;
        }
        if (i > 0) {
            const Block& previous = chain[i - 1];
            if (current.getPrevHash() != previous.getHash()) {
                cerr << "Blokas #" << i + 1 << " neturi teisingo ankstesnio hash!" << endl;
                return false;
            }
        }
    }
    return true;
}

int main()
{
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BLOCKCHAIN =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    int difficulty = 3;

    vector<User> users = GenerateUsers();
    vector<Transaction> transactions = GenerateTransactions(users);
    vector<Block> chain;

    cout << "=== Bloku generavimas ===" << endl;
    ofstream("data/blocks.txt").close();
    while (!transactions.empty()) {
        MineBlock(transactions, chain, users, difficulty);
    }

    uint64_t totalNonce = 0;
    size_t transactionCount = 0;
    for (const auto& block : chain) {
        totalNonce += block.getNonce();
        transactionCount += block.getTransactions().size();
    }
    cout << " - Vidutinis nonce: " << (chain.size() ? totalNonce / chain.size() : 0) << endl;
    cout << " - Vidutinis transakciju kiekis: " << (chain.size() ? transactionCount / chain.size() : 0) << endl;
    cout << "* Kiekvieno bloko duomenys issaugoti i faila: data/bloks.txt" << endl;

    if (isChainValid(chain)) {
        cout << "|OK| Bloku grandine sukurta teisingai!" << endl;
    } else {
        cout << "|BAD| Bloku grandine sugadinta!" << endl;
    }

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BLOCKCHAIN END =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    return 0;
}