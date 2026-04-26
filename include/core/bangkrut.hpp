#pragma once

class User;
class Game;

class Bangkrut{
public:
    Bangkrut();
    ~Bangkrut();
    
    bool isPayable(const User& user, int amount) const;
    std::vector<Properti*> executeBangkrut(User& debtor, User* creditor, Game* game);
    void likuidasi(User& user, int amount);

private:
    void transferToPlayer(User& debtor, User& creditor); // Bangkrut ke pemain lain
    std::vector<Properti*> transferToBank(User& debtor); // Bangkrut ke bank
};