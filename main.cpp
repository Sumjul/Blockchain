#include "main.h"

int main()
{
    vector<User> users;
    users.reserve(1000);

    mt19937 gen(random_device{}());
    uniform_int_distribution<uint64_t> rndBalance(100, 1000000);
    uniform_int_distribution<int> rndLetters('A', 'Z');

    for (int i = 0; i < 1000; ++i)
    {
       string name = "User_" + to_string(i + 1);
       string key = "PUBKEY_";
       for (int j = 0; j < 8; ++j) key += static_cast<char>(rndLetters(gen));
       uint64_t  balance = rndBalance(gen);

       users.emplace_back(name, key, balance);
    }
    

    // 2 testas
    cout << "Generated Users: " << users.size() << endl;
    cout << "User: " << users[999].getName() << " | Key: " << users[999].getKey() << " | Balance: " << users[999].getBalance() << endl;

    return 0;
}
