#include "../../include/core/kartu.hpp"
#include "../../include/core/game.hpp"
#include "../../include/core/user.hpp"
#include "../../include/core/board.hpp"
#include "../../include/core/petak.hpp"

namespace {
    std::mt19937& cardRandomGenerator() {
        static std::mt19937 generator(std::random_device{}());
        return generator;
    }

    int randomInt(int min, int max) {
        std::uniform_int_distribution<int> distribution(min, max);
        return distribution(cardRandomGenerator());
    }
}

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

KartuKesempatan::KartuKesempatan(std::string nama, std::string deskripsi) 
: KartuAksi(nama, deskripsi) {}

KartuDanaUmum::KartuDanaUmum(std::string nama, std::string deskripsi) 
: KartuAksi(nama, deskripsi) {}

// Kartu Kesempatan
// 1. "Pergi ke stasiun terdekat."
KartuStasiunTerdekat::KartuStasiunTerdekat() 
    : KartuKesempatan("Kartu Kesempatan", "Pergi ke stasiun terdekat.") {}

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
            game->getBoard()->getPetakAt(checkPos)->onLanded(&user, game);
            return;
        }
    }
}

// 2. "Mundur 3 petak."
KartuMundurTigaPetak::KartuMundurTigaPetak() 
    : KartuKesempatan("Kartu Kesempatan", "Mundur 3 petak.") {}

void KartuMundurTigaPetak::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] \"" << deskripsi << "\"\n";
    user.move(-3, game->getBoard());
    std::cout << "> " << user.getUsername() << " mundur ke petak indeks " << user.getKoordinat() << ".\n";
}

// 3. "Masuk Penjara."
KartuMasukPenjara::KartuMasukPenjara() 
    : KartuKesempatan("Kartu Kesempatan", "Masuk Penjara.") {}

void KartuMasukPenjara::apply(Game* game, User& user) {
    std::cout << "\n[KESEMPATAN] \"" << deskripsi << "\"\n";
    const bool shieldAktif = user.isShieldActive();
    game->sendPlayerToJail(user);
    if (!shieldAktif) {
        std::cout << "> " << user.getUsername() << " langsung dijebloskan ke Penjara!\n";
    }
}

// Kartu Dana Umum
// 1. "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain."
KartuHadiahUlangTahun::KartuHadiahUlangTahun() 
    : KartuDanaUmum("Kartu Dana Umum", "Ini adalah hari ulang tahun Anda. Dapatkan M100 dari setiap pemain.") {}

void KartuHadiahUlangTahun::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    int totalHadiah = 0;
    
    for (auto& p : game->getPemain()) {
        if (p.getUsername() != user.getUsername() && p.getStatus() != 2) {
            if (p.isShieldActive()) {
                std::cout << "> " << p.getUsername()
                          << " dilindungi ShieldCard dan tidak membayar kado.\n";
                continue;
            }
            // BANGKRUT KALAU GABISA
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
    : KartuDanaUmum("Kartu Dana Umum", "Biaya dokter. Bayar M700.") {}

void KartuBiayaDokter::apply(Game* game, User& user) {
    (void) game;
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    if (user.isShieldActive()) {
        std::cout << "> ShieldCard aktif. Biaya dokter M700 dibatalkan.\n";
        return;
    }

    if (user.getUang() >= 700) {
        user -= 700;
        std::cout << "> Sisa uang Anda: M" << user.getUang() << "\n";
    } else {
        // BANGKRUT
        std::cout << "> Anda tidak memiliki cukup uang untuk membayar biaya dokter.\n";
        
    }
}

// 3. "Anda mau nyaleg. Bayar M200 kepada setiap pemain."
KartuNyaleg::KartuNyaleg() 
    : KartuDanaUmum("Kartu Dana Umum", "Anda mau nyaleg. Bayar M200 kepada setiap pemain.") {}

void KartuNyaleg::apply(Game* game, User& user) {
    std::cout << "\n[DANA UMUM] \"" << deskripsi << "\"\n";
    int biayaPerPemain = 200;
    if (user.isShieldActive()) {
        std::cout << "> ShieldCard aktif. Biaya kampanye dibatalkan.\n";
        return;
    }
    
    for (auto& p : game->getPemain()) {
        if (user.getUang() < biayaPerPemain) {
            std::cout << "> Anda tidak memiliki cukup uang untuk membayar biaya kampanye kepada " << p.getUsername() << ".\n";
            //BANGKRUT
        }
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

// 1. MoveCard
MoveCard::MoveCard() : KartuSpesial("Move Card", "Maju sekian petak secara instan") {
    // Nominal acak saat kartu didapatkan
    this->langkah = 0;
}

void MoveCard::randomize() {
    this->langkah = randomInt(1, 12);
}

int MoveCard::getLangkah() const {
    return langkah;
}

void MoveCard::setLangkah(int value) {
    langkah = value;
}

void MoveCard::apply(Game* game, User& user) {
    std::cout << "\n[SPESIAL] Menggunakan Move Card! Bidak melesat " << langkah << " petak.\n";
    game->move(langkah, user);
}

// 2. DiscountCard
DiscountCard::DiscountCard() : KartuSpesial("Discount Card", "Diskon sewa untuk 1 giliran") {
    this->persentasediskon = 0;
}

void DiscountCard::apply(Game* game, User& user) {
    (void) game;
    std::cout << "\n[SPESIAL] Menggunakan Discount Card! Anda mendapat diskon " 
              << persentasediskon << "% sewa selama 1 giliran.\n";
    user.setActiveDiscount(persentasediskon);
}

void DiscountCard::randomize() {
    this->persentasediskon = randomInt(1, 5) * 10;
}

int DiscountCard::getPersentaseDiskon() const {
    return persentasediskon;
}

void DiscountCard::setPersentaseDiskon(int value) {
    persentasediskon = value;
}

// 3. ShieldCard
ShieldCard::ShieldCard() : KartuSpesial("Shield Card", "Kebal dari sewa/sanksi untuk 1 giliran") {}

void ShieldCard::apply(Game* game, User& user) {
    (void) game;
    std::cout << "\n[SPESIAL] Menggunakan Shield Card! Anda dilindungi penuh untuk giliran ini.\n";
    user.setShieldActive(true);
}

// 4. TeleportCard
TeleportCard::TeleportCard() : KartuSpesial("Teleport Card", "Berpindah ke petak pilihan bebas") {}

void TeleportCard::apply(Game* game, User& user) {
    int targetPetak;
    std::cout << "\n[SPESIAL] Teleport Card Aktif!\n";
    std::cout << "> Masukkan indeks petak tujuan (0 - " << game->getBoard()->getSize() - 1 << "): ";
    std::cin >> targetPetak;
    
    if (targetPetak >= 0 && targetPetak < game->getBoard()->getSize()) {
        user.setKoordinat(targetPetak);
        std::cout << "Berhasil teleport ke indeks " << targetPetak << ".\n";
        game->getBoard()->getPetakAt(user.getKoordinat())->onLanded(&user, game);
    } else {
        std::cout << "Pilihan tidak valid! Kartu terbuang sia-sia :))))))))\n";
    }
}

// 5. LassoCard
LassoCard::LassoCard() : KartuSpesial("Lasso Card", "Menarik lawan yang ada di depan") {}

void LassoCard::apply(Game* game, User& user) {
    std::cout << "\n[SPESIAL] Lasso Card Aktif!\n";

    int userPosition = user.getKoordinat();
    int boardSize = game->getBoard()->getSize();
    User* target = nullptr;
    int nearestDistance = boardSize;

    for (auto& p : game->getPemain()) {
        if (&p == &user) {
            continue;
        }

        int distance = (p.getKoordinat() - userPosition + boardSize) % boardSize;
        if (distance > 0 && distance < nearestDistance) {
            target = &p;
            nearestDistance = distance;
        }
    }

    if (!target) {
        std::cout << "Tidak ada pemain di depan! Efek gagal.\n";
        return;
    }

    int targetPosition = target->getKoordinat();
    target->setKoordinat(userPosition);
    std::cout << "Berhasil menarik " << target->getUsername() << " dari petak " << targetPosition << " ke petak " << userPosition << ".\n";
}

// 6. DemolitionCard
DemolitionCard::DemolitionCard() : KartuSpesial("Demolition Card", "Menghancurkan 1 tingkat bangunan lawan") {}

void DemolitionCard::apply(Game* game, User& user) {
    std::string targetKode;
    std::cout << "\n[SPESIAL] Demolition Card Aktif!\n";
    std::cout << "> Masukkan kode properti target (misal JKT): ";
    std::cin >> targetKode;

    auto& lokasiKode = game->getLokasiKode();
    auto it = lokasiKode.find(targetKode);
    PetakProperti* targetPetak = (it != lokasiKode.end()) ? it->second : nullptr;

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
         std::cout << "KABOOM! Berhasil menghancurkan 1 tingkat bangunan di " << targetKode << "!\n";
    } else {
         std::cout << "Target tidak valid (belum dimiliki atau milik sendiri).\n";
    }
}

template <class T>
CardDeck<T>::CardDeck() {}

template <>
CardDeck<KartuKesempatan>::CardDeck() {
    addCard(new KartuStasiunTerdekat());
    addCard(new KartuMundurTigaPetak());
    addCard(new KartuMasukPenjara());
    shuffle();
}

template <>
CardDeck<KartuDanaUmum>::CardDeck() {
    addCard(new KartuHadiahUlangTahun());
    addCard(new KartuBiayaDokter());
    addCard(new KartuNyaleg());
    shuffle();
}

template <>
CardDeck<KartuSpesial>::CardDeck() {
    addCard(new MoveCard());
    addCard(new MoveCard());
    addCard(new MoveCard());
    addCard(new MoveCard());
    addCard(new DiscountCard());
    addCard(new DiscountCard());
    addCard(new DiscountCard());
    addCard(new ShieldCard());
    addCard(new ShieldCard());
    addCard(new TeleportCard());
    addCard(new TeleportCard());
    addCard(new LassoCard());
    addCard(new LassoCard());
    addCard(new DemolitionCard());
    addCard(new DemolitionCard());
    shuffle();
}

template class CardDeck<KartuKesempatan>;
template class CardDeck<KartuDanaUmum>;
template class CardDeck<KartuSpesial>;
