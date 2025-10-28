#include "main.h"

// Randomizers
mt19937 rndGen(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(0, 999);
uniform_int_distribution<int> rndLetter('A', 'Z');

// Console loging
void ConsoleUserStats(const vector<User>& users) {
    cout << "=== VARTOTOJAI ===" << endl;
    if (users.empty()) {
        cout << " - Nera vartotoju." << endl;
        return;
    } else {
        uint64_t totalBalance = 0, maxBalance = 0, minBalance = UINT64_MAX;
        for (const auto& u : users) {
            totalBalance += u.getBalance();
            maxBalance = max(maxBalance, u.getBalance());
            minBalance = min(minBalance, u.getBalance());
        }
        cout << " - Viso vartotoju: " << users.size() << endl;
        cout << " - Bendras balansas: " << totalBalance << endl;
        cout << " - Vidutinis balansas: " << totalBalance / users.size() << endl;
        cout << " - Didziausias balansas: " << maxBalance << endl;
        cout << " - Maziausias balansas: " << minBalance << endl;
        cout << " * Kiekvieno vartotojo duomenys issaugoti i faila: data/users.txt" << endl;
    }
    cout << "=============================" << endl << endl;
}

void ConsoleTransactionStats(const vector<Transaction>& transactions) {
    cout << "=== TRANSAKCIJOS ===" << endl;
    if (transactions.empty()) {
        cout << " - Nera transakciju." << endl;
        return;
    } else {
        uint64_t totalAmount = 0, maxAmount = 0, minAmount = UINT64_MAX;
        for (const auto& t : transactions) {
            totalAmount += t.getAmount();
            maxAmount = max(maxAmount, t.getAmount());
            minAmount = min(minAmount, t.getAmount());
        }
        cout << " - Viso transakciju: " << transactions.size() << endl;
        cout << " - Bendra suma: " << totalAmount << endl;
        cout << " - Vidutine suma: " << totalAmount / transactions.size() << endl;
        cout << " - Didziausia suma: " << maxAmount << endl;
        cout << " - Maziausia suma: " << minAmount << endl;
        cout << " * Kiekvienos transakcijos duomenys issaugoti i faila: data/transactions.txt" << endl;
    }
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
        for (auto& u : users) {
            outFile << u.getName() << "," << u.getKey() << "," << u.getBalance() << endl;
        }
    outFile << "Viso vartotoju: " << users.size() << endl;
    }
}

void ToFileTransactions(const vector<Transaction>& transactions) {
    ofstream outFile("data/transactions.txt");
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su transakcijomis!" << endl;
        return;
    }
    outFile << "=== Transakciju sarasas (transakcijos hash, siuntejas, gavejas, suma) ===" << endl;
    for (auto& t : transactions) {
        outFile << t.getID() << "," << t.getSender() << "," << t.getReceiver() << "," << t.getAmount() << endl;
    }
    outFile << "Viso transakciju: " << transactions.size() << endl;
}

void ToFileBlocks (const Block& block, const vector<Transaction>& confirmedTransactions, int size, int difficulty) {
    ofstream outFile("data/blocks.txt", ios::app);
    if (!outFile.is_open()) {
        cerr << "Nepavyko atidaryti failo su blokais!" << endl;
        return;
    }
    time_t t = block.getTime();
    outFile  << "==============================" << endl;
    outFile  << "Blokas #" << size << endl;
    outFile  << "Versija: " << block.getVersion() << endl;
    outFile  << "Ankstesnio bloko hash: " << block.getPrevHash() << endl;
    outFile  << "Bloko hash: " << block.getHash() << endl;
    outFile  << "Nonce: " << block.getNonce() << endl;
    outFile  << "Sudetingumas: " << difficulty << endl;
    outFile << "Laikas: " << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S") << endl;
    outFile << "--- Transakcijos ---" << endl;
    for (const auto& t : confirmedTransactions) {
        outFile << t.getID() << " | Siuntejas: " << t.getSender() << " -> Gavejas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
    }
    outFile << "==============================" << endl << endl;
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
        if (line.empty() || line[0] == '=') continue;
        stringstream ss(line);
        vector<string> parts;
        string part;
        while (getline(ss, part, ',')) parts.push_back(part);
        if (parts.size() < 2) continue;

        string name = parts[0];
        string key;
        uint64_t balance = 0;

        if (parts.size() == 2) {
            key = "PUBKEY_";
            for (int i = 0; i < 16; ++i) key += static_cast<char>(rndLetter(rndGen));
            balance = stoull(parts[1]);
        } else if (parts.size() == 3) {
            key = parts[1];
            balance = stoull(parts[2]);
        }
        users.emplace_back(name, key, balance);
    }

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
    for (auto& u : users) nameToKey[u.getName()] = u.getKey();

    string line;
    while (getline(inFile, line)) {
        if (line.empty() || line[0] == '=') continue;
        stringstream ss(line);
        vector<string> parts;
        string part;
        while (getline(ss, part, ',')) parts.push_back(part);
        if (parts.size() < 3) continue;

        string sender, receiver;
        uint64_t amount = 0;
        string hash;
        if (parts.size() == 3) {
            sender = parts[0];
            receiver = parts[1];
            amount = stoull(parts[2]);
        } else if (parts.size() == 4) {
            hash = parts[0];
            sender = parts[1];
            receiver = parts[2];
            amount = stoull(parts[3]);
        }
        string senderKey = nameToKey.count(sender) ? nameToKey[sender] : sender;
        string receiverKey = nameToKey.count(receiver) ? nameToKey[receiver] : receiver;

        transactions.emplace_back(senderKey, receiverKey, amount);
    }

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
        users.emplace_back(name, key, balance);
    }

    ConsoleUserStats(users);
    ToFileUsers(users);
    return users;
}

vector<Transaction> GenerateTransactions(vector<User>& users) {
    vector<Transaction> transactions;
    transactions.reserve(20000);

    while (transactions.size() < 20000) {
        int senderInd = rndUser(rndGen);
        int receiverInd = rndUser(rndGen);
        while (receiverInd == senderInd) receiverInd = rndUser(rndGen);
        
        uint64_t amount = uniform_int_distribution<uint64_t>(1, users[senderInd].getBalance())(rndGen);
        transactions.emplace_back(users[senderInd].getKey(), users[receiverInd].getKey(), amount);
    }

    ConsoleTransactionStats(transactions);
    return transactions;
}

void MineBlock(vector<Transaction>& pendingTransactions, vector<Block>& chain, vector<User>& users, int difficulty, const unordered_map<string,size_t>& keyIndex) {
    if (pendingTransactions.empty()) return;
    vector<Transaction> confirmedTransactions;
    size_t skipped = 0;

    auto begin = pendingTransactions.begin();
    while (confirmedTransactions.size() < 100 && begin != pendingTransactions.end()) {
        auto &t = *begin;

        auto itSender = keyIndex.find(t.getSender());
        auto itReceiver = keyIndex.find(t.getReceiver());
        if (itSender == keyIndex.end() || itReceiver == keyIndex.end() || users[itSender->second].getBalance() < t.getAmount()) {
            ++skipped;
            begin = pendingTransactions.erase(begin);
            continue;
        }

        size_t sInd = itSender->second, rInd = itReceiver->second;
        uint64_t amount = t.getAmount();

        users[sInd].setBalance(users[sInd].getBalance() - amount);
        users[rInd].setBalance(users[rInd].getBalance() + amount);

        confirmedTransactions.emplace_back(t);
        begin = pendingTransactions.erase(begin);
    }

    if (confirmedTransactions.empty() && !pendingTransactions.empty()) {
        cerr << "Nera galiojanciu transakciju bloko formavimui arba visi siuntejai neturi pakankamai lesu." << endl;
        pendingTransactions.clear();
        return;
    }

    string prevHash = chain.empty() ? "0" : chain.back().getHash();
    Block block(prevHash, confirmedTransactions, difficulty);

    auto start = chrono::high_resolution_clock::now();
    block.mineBlock();
    auto end = chrono::high_resolution_clock::now();
    
    chain.emplace_back(block);

    // Console log
    uint64_t totalAmount = 0;
    double timeTaken = chrono::duration<double>(end - start).count();
    uint64_t transactionSpeed = confirmedTransactions.size() / timeTaken;
    for (const auto &t : confirmedTransactions) totalAmount += t.getAmount();

    cout << " ------------ Iškastas blokas #" << chain.size() << " ------------" << endl;
    cout << " - Kasimo laikas: " << timeTaken << " s." << endl;
    cout << " - Greitis: " << transactionSpeed << " transakciju per sekunde" << endl;
    cout << " - Sudetingumas: " << difficulty << " | Nonce: " << block.getNonce() << endl;
    cout << " - Transakciju skaicius: " << confirmedTransactions.size() << " | Suma: " << totalAmount << endl;
    cout << " - Praleistos negaliojančios transakcijos: " << skipped << endl << endl;

    ToFileBlocks(block, confirmedTransactions, chain.size(), difficulty);
}

bool isChainValid (const vector<Block>& chain) {
    if (chain.size() < 1) return false;
    for (size_t i = 0; i < chain.size(); ++i) {
        const Block& current = chain[i];
        string expectedHash = HashFun(current.getPrevHash() + current.calculateMerkleRoot() + to_string(current.getNonce()) + to_string(current.getDifficulty()) + to_string(current.getTime()) + current.getVersion());

        if (current.getHash() != expectedHash) {
            cerr << "Blokas #" << i + 1 << " turi neteisinga hash!" << endl;
            return false;
        }
        if (i > 0 && current.getPrevHash() != chain[i - 1].getHash()) {
            cerr << "Blokas #" << i + 1 << " neturi teisingo ankstesnio hash!" << endl;
            return false;
        }
    }
    return true;
}

int main()
{
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BLOCKCHAIN =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    int difficulty = 3, choice;
    vector<User> users;
    vector<Transaction> transactions;
    vector<Block> chain;

    cout << "Pasirinkite kaip generuoti bloku grandine, ivesdami skaiciu:" << endl;
    cout << " 0 - atsitiktinai generuoti 1000 vartotoju ir apie 10 000 transakciju" << endl;
    cout << " 1 - perskaityti vartotojus ir transakcijas is failu" << endl;
    cin >> choice;
    if (choice == 0) {
        ofstream("data/users.txt").close();
        ofstream("data/transactions.txt").close();
        users = GenerateUsers();
        transactions = GenerateTransactions(users); 
    } else if (choice == 1) {
        users = LoadUsers();
        transactions = LoadTransactions(users);
    } else {
        cerr << "Neteisingas pasirinkimas!" << endl;
        return 1;
    }

    size_t initialTransactions = transactions.size();
    unordered_map<string,size_t> keyIndex;
    for (size_t i = 0; i < users.size(); ++i) keyIndex[users[i].getKey()] = i;

    ofstream("data/blocks.txt").close();
    cout << "=== BLOKU GENERAVIMAS ===" << endl;
    while (!transactions.empty()) {
        MineBlock(transactions, chain, users, difficulty, keyIndex);
    }
    ToFileUsers(users);

    vector<Transaction> confirmedTransactions;
    for (const auto& block : chain) {
        const auto& transactions = block.getTransactions();
        confirmedTransactions.insert(confirmedTransactions.end(), transactions.begin(), transactions.end());
    }
    ToFileTransactions(confirmedTransactions);

    size_t totalNonce = 0, totalTransactions = 0;
    for (const auto& block : chain) {
        totalNonce += block.getNonce();
        totalTransactions += block.getTransactions().size();
    }
    cout << "=== BLOKU GRANDINES SANTRAUKA ===" << endl;
    cout << " - Sukurta bloku: " << chain.size() << endl;
    cout << " - Patvirtinta transakcijų: " << confirmedTransactions.size() << endl;
    cout << " - Vidutinis visu bloku nonce: " << (chain.empty() ? 0 : totalNonce / chain.size()) << endl;
    cout << " - Vidutinis transakciju kiekis bloke: " << (chain.empty() ? 0 : totalTransactions / chain.size()) << endl;
    cout << " * Kiekvieno bloko duomenys issaugoti i faila: data/blocks.txt" << endl << endl;

    ConsoleUserStats(users);
    ConsoleTransactionStats(confirmedTransactions);

    cout << (isChainValid(chain) ? "|OK| Grandine teisinga!" : "|BAD| Grandine sugadinta!") << endl;
    cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- BLOCKCHAIN END =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
    return 0;
}