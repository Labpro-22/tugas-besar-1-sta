#include "../../include/core/kartu.hpp"
#include "../../include/core/game.hpp"
#include "../../include/core/user.hpp"
#include "../../include/core/board.hpp"
#include "../../include/core/petak.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>
#include <vector>

Kartu::Kartu() {
    this->nama = "Kartu Default";
    this->deskripsi = "Deskripsi Default";
}

Kartu::~Kartu() {}

// KartuAksi (Kesempatan & Dana Umum)
KartuAksi::KartuAksi() {}
KartuAksi::KartuAksi(TipeAksi kategori) : kategori(kategori){}
KartuAksi::~KartuAksi() {}

void KartuAksi::apply(Game* game, User& user) {
    // Kartu Kesempatan
    if (kategori == TipeAksi::Kesempatan){
        srand(time(NULL));
        int acak = rand() % 3; 
        
        std::cout << "[KARTU KESEMPATAN] Mengambil kartu...\n";

        if (acak == 0) {
            std::cout << "Kartu: \"Pergi ke stasiun terdekat.\"\n";
            int currentPos = user.getKoordinat();
            int boardSize = game->getBoard()->getSize();
            int closestStation = currentPos;
            bool found = false;

            for (int i = 1; i <= boardSize; ++i) {
                int checkPos = (currentPos + i) % boardSize;
                Petak* p = game->getBoard()->getPetakAt(checkPos);
                if (p != nullptr && p->getKategori() == "STASIUN") {
                    closestStation = checkPos;
                    found = true;
                    break;
                }
            }
            if (found) {
                std::cout << "Memindahkan " << user.getUsername() << " ke stasiun terdekat di indeks " << closestStation << ".\n";
                user.setKoordinat(closestStation);
            }
        } 
        else if (acak == 1) {
            std::cout << "Kartu: \"Mundur 3 petak.\"\n";
            user.move(-3); 
        } 
        else {
            std::cout << "Kartu: \"Masuk Penjara.\"\n";
            user.setKoordinat(game->getBoard()->getPenjaraIndex());
            user.setStatus("JAILED"); 
        }
    }

    // Kartu Dana Umum
    else if (kategori == TipeAksi::Dana_Umum){
        srand(time(NULL));
        int acak = rand() % 3;

        std::cout << "[KARTU DANA UMUM] Mengambil kartu...\n";

        if (acak == 0) {
            std::cout << "Kartu: \"Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.\"\n";
            int totalHadiah = 0;
            for(auto& p : game->getPemain()) {
                if(p.getUsername() != user.getUsername() && p.getStatus() != "BANKRUPT") {
                    if(p.getUang() >= 100) {
                        p -= (100);
                        totalHadiah += 100;
                    } else {
                        totalHadiah += p.getUang();
                        p -= (p.getUang());
                        std::cout << p.getUsername() << " tidak memiliki cukup uang untuk membayar hadiah penuh!\n";
                    }
                }
            }
            user += (totalHadiah);
            std::cout << "Anda mendapatkan total M" << totalHadiah << "!\n";
        } 
        else if (acak == 1) {
            std::cout << "Kartu: \"Biaya dokter. Bayar M700.\"\n";
            user -= (700);
            std::cout << "Sisa uang Anda: M" << user.getUang() << "\n";
        } 
        else {
            std::cout << "Kartu: \"Anda mau nyaleg. Bayar M200 kepada setiap pemain.\"\n";
            int biayaPerPemain = 200;
            for(auto& p : game->getPemain()) {
                if(p.getUsername() != user.getUsername() && p.getStatus() != "BANKRUPT") {
                    user-=(biayaPerPemain);
                    p+=(biayaPerPemain);
                    std::cout << "Membayar M200 kepada " << p.getUsername() << ".\n";
                }
            }
            std::cout << "Sisa uang Anda: M" << user.getUang() << "\n";
        }
    }

    // Case Lain
    else {return;}
}

// Kartu Kesempatan
void KartuKesempatan::apply(Game* game, User& user) {
    
}

// Kartu Dana Umum
void KartuDanaUmum::apply(Game* game, User& user) {
    
}

// KartuSpesial
KartuSpesial::KartuSpesial() {
    this->jenisKartu = 0;
}

KartuSpesial::~KartuSpesial() {}

void KartuSpesial::apply(Game* g, User& user) {}

// Move Card
MoveCard::MoveCard() {
    this->langkah = (rand() % 12) + 1; 
}

void MoveCard::apply(Game* game, User& user) {
    std::cout << "[KARTU SPESIAL] Menggunakan Move Card!\n";
    std::cout << "Pemain maju sebanyak " << langkah << " petak.\n";
    user.move(langkah);
}

// Discount Card
void DiscountCard::apply(Game* game, User& user) {
    srand(time(NULL));
    this->persentasediskon = (rand() % 5 + 1) * 10; 
    
    std::cout << "[KARTU SPESIAL] Menggunakan Discount Card!\n";
    std::cout << "Kamu mendapatkan diskon sewa sebesar " << persentasediskon << "% untuk 1 giliran ke depan.\n";
    
    user.setActiveDiscount(persentasediskon); 
}

// Shield Card
void ShieldCard::apply(Game* game, User& user) {
    std::cout << "[KARTU SPESIAL] Menggunakan Shield Card!\n";
    std::cout << "Kamu kebal dari tagihan sewa maupun sanksi apapun selama 1 giliran.\n";
    
    user.setShieldActive(true);
}

// Teleport Card
void TeleportCard::apply(Game* game, User& user) {
    int targetPetak;
    std::cout << "[KARTU SPESIAL] Menggunakan Teleport Card!\n";
    std::cout << "Masukkan indeks petak tujuan (0 - " << game->getBoard()->getSize() - 1 << "): ";
    std::cin >> targetPetak;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if(targetPetak >= 0 && targetPetak < game->getBoard()->getSize()) {
        std::cout << "Bidak dipindahkan ke petak indeks " << targetPetak << ".\n";
        user.setKoordinat(targetPetak);
    } else {
        std::cout << "Pilihan tidak valid! Efek kartu gagal atau terbuang.\n";
    }
}

// Lasso Card
void LassoCard::apply(Game* game, User& user) {
    std::string targetUsername;
    std::cout << "[KARTU SPESIAL] Menggunakan Lasso Card!\n";
    std::cout << "Masukkan username pemain lawan yang ingin ditarik: ";
    std::cin >> targetUsername;
    
    std::cout << "Mencoba menarik " << targetUsername << " ke lokasimu...\n";
    
    User* target = nullptr;
    for(auto& p : game->getPemain()) {
        if(p.getUsername() == targetUsername) {
            target = &p;
            break;
        }
    }

    if(target != nullptr) {
        if(target->getKoordinat() > user.getKoordinat()) {
             target->setKoordinat(user.getKoordinat());
             std::cout << targetUsername << " berhasil ditarik ke lokasimu (indeks " << user.getKoordinat() << ")!\n";
        } else {
             std::cout << targetUsername << " tidak berada di depanmu! Efek gagal.\n";
        }
    } else {
        std::cout << "Pemain dengan username tersebut tidak ditemukan.\n";
    }
}

// Demolition Card
void DemolitionCard::apply(Game* game, User& user) {
    std::string targetKode;
    std::cout << "[KARTU SPESIAL] Menggunakan Demolition Card!\n";
    std::cout << "Masukkan kode properti (misal JKT) milik lawan yang bangunannya ingin dihancurkan: ";
    std::cin >> targetKode;

    std::cout << "Mencoba menghancurkan bangunan di " << targetKode << "...\n";
    
    Petak* targetPetak = nullptr;
    for(int i = 0; i < game->getBoard()->getSize(); i++) {
        Petak* p = game->getBoard()->getPetakAt(i);
        if(p != nullptr && p->getKodePetak() == targetKode) {
            targetPetak = p;
            break;
        }
    }

    if (targetPetak != nullptr && targetPetak->getKategori() == "LAHAN") {
        PetakLahan* lahan = dynamic_cast<PetakLahan*>(targetPetak);
        if (lahan != nullptr) {
            if(lahan->getOwnerName() != "" && lahan->getOwnerName() != user.getUsername()) {
                 lahan->hancurkanBangunan();
                 std::cout << "Berhasil menghancurkan 1 tingkat bangunan di " << targetKode << "!\n";
            } else {
                 std::cout << "Properti ini milikmu sendiri atau belum dimiliki siapa-siapa!\n";
            }
        }
    } else {
        std::cout << "Properti dengan kode " << targetKode << " tidak ditemukan atau bukan petak lahan!\n";
    }
}

// Explicit Instantiation untuk Template CardDeck
template <class T>
CardDeck<T>::CardDeck() {
    this->JenisDeck = "Deck Default";
}

template <class T>
CardDeck<T>::~CardDeck() {
    this->Kartu.clear();
}
 /*
template class CardDeck<KartuAksi>;
template class CardDeck<KartuSpesial>;
template class CardDeck<KartuKesempatan>;
template class CardDeck<KartuDanaUmum>;
*/

template<>
CardDeck<KartuAksi>::CardDeck(){

}

template<>
CardDeck<KartuSpesial>::CardDeck(){

}
