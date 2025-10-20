#include "main.h"

int main()
{
    vector<User> users;

    // 1 testas
    User u1("Alice", "PUB_12345678", 5000);
    users.push_back(u1);

    cout << "User: " << users[0].getName() << " | Key: " << users[0].getKey() << " | Balance: " << users[0].getBalance() << endl;

    return 0;
}
