#include "main.h"

// Randomizers
mt19937 rndGen(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(0, 999);
uniform_int_distribution<int> rndLetter('A', 'Z');

// Functions
vector<User> GenerateUsers() {
    cout << "=== Naudotoju generavimas ===" << endl;
    vector<User> users;
    users.reserve(1000);
    ofstream outFile("data/users.txt");
    if (outFile.is_open()) {
        outFile << "=== Vartotoju generavimas ===" << endl;

        uint64_t totalBalance = 0, maxBalance = 0, minBalance = UINT64_MAX;
        for (int i = 0; i < 1000; ++i) {
            string name = "User_" + to_string(i + 1);
            string key = "PUBKEY_";
            for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetter(rndGen));
            uint64_t  balance = rndBalance(rndGen);

            totalBalance += balance;
            if (balance > maxBalance) maxBalance = balance;
            if (balance < minBalance) minBalance = balance;
            User u(name, key, balance);
            users.push_back(u);
            outFile << "Vardas: " << u.getName() << " | Raktas: " << u.getKey() << " | Balansas: " << u.getBalance() << endl;
        }
        outFile << "Sugeneruota vartotoju: " << users.size() << endl;
        outFile.close();
        double avgBalance = static_cast<double>(totalBalance) / users.size();
        cout << "[OK] Sugeneruota vartotoju: " << users.size() << endl;
        cout << " - Bendras balansas: " << totalBalance << endl;
        cout << " - Vidutinis balansas: " << (uint64_t)avgBalance << endl;
        cout << " - Maksimalus balansas: " << maxBalance << endl;
        cout << " - Minimalus balansas: " << minBalance << endl;
        cout << " * Kiekvieno vartotojo duomenys issaugoti i faila: data/users.txt" << endl;
        cout << "==============================" << endl << endl;
    } else {
        outFile << "Nera failo duomenu irasymui!" << endl;
        return vector<User>();
    }
    return users;
}

vector<Transaction> GenerateTransactions(vector<User>& users) {
    cout << "=== Transakciju generavimas ===" << endl;
    vector<Transaction> transactions;
    transactions.reserve(10000);
    ofstream outFile("data/transactions.txt");
    if (outFile.is_open()) {
        outFile << "=== Transakciju generavimas ===" << endl;

        uint64_t totalAmount = 0, maxAmount = 0;
        int generated = 0, skipped = 0;
        while (generated < 10000) {
            int senderInd = rndUser(rndGen);
            int receiverInd = rndUser(rndGen);
            while (receiverInd == senderInd) receiverInd = rndUser(rndGen);

            uint64_t maxPossible = users[senderInd].getBalance();
            if (maxPossible == 0) {
                skipped++;
                continue;
            }
            uint64_t amount = uniform_int_distribution<uint64_t>(1, maxPossible)(rndGen);

            Transaction t(users[senderInd].getKey(), users[receiverInd].getKey(), amount);
            transactions.push_back(t);
            
            totalAmount += amount;
            if (amount > maxAmount) maxAmount = amount;
            outFile << t.getID() << " | Siuntejas: " << t.getSender() << " -> Gavejas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
            ++generated;
        }
        double avgAmount = static_cast<double>(totalAmount) / transactions.size();
        outFile << "Sugeneruota transakciju: " << transactions.size() << endl;
        outFile.close();

        cout << "[OK] Sugeneruota transakciju: " << transactions.size() << endl;
        cout << " - Bendra pervesta suma: " << totalAmount << endl;
        cout << " - Vidutinė suma: " << (uint64_t)avgAmount << endl;
        cout << " - Didžiausia suma: " << maxAmount << endl;
        cout << " - Praleista dėl nulinio balanso: " << skipped << endl;
        cout << " * Kiekvienos transakcijos duomenys issaugoti i faila: data/transactions.txt" << endl;
        cout << "==============================" << endl << endl;
   
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

    cout << "Blokas #" << chain.size() << " iškastas per " << duration.count() << " s." << endl;
    cout << "Transakciju skaicius: " << confirmedTransactions.size() << endl;    
    cout << "Transakciju suma: " << totalAmount << endl;

    cout << "Sudetingumas: " << difficulty << " | Nonce: " << currentBlock.getNonce() << endl;
    cout << "==============================" << endl;

    ofstream outFile("data/blocks.txt", ios::app);
    if (outFile.is_open()) {
        ostringstream log;
        outFile  << "==============================" << endl;
        outFile  << "Blokas #" << chain.size() << endl;
        outFile  << "Versija: " << currentBlock.getVersion() << endl;
        outFile  << "Ankstesnio bloko hash: " << prevHash << endl;
        outFile  << "Bloko hash: " << currentBlock.getHash() << endl;
        outFile  << "Nonce: " << currentBlock.getNonce() << endl;
        outFile  << "Sudetingumas: " << difficulty << endl;
        time_t t = currentBlock.getTime();
        outFile << "Laikas: " << put_time(localtime(&t), "%Y-%m-%d %H:%M:%S") << endl;
        outFile << "--- Transakcijos ---" << endl;
        for (const auto& t : confirmedTransactions) {
            outFile << t.getID() << " | Siuntejas: " << t.getSender() << " -> Gavejas: " << t.getReceiver() << " | Suma: " << t.getAmount() << endl;
        }
        outFile << "==============================" << endl << endl;
        outFile.close();
    } else {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return;
    }
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
    cout << "======= BLOCKCHAIN =======" << endl;
    int difficulty = 3;

    vector<User> users = GenerateUsers();
    vector<Transaction> transactions = GenerateTransactions(users);
    vector<Block> chain;
    ofstream outFile("data/blocks.txt");
    outFile.close();
    while (transactions.size() > 0) {
        MineBlock(transactions, chain, users, difficulty);
    }
    double totalTime = 0;
    uint64_t totalNonce = 0;
    size_t transactionCount = 0;
    for (const auto& block : chain) {
        totalNonce += block.getNonce();
        transactionCount += block.getTransactions().size();
    }
    cout << "Vidutinis nonce: " << (chain.size() ? totalNonce / chain.size() : 0) << endl;
    cout << "Vidutinis transakciju kiekis: " << (chain.size() ? transactionCount / chain.size() : 0) << endl;
    cout << "* Kiekvieno bloko duomenys issaugoti i faila: data/bloks.txt" << endl;
    if (isChainValid(chain)) {
        cout << "|OK| Bloku grandine teisinga!" << endl;
    } else {
        cout << "|BAD| Bloku grandine sugadinta!" << endl;
    }
    cout << "======= BLOCKCHAIN END =======" << endl;
    return 0;
}