#include "main.h"

int main()
{
    vector<User> users;

    // 1 testas
    User u1("Alice", "PUB_12345678", 5000.0);
    users.push_back(u1);

    cout << "User: " << users[0].name << " | Key: " << users[0].publicKey << " | Balance: " << users[0].balance << endl;

    return 0;
}
