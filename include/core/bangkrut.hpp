#pragma once

#include "user.hpp"

class Bangkrut{
public:
    Bangkrut();
    ~Bangkrut();
    
    bool isPayable(User& user, int amount);
    void bayarHutang(User& debtor, int amount, User* creditor);
    void startLikuidasi(User& user, int amountNeeded);
    void deklarasiBangkrut(User& debtor, User* creditor);
    void transfer(User& debtor, User& creditor);
    void transfer(User& debtor);
};