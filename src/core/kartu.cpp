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

Kartu::Kartu(std::string nama, std::string deskripsi) 
    : nama(nama), deskripsi(deskripsi) {}

Kartu::~Kartu() {}

std::string Kartu::getNama() const { 
    return nama;
}

std::string Kartu::getDeskripsi() const { 
    return deskripsi;
}

KartuAksi::KartuAksi(std::string nama, std::string deskripsi) 
: Kartu(nama, deskripsi) {}


// Kartu Kesempatan
// 1. "Pergi ke stasiun terdekat."
KartuStasiunTerdekat::KartuStasiunTerdekat() 
    : KartuAksi("Kartu Kesempatan", "Pergi ke stasiun terdekat.") {}

void KartuStasiunTerdekat::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] \"" << deskripsi << "\"\n";
    
    int currentPos = user.getKoordinat();
    int boardSize = game->getBoard()->getSize();
    
    for (int i = 1; i <= boardSize; ++i) {
        int checkPos = (currentPos + i) % boardSize;
        Petak* p = game->getBoard()->getPetakAt(checkPos);
        
        if (p != nullptr && (p->getKategori() == "Stasiun" || p->getKategori() == "STASIUN")) {
            std::cout << "> Memindahkan " << user.getUsername() << " ke stasiun terdekat di indeks " << checkPos << ".\n";
            user.setKoordinat(checkPos);
            return;
        }
    }
}

// 2. "Mundur 3 petak."
KartuMundurTigaPetak::KartuMundurTigaPetak() 
    : KartuAksi("Kartu Kesempatan", "Mundur 3 petak.") {}

void KartuMundurTigaPetak::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] \"" << deskripsi << "\"\n";
    user.move(-3, game->getBoard()->getSize());
    std::cout << "> " << user.getUsername() << " mundur ke petak indeks " << user.getKoordinat() << ".\n";
}

// 3. "Masuk Penjara."
KartuMasukPenjara::KartuMasukPenjara() 
    : KartuAksi("Kartu Kesempatan", "Masuk Penjara.") {}

void KartuMasukPenjara::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] \"" << deskripsi << "\"\n";
    user.setKoordinat(game->getBoard()->getPenjaraIndex());
    user.setStatus(1); 
    std::cout << "> " << user.getUsername() << " langsung dijebloskan ke Penjara!\n";
}

// Kartu Dana Umum
// 1. "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."
KartuHadiahUlangTahun::KartuHadiahUlangTahun() 
    : KartuAksi("Kartu Dana Umum", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.") {}

void KartuHadiahUlangTahun::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    int totalHadiah = 0;
    
    for (auto& p : game->getPemain()) {
        if (p.getUsername() != user.getUsername() && p.getStatus() != 2) {
            int bayar = (p.getUang() >= 100) ? 100 : p.getUang();
            p -= bayar;
            totalHadiah += bayar;
            
            if (bayar < 100) {
                std::cout << "> " << p.getUsername() << " tidak memiliki cukup uang, hanya memberikan M" << bayar << ".\n";
            } else {
                std::cout << "> " << p.getUsername() << " memberikan kado M100.\n";
            }
        }
    }
    user += totalHadiah;
    std::cout << "> Anda mendapatkan total M" << totalHadiah << "!\n";
}

// 2. "Biaya dokter. Bayar M700."
KartuBiayaDokter::KartuBiayaDokter() 
    : KartuAksi("Kartu Dana Umum", "Biaya dokter. Bayar M700.") {}

void KartuBiayaDokter::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    user -= 700;
    std::cout << "> Sisa uang Anda: M" << user.getUang() << "\n";
}

// 3. "Anda mau nyaleg. Bayar M200 kepada setiap pemain."
KartuNyaleg::KartuNyaleg() 
    : KartuAksi("Kartu Dana Umum", "Anda mau nyaleg. Bayar M200 kepada setiap pemain.") {}

void KartuNyaleg::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    int biayaPerPemain = 200;
    
    for (auto& p : game->getPemain()) {
        if (p.getUsername() != user.getUsername() && p.getStatus() != 2) {
            user -= biayaPerPemain;
            p += biayaPerPemain;
            std::cout << "> Membayar dana kampanye M200 kepada " << p.getUsername() << ".\n";
        }
    }
    std::cout << "> Sisa uang Anda: M" << user.getUang() << "\n";
}

KartuSpesial::KartuSpesial(std::string nama, std::string deskripsi) 
: Kartu(nama, deskripsi) {}

MoveCard::MoveCard() : KartuSpesial("Move Card", "Maju sekian petak secara instan") {
    // Nominal acak saat kartu didapatkan
    this->langkah = (rand() % 12) + 1;
}

void MoveCard::apply(Game* game, User& user) {
    std::cout << "\n[SPESIAL] Menggunakan Move Card! Bidak melesat " << langkah << " petak.\n";
    user.move(langkah, game->getBoard()->getSize());
}

DiscountCard::DiscountCard() : KartuSpesial("Discount Card", "Diskon sewa untuk 1 giliran") {
    this->persentasediskon = (rand() % 5 + 1) * 10;
}

void DiscountCard::apply(Game* game, User& user) {
    std::cout << "\n[SPESIAL] Menggunakan Discount Card! Anda mendapat diskon " 
              << persentasediskon << "% sewa selama 1 giliran.\n";
    user.setActiveDiscount(persentasediskon);
}

ShieldCard::ShieldCard() : KartuSpesial("Shield Card", "Kebal dari sewa/sanksi untuk 1 giliran") {}

void ShieldCard::apply(Game* game, User& user) {
    std::cout << "\n[SPESIAL] Menggunakan Shield Card! Anda dilindungi penuh untuk giliran ini.\n";
    user.setShieldActive(true);
}

TeleportCard::TeleportCard() : KartuSpesial("Teleport Card", "Berpindah ke petak pilihan bebas") {}

void TeleportCard::apply(Game* game, User& user) {
    int targetPetak;
    std::cout << "\n[SPESIAL] Teleport Card Aktif!\n";
    std::cout << "> Masukkan indeks petak tujuan (0 - " << game->getBoard()->getSize() - 1 << "): ";
    std::cin >> targetPetak;
    
    if (targetPetak >= 0 && targetPetak < game->getBoard()->getSize()) {
        user.setKoordinat(targetPetak);
        std::cout << "Berhasil teleport ke indeks " << targetPetak << ".\n";
    } else {
        std::cout << "Pilihan tidak valid! Kartu terbuang sia-sia.\n";
    }
}

LassoCard::LassoCard() : KartuSpesial("Lasso Card", "Menarik lawan yang ada di depan") {}

void LassoCard::apply(Game* game, User& user) {
    std::string targetUsername;
    std::cout << "\n[SPESIAL] Lasso Card Aktif!\n";
    std::cout << "> Masukkan username target: ";
    std::cin >> targetUsername;
    
    User* target = nullptr;
    for (auto& p : game->getPemain()) {
        if (p.getUsername() == targetUsername) {
            target = &p;
            break;
        }
    }

    if (!target) {
        std::cout << "Pemain '" << targetUsername << "' tidak ditemukan. Efek gagal.\n";
        return;
    }
    
    if (target->getKoordinat() > user.getKoordinat()) {
        target->setKoordinat(user.getKoordinat());
        std::cout << "Berhasil menarik " << targetUsername << " ke lokasimu!\n";
    } else {
        std::cout << targetUsername << " tidak berada di depanmu! Efek gagal.\n";
    }
}

DemolitionCard::DemolitionCard() : KartuSpesial("Demolition Card", "Menghancurkan 1 tingkat bangunan lawan") {}

void DemolitionCard::apply(Game* game, User& user) {
    std::string targetKode;
    std::cout << "\n[SPESIAL] Demolition Card Aktif!\n";
    std::cout << "> Masukkan kode properti target (misal JKT): ";
    std::cin >> targetKode;

    Petak* targetPetak = nullptr;
    for (int i = 0; i < game->getBoard()->getSize(); i++) {
        Petak* p = game->getBoard()->getPetakAt(i);
        if (p != nullptr && p->getKodePetak() == targetKode) {
            targetPetak = p; 
            break;
        }
    }

    if (!targetPetak) {
        std::cout << "Properti dengan kode '" << targetKode << "' tidak ditemukan di papan!\n";
        return;
    }

    PetakLahan* lahan = dynamic_cast<PetakLahan*>(targetPetak);
    if (!lahan) {
        std::cout << "Properti " << targetKode << " bukan petak lahan (tidak memiliki bangunan)!\n";
        return;
    }

    if (lahan->getOwnerName() != "" && lahan->getOwnerName() != user.getUsername()) {
         lahan->hancurkanBangunan();
         std::cout << "BOOM! Berhasil menghancurkan 1 tingkat bangunan di " << targetKode << "!\n";
    } else {
         std::cout << "Target tidak valid (belum dimiliki atau milik sendiri).\n";
    }
}

KartuKesempatan::KartuKesempatan() : Kartu("Kesempatan", "Pengelola Deck Kesempatan") {
    deck.addCard(new KartuStasiunTerdekat());
    deck.addCard(new KartuMundurTigaPetak());
    deck.addCard(new KartuMasukPenjara());
    deck.shuffle();
}

void KartuKesempatan::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] Menarik kartu dari tumpukan...\n";
    KartuAksi* ditarik = deck.draw();
    if (ditarik != nullptr) {
        ditarik->apply(game, user);
        deck.discard(ditarik); // Kembalikan ke deck buangan
    }
}

KartuDanaUmum::KartuDanaUmum() : Kartu("Dana Umum", "Pengelola Deck Dana Umum") {
    deck.addCard(new KartuHadiahUlangTahun());
    deck.addCard(new KartuBiayaDokter());
    deck.addCard(new KartuNyaleg());
    deck.shuffle();
}

void KartuDanaUmum::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] Menarik kartu dari tumpukan...\n";
    KartuAksi* ditarik = deck.draw();
    if (ditarik != nullptr) {
        ditarik->apply(game, user);
        deck.discard(ditarik); // Kembalikan ke deck buangan
    }
}

KartuKesempatan* getKartuKesempatanInstance() {
    static KartuKesempatan instanceKesempatan; 
    return &instanceKesempatan;
}

KartuDanaUmum* getKartuDanaUmumInstance() {
    static KartuDanaUmum instanceDanaUmum;
    return &instanceDanaUmum;
}

CardDeck<KartuSpesial>& getDeckSpesialInstance() {
    static CardDeck<KartuSpesial> deckSpesial;
    static bool isInitialized = false;
    
    if (!isInitialized) {
        deckSpesial.addCard(new MoveCard());
        deckSpesial.addCard(new DiscountCard());
        deckSpesial.addCard(new ShieldCard());
        deckSpesial.addCard(new TeleportCard());
        deckSpesial.addCard(new LassoCard());
        deckSpesial.addCard(new DemolitionCard());
        
        deckSpesial.shuffle();
        isInitialized = true;
    }
    return deckSpesial;
}