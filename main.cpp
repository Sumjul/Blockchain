#include "main.h"

void GenerateUsers () {
    vector<User> users;
    users.reserve(1000);

    mt19937 rndDev(random_device{}());
    uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
    uniform_int_distribution<int> rndLetters('A', 'Z');

    ofstream outFile("data/users.txt");
    if (!outFile.is_open()) {
        cerr << "Nera failo duomenu irasymui!" << endl;
        return;
    }

    for (int i = 0; i < 1000; ++i)
    {
        string name = "User_" + to_string(i + 1);
        string key = "PUBKEY_";
        for (int j = 0; j < 16; ++j) key += static_cast<char>(rndLetters(rndDev));
        uint64_t  balance = rndBalance(rndDev);

        users.emplace_back(name, key, balance);
        outFile << users[i].getName() << "," << users[i].getKey() << "," << users[i].getBalance() << endl;
        cout << "Vardas: " << users[i].getName() << " | Raktas: " << users[i].getKey() << " | Balansas: " << users[i].getBalance() << endl;
    }
    cout << "Is viso vartotoju sugeneruota: " << users.size() << endl;
    outFile.close();
}

int main()
{
    GenerateUsers();
    return 0;
}
