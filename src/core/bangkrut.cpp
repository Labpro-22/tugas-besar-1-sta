#include "include/core/bangkrut.hpp"

Bangkrut::Bangkrut(){}
Bangkrut::~Bangkrut(){}

bool Bangkrut::isPayable(User& user, int amount){
    return user.
}
void Bangkrut::bayarHutang(User& debtor, int amount, User* creditor);
void Bangkrut::startLikuidasi(User& user, int amountNeeded);
void Bangkrut::deklarasiBangkrut(User& debtor, User* creditor);
void Bangkrut::transfer(User& debtor, User& creditor);
void Bangkrut::transfer(User& debtor);