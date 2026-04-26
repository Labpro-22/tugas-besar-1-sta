#include "core/bangkrut.hpp"
#include "core/properti.hpp"
#include "core/user.hpp"
#include "core/game.hpp"

Bangkrut::Bangkrut(){}
Bangkrut::~Bangkrut(){}

bool Bangkrut::isPayable(const User& user, int amount) const{
    return user.getTotalKekayaan() >= amount;
}

std::vector<Properti*> Bangkrut::executeBangkrut(User& debtor, User* creditor, Game* game){
    std::vector<Properti*> propertiUntukDilelang;
    
    if (creditor != nullptr){
        transferToPlayer(debtor, *creditor);
    }

    else{
        propertiUntukDilelang = transferToBank(debtor);
    }

    if (game != nullptr){
        game->leave(debtor);
    }

    return propertiUntukDilelang;
}

void Bangkrut::transferToPlayer(User& debtor, User& creditor){ // Bangkrut ke pemain lain
    creditor += (debtor.getUang());
    debtor -= (debtor.getUang());

    std::vector<Properti*> copyList = debtor.getListProperti();
    for (Properti* p : copyList) {
        if (p != nullptr) {
            p->setOwner(&creditor); 
        }
    }
} 

std::vector<Properti*> Bangkrut::transferToBank(User& debtor){ // Bangkrut ke bank
    std::vector<Properti*> daftarLelang;
    debtor -= (debtor.getUang());

    std::vector<Properti*> copyList = debtor.getListProperti();
    for (Properti* p : copyList) {
        if (p != nullptr) {
            p->setOwner(nullptr);
            p->setStatus(PropStatus::BANK);
            
            Street* s = dynamic_cast<Street*>(p);
            if (s != nullptr) {
                s->setHotel(false);
                s->setJumlahRumah(0);
            }

            daftarLelang.push_back(p);
        }
    }

    return daftarLelang;
} 