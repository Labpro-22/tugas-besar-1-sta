#include "core/bangkrut.hpp"
#include "core/properti.hpp"
#include "core/user.hpp"
#include "core/game.hpp"

Bangkrut::Bangkrut(){}
Bangkrut::~Bangkrut(){}

bool Bangkrut::isPayable(const User& user, int amount) const{
    if (user.getUang() >= amount){
        return true;
    }

    else{
        int jumlahMaksDana = user.getUang();
        
        for (Properti* p : user.getListProperti()){
            if (p != nullptr){
                if (p->getStatus() == PropStatus::OWNED){
                    int nilaiGadai = p->getNilaiGadai();
                    int nilaiJual = p->getHargaBeli();
                    Street* s = dynamic_cast<Street*>(p);
                    
                    if (s != nullptr){
                        if (s->isHotel()){
                            nilaiJual += s->getHargaHotel() * 0.5;
                        }
                        else{
                            nilaiJual += (s->getJumlahBangunan() * s->getHargaBangunan()) / 2;
                        }
                    }
    
                    // ini pilih otomatis better di gadai atau jual
                    jumlahMaksDana += std::max(nilaiGadai, nilaiJual);
                }    
            }
        }

        return jumlahMaksDana >= amount;
        // Kalo true, berarti masih bisa likuidasi, kalo false ga usah likuidasi (udah pasti ga cukup)
    }
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

    if (game != nullptr && !propertiUntukDilelang.empty()){
        std::cout << "\nSeluruh properti dari " << debtor.getUsername() << " akan dilelang:\n";

        for (Properti* p : propertiUntukDilelang){
            if (p != nullptr){
                std::cout << "\n -> Lelang: " << p->getNama() << " (" << p->getKode() << ") ...\n";
                game->mulaiLelang(p, &debtor);
            }
        }
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

void Bangkrut::likuidasi(User& user, int amount){
    std::cout << "\n=== Panel Likuidasi ===\n";
 
    while (user.getUang() < amount) {
        const auto& daftarProperti = user.getListProperti();

        // Pisahin mana yang bisa dijual/gadai
        std::vector<Properti*> bisaJual;
        std::vector<Properti*> bisaGadai;

        for (Properti* p : daftarProperti) {
            if (p == nullptr || p->getStatus() != PropStatus::OWNED) continue;
            bisaJual.push_back(p);
            bisaGadai.push_back(p);
        }

        if (bisaJual.empty() && bisaGadai.empty()) {
            // Udah ga bisa likuidasi, udah abis asetnya
            break;
        }

        std::cout << "\nUang kamu: M" << user.getUang()
                  << " | Kewajiban: M" << amount << "\n";

        int nomor = 1;
        std::vector<std::pair<char, Properti*>> pilihan; // 'J'=jual, 'G'=gadai

        if (!bisaJual.empty()) {
            std::cout << "[Jual ke Bank]\n";
            for (Properti* p : bisaJual) {
                int nilaiJual = p->getHargaBeli();
                Street* s = dynamic_cast<Street*>(p);
                if (s != nullptr) {
                    if (s->isHotel())
                        nilaiJual += s->getHargaHotel() / 2;
                    else
                        nilaiJual += (s->getJumlahBangunan() * s->getHargaBangunan()) / 2;
                }
                std::cout << nomor << ". " << p->getNama() << " (" << p->getKode() << ")" << " -> Harga Jual: M" << nilaiJual << "\n";
                pilihan.push_back({'J', p});
                nomor++;
            }
        }

        if (!bisaGadai.empty()) {
            std::cout << "[Gadaikan]\n";
            for (Properti* p : bisaGadai) {
                std::cout << nomor << ". " << p->getNama() << " (" << p->getKode() << ")" << " -> Nilai Gadai: M" << p->getNilaiGadai() << "\n";
                pilihan.push_back({'G', p});
                nomor++;
            }
        }

        std::cout << "0. Selesai memilih\n";
        std::cout << "Pilih aksi: ";
        int input = -1;
        std::cin >> input;

        if (input == 0) break;

        if (input < 1 || input > static_cast<int>(pilihan.size())) {
            std::cout << "[ERROR] Pilihan tidak valid.\n";
            continue;
        }

        char aksi      = pilihan[input - 1].first;
        Properti* prop = pilihan[input - 1].second;

        if (aksi == 'J') { // jual
            int nilaiJual = prop->getHargaBeli();
            Street* s = dynamic_cast<Street*>(prop);
            if (s != nullptr) {
                if (s->isHotel()) {
                    nilaiJual += s->getHargaHotel() / 2;
                    s->setHotel(false);
                } else if (s->getJumlahBangunan() > 0) {
                    nilaiJual += (s->getJumlahBangunan() * s->getHargaBangunan()) / 2;
                    s->setJumlahRumah(0);
                }
            }
            user += nilaiJual;
            prop->setOwner(nullptr);
            prop->setStatus(PropStatus::BANK);
            std::cout << "[INFO] " << prop->getNama() << " terjual ke Bank. " << "Kamu menerima M" << nilaiJual << ".\n";
            std::cout << "[INFO] Uang kamu sekarang: M" << user.getUang() << "\n";

        } else { // gadai
            int nilaiGadai = prop->getNilaiGadai();

            Street* s = dynamic_cast<Street*>(prop);
            if (s != nullptr) {
                auto grupWarna = user.getStreetByColor(prop->getWarna());
                int duitBangunan = 0;
                for (Street* anggota : grupWarna) {
                    if (anggota->isHotel()) {
                        duitBangunan += anggota->getHargaHotel() / 2;
                        anggota->setHotel(false);
                    } else if (anggota->getJumlahBangunan() > 0) {
                        duitBangunan += (anggota->getJumlahBangunan()
                                         * anggota->getHargaBangunan()) / 2;
                        anggota->setJumlahRumah(0);
                    }
                }
                if (duitBangunan > 0) {
                    user += duitBangunan;
                    std::cout << "[INFO] Bangunan di color group dijual. " << "Kamu menerima M" << duitBangunan << ".\n";
                }
            }

            user += nilaiGadai;
            prop->setStatus(PropStatus::MORTGAGED);
            std::cout << "[INFO] " << prop->getNama() << " berhasil digadaikan. " << "Kamu menerima M" << nilaiGadai << ".\n";
            std::cout << "[INFO] Uang kamu sekarang: M" << user.getUang() << "\n";
        }
    }
}