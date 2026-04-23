#pragma once

#include "user.hpp"
#include "game.hpp"

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