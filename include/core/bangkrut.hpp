#pragma once

#include "user.hpp"
#include "game.hpp"

class Bangkrut{
public:
    Bangkrut();
    ~Bangkrut();
    
    bool isPayable(const User& user, int amount) const;
    void executeBangkrut(User& debtor, User* creditor, Game* game);

private:
    void transferToPlayer(User& debtor, User& creditor); // Bangkrut ke pemain lain
    void transferToBank(User& debtor, Game* game); // Bangkrut ke bank
};