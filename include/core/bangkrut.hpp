#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
class User;
class Game;
class Properti;

class Bangkrut{
public:
    Bangkrut();
    ~Bangkrut();
    
    bool isPayable(const User& user, int amount) const;
    std::vector<Properti*> executeBangkrut(User& debtor, User* creditor, Game* game);

private:
    void transferToPlayer(User& debtor, User& creditor); // Bangkrut ke pemain lain
    std::vector<Properti*> transferToBank(User& debtor); // Bangkrut ke bank
};