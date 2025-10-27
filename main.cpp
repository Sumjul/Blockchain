#include "main.h"

// Randomizers
mt19937 rndGen(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(0, 999);
uniform_int_distribution<int> rndLetter('A', 'Z');

// Console loging
void ConsoleUserStats(const vector<User>& users) {
    if (users.empty()) {
        cout << "=== VARTOTOJAI ===" << endl;
        cout << " - Nera vartotoju." << endl;
        cout << "=============================" << endl << endl;
        return;
    }

    uint64_t totalBalance = 0, maxBalance = 0, minBalance = UINT64_MAX;
    for (const auto& u : users) {
        totalBalance += u.getBalance();
        maxBalance = max(maxBalance, u.getBalance());
        minBalance = min(minBalance, u.getBalance());
    }
    double avgBalance = static_cast<double>(totalBalance) / users.size();

    cout << "=== VARTOTOJAI ===" << endl;
    cout << " - Viso vartotoju: " << users.size() << endl;
    cout << " - Bendras balansas: " << totalBalance << endl;
    cout << " - Vidutinis balansas: " << (uint64_t)avgBalance << endl;
    cout << " - Didziausias balansas: " << maxBalance << endl;
    cout << " - Maziausias balansas: " << minBalance << endl;
    cout << " * Kiekvieno vartotojo duomenys issaugoti i faila: data/users.txt" << endl;
    cout << "=============================" << endl << endl;
}

void ConsoleTransactionStats(const vector<Transaction>& transactions) {
    if (transactions.empty()) {
        cout << "=== TRANSAKCIJOS ===" << endl;
        cout << " - Nera transakciju." << endl;
        cout << "=============================" << endl << endl;
        return;
    }

    uint64_t totalAmount = 0, maxAmount = 0, minAmount = UINT64_MAX;
    for (const auto& t : transactions) {
        totalAmount += t.getAmount();
        maxAmount = max(maxAmount, t.getAmount());
        minAmount = min(minAmount, t.getAmount());
    }
    double avgAmount = transactions.empty() ? 0 : static_cast<double>(totalAmount) / transactions.size();

    cout << "=== TRANSAKCIJOS ===" << endl;
    cout << " - Viso transakciju: " << transactions.size() << endl;
    cout << " - Bendra suma: " << totalAmount << endl;
    cout << " - Vidutine suma: " << (uint64_t)avgAmount << endl;
    cout << " - Didziausia suma: " << maxAmount << endl;
    cout << " - Maziausia suma: " << minAmount << endl;
    cout << " * Kiekvienos transakcijos duomenys issaugoti i faila: data/transactions.txt" << endl;
    cout << "===============================" << endl << endl;
}

// Writing to files
void ToFileUsers(const vector<User>& users) {
    ofstream outFile("data/users.txt");
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su vartotojais!" << endl;
        return;
    } else {
    outFile << "=== Vartotoju sarasas (vardas, raktas, balansas) ===" << endl;
        for (const auto& u : users) {
            outFile << u.getName() << "," << u.getKey() << "," << u.getBalance() << endl;
        }
    outFile << "Viso vartotoju: " << users.size() << endl;
    outFile.close();
    }
}

void ToFileTransactions(const vector<Transaction>& transactions) {
    ofstream outFile("data/transactions.txt");
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su transakcijomis!" << endl;
        return;
    } else {
        outFile << "=== Transakciju sarasas (transakcijos hash, siuntejas, gavejas, suma) ===" << endl;
        for (const auto& t : transactions) {
            outFile << t.getID() << "," << t.getSender() << "," << t.getReceiver() << "," << t.getAmount() << endl;
        }
        outFile << "Viso transakciju: " << transactions.size() << endl;
        outFile.close();
    }
}

void ToFileBlocks (const Block& block, const vector<Transaction>& confirmedTransactions, int size, int difficulty) {
    ofstream outFile("data/blocks.txt", ios::app);
        if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su blokais!" << endl;
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

// Reading from files
vector<User> LoadUsers() {
    vector<User> users;
    ifstream inFile("data/users.txt");
    if (!inFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su vartotojais! " << endl;
        return users;
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string name, balanceStr;
        if (getline(ss, name, ',') && getline(ss, balanceStr, ',')) {
            uint64_t balance = stoull(balanceStr);
            string key = "PUBKEY_";
            for (int i = 0; i < 16; ++i) key += static_cast<char>(rndLetter(rndGen));
            users.emplace_back(name, key, balance);
        }
    }

    inFile.close();
    ConsoleUserStats(users);
    return users;
}

vector<Transaction> LoadTransactions(const vector<User>& users) {
    vector<Transaction> transactions;
    ifstream inFile("data/transactions.txt");
    if (!inFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su trnsakcijomis! " << endl;
        return transactions;
    }

    unordered_map<string, string> nameToKey;
    for (const auto& u : users) {
        nameToKey[u.getName()] = u.getKey();
    }

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string sender, receiver, amountStr;
        if (getline(ss, sender, ',') && getline(ss, receiver, ',') && getline(ss, amountStr, ',')) {
            uint64_t amount = stoull(amountStr);
            string senderKey = nameToKey[sender];
            string receiverKey = nameToKey[receiver];
            transactions.emplace_back(senderKey, receiverKey, amount);
        }
    }

    inFile.close();
    ConsoleTransactionStats(transactions);
    return transactions;
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
        users.emplace_back(u);
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
        transactions.emplace_back(t);
        ++generated;
    }

    ConsoleTransactionStats(transactions);
    ToFileTransactions(transactions);
    return transactions;
}

void MineBlock(vector<Transaction>& pendingTransactions, vector<Block>& chain, vector<User>& users, int difficulty) {
    if (pendingTransactions.empty()) return;
    
    unordered_map<string, size_t> keyIndex;
    for (size_t i = 0; i < users.size(); ++i) keyIndex[ users[i].getKey() ] = i;

    size_t targetCount = min<size_t>(100, pendingTransactions.size());
    vector<Transaction> confirmedTransactions;
    confirmedTransactions.reserve(targetCount);
    vector<Transaction> remainingTransactions;
    remainingTransactions.reserve(pendingTransactions.size());
    size_t skipped_invalid = 0;
    for (auto &t : pendingTransactions) {
        if (confirmedTransactions.size() >= targetCount) {
            remainingTransactions.emplace_back(t);
            continue;
        }

        const string &senderKey = t.getSender();
        const string &receiverKey = t.getReceiver();
        uint64_t amount = t.getAmount();

        auto itSender = keyIndex.find(senderKey);
        auto itReceiver = keyIndex.find(receiverKey);

        if (itSender == keyIndex.end() || itReceiver == keyIndex.end()) {
            ++skipped_invalid;
            continue;
        }

        size_t sInd = itSender->second;
        size_t rInd = itReceiver->second;

        if (users[sInd].getBalance() < amount) {
            ++skipped_invalid;
            continue;
        }

        users[sInd].setBalance(users[sInd].getBalance() - amount);
        users[rInd].setBalance(users[rInd].getBalance() + amount);

        confirmedTransactions.push_back(t);
    }
    pendingTransactions.swap(remainingTransactions);

    if (confirmedTransactions.empty()) {
        cerr << "Nera galiojanciu transakciju bloko formavimui arba visi siuntejai neturi pakankamai lesu." << endl;
        return;
    }

    string prevHash = chain.empty() ? to_string(0) : chain.back().getHash();
    Block currentBlock(prevHash, confirmedTransactions, difficulty);

    auto start = chrono::high_resolution_clock::now();
    currentBlock.mineBlock();
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    
    chain.emplace_back(currentBlock);

    // Console log
    uint64_t totalAmount = 0;
    for (const auto &t : confirmedTransactions) totalAmount += t.getAmount();
    cout << " --- Iškastas blokas #" << chain.size() << " ---" << endl;
    cout << " - Kasimo laikas: " << duration.count() << " s." << endl;
    cout << " - Sudetingumas: " << difficulty << " | Nonce: " << currentBlock.getNonce() << endl;
    cout << " - Transakciju skaicius: " << confirmedTransactions.size() << " | Suma: " << totalAmount << endl;
    cout << " - Praleistos negaliojančios transakcijos: " << skipped_invalid << endl;
    cout << "-----------------------------" << endl << endl;

    ToFileBlocks(currentBlock, confirmedTransactions, chain.size(), difficulty);
    ToFileUsers(users);
    ToFileTransactions(confirmedTransactions);
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
    vector<User> users;
    vector<Transaction> transactions;
    vector<Block> chain;

    int choice;
    cout << "Pasirinkite kaip generuoti bloku grandine, ivesdami skaiciu:" << endl;
    cout << " 0 - atsitiktinai generuoti 1000 vartotoju ir 10 000 transakciju" << endl;
    cout << " 1 - perskaityti vartotojus ir transakcijas is failu" << endl;
    cin >> choice;
    if (choice == 0) {
        users = GenerateUsers();
        transactions = GenerateTransactions(users);
    } else if (choice == 1) {
        users = LoadUsers();
        transactions = LoadTransactions(users);
    } else {
        cerr << "Neteisingas pasirinkimas!" << endl;
        return 1;
    }

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
    cout << " * Kiekvieno bloko duomenys issaugoti i faila: data/blocks.txt" << endl;

    if (isChainValid(chain)) {
        cout << "|OK| Bloku grandine sukurta teisingai!" << endl;
    } else {
        cout << "|BAD| Bloku grandine sugadinta!" << endl;
    }

    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BLOCKCHAIN END =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    return 0;
}