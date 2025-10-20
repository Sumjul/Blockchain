#include "main.h"

// Randomizers
mt19937 rndDev(random_device{}());
uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
uniform_int_distribution<int> rndUser(1, 999);
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

    for (int i = 0; i < 1000; ++i)
    {
        string name = "User_" + to_string(i + 1);
        string key = "PUBKEY_";
        for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetter(rndDev));
        uint64_t  balance = rndBalance(rndDev);

        User u(name, key, balance);
        users.emplace_back(u);
        outFile << u.getName() << "," << u.getKey() << "," << u.getBalance() << endl;
        cout << "Vardas: " << u.getName() << " | Raktas: " << u.getKey() << " | Balansas: " << u.getBalance() << endl;
    }
    cout << "Is viso vartotoju sugeneruota: " << users.size() << endl;
    outFile.close();
    return users;
}

void GenerateTransaction(vector<User>& users) {
    vector<Transaction> transactions;
    transactions.reserve(10000);

    ofstream outFile("data/transactions.txt");
    if (!outFile.is_open()) {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return;
    }

    for (int i = 0; i < 10000; ++i)
    {
        int senderInd = rndUser(rndDev);
        int receiverInd = rndUser(rndDev);
        while (receiverInd == senderInd) receiverInd = rndUser(rndDev);

        uint64_t maxAmount = users[senderInd].getBalance();
        if (maxAmount == 0) continue;
        uint64_t amount = uniform_int_distribution<uint64_t>(1, maxAmount)(rndDev);
        users[senderInd].send(amount);
        users[receiverInd].receive(amount);

        Transaction t(users[senderInd].getKey(), users[receiverInd].getKey(), amount);
        transactions.emplace_back(t);
        outFile << t.getID() << "," << t.getSender() << "," << t.getReceiver() << "," << t.getAmount() << endl;
        cout << t.getID() << " | " << t.getSender() << " -> " << t.getReceiver() << " : " << t.getAmount() << endl;
    }
    cout << "Is viso transakciju sugeneruota: " << transactions.size() << endl;
    outFile.close();
}

int main()
{
    vector<User> users = GenerateUsers();
    GenerateTransaction(users);
    return 0;
}
