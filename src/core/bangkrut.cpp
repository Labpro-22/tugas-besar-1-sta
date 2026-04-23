#include "../../include/core/bangkrut.hpp"
#include "../../include/core/properti.hpp"

Bangkrut::Bangkrut(){}
Bangkrut::~Bangkrut(){}

bool Bangkrut::isPayable(const User& user, int amount) const{
    return user.getTotalKekayaan() >= amount;
}

void Bangkrut::executeBangkrut(User& debtor, User* creditor, Game* game){
    if (creditor != nullptr){
        transferToPlayer(debtor, *creditor);
    }

    else{
        transferToBank(debtor, game);
    }

    if (game != nullptr){
        game->leave(debtor);
    }
}

void Bangkrut::transferToPlayer(User& debtor, User& creditor){ // Bangkrut ke pemain lain
    creditor += (debtor.getUang());
    debtor -= (debtor.getUang());

    const std::vector<Properti*>& listProperti = debtor.getListProperti();
    for (Properti* p : listProperti) {
        if (p != nullptr) {
            p->setOwner(&creditor); 
        }
    }
} 

void Bangkrut::transferToBank(User& debtor, Game* game){ // Bangkrut ke bank
    debtor -= (debtor.getUang());

    const std::vector<Properti*>& listProperti = debtor.getListProperti();
    for (Properti* p : listProperti) {
        if (p != nullptr) {
            // Ubah kepemilikan menjadi Bank
            p->setOwner(nullptr);
            p->setStatus(PropStatus::BANK); 
            
            // Hancurkan bangunan jika bertipe Street
            Street* s = dynamic_cast<Street*>(p);
            if (s != nullptr) {
                s->setJumlahRumah(0);
                s->setHotel(false);
            }
            
            if (game != nullptr) {
                game->mulaiLelang(p, nullptr); 
            }
        }
    }
} 