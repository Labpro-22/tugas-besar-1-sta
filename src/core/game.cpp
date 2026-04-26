#include "core/game.hpp"
#include "core/bangun.hpp"
#include <utility>

Game::Game() : MAX_TURN(100), turn(1), end(false), currentPemain(0), sudahPakaiKartuKemampuan(false) {} // Asumsi default batas giliran
Game::Game(int Maxturn) : MAX_TURN(Maxturn), turn(1), end(false), currentPemain(0), sudahPakaiKartuKemampuan(false) {}
Game::Game(int maxTurn,int turn,bool end,std::vector<User> pemain,std::vector<std::unique_ptr<Properti>>&& daftarProperti,
Board board,Dadu dadu,std::map<std::string, PetakProperti*> lokasiKode,std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup) 
: MAX_TURN(maxTurn),turn(turn),end(end),pemain(std::move(pemain)),
daftarProperti(std::move(daftarProperti)),currentPemain(0),board(std::move(board)),
dadu(std::move(dadu)),deckKartuSpesial(),sudahPakaiKartuKemampuan(false),
lokasiKode(std::move(lokasiKode)),lokasiColorGroup(std::move(lokasiColorGroup)) {}

bool Game::isEnd() {
    // Jika Pemain tinggal satu atau end true
    return (this->end || getActivePlayerCount() <= 1 || turn > MAX_TURN);
}

void Game::setMAXTURN(int max) {
    this->MAX_TURN = max;
}

void Game::nextturn() {
    for (const auto& properti : daftarProperti) {
        if (properti != nullptr) {
            properti->tickFestival();
        }
    }

    turn++;

    if (turn > MAX_TURN) {
        end = true;
    }
}

void Game::nextPlayer() {
    if (pemain.empty()) {
        return;
    }

    const int totalPemain = static_cast<int>(pemain.size());
    for (int langkah = 1; langkah <= totalPemain; ++langkah) {
        int kandidat = (currentPemain + langkah) % totalPemain;
        if (!pemain[kandidat].isBankrupt()) {
            currentPemain = kandidat;
            sudahPakaiKartuKemampuan = false;
            return;
        }
    }
}

void Game::bagikanKartuSpesial(User& user) {
    if (user.isBankrupt()) {
        return;
    }

    KartuSpesial* kartu = deckKartuSpesial.draw();
    if (kartu == nullptr) {
        std::cout << "[KARTU SPESIAL] Deck kosong, tidak ada kartu yang dibagikan.\n";
        return;
    }

    if (MoveCard* moveCard = dynamic_cast<MoveCard*>(kartu)) {
        moveCard->randomize();
    } else if (DiscountCard* discountCard = dynamic_cast<DiscountCard*>(kartu)) {
        discountCard->randomize();
    }

    std::cout << "[KARTU SPESIAL] " << user.getUsername() << " mendapat "
              << kartu->getNama() << ".\n";
    user.addKartuSpesial(kartu, &deckKartuSpesial);
}

void Game::mulaiLelang(Properti* targetProperti, User* pemicu) {
    if (targetProperti == nullptr) {
        return;
    }

    Lelang lelang(targetProperti, this, pemicu);
    lelang.mulaiLelang();
}

int Game::getTurn() {
    return turn;
}

int Game::getTurn() const {
    return turn;
}

int Game::getMaxTurn() const {
    return MAX_TURN;
}

int Game::getCurrentPemainIndex() const {
    return currentPemain;
}

int Game::getActivePlayerCount() const {
    int activeCount = 0;
    for (const auto& user : pemain) {
        if (!user.isBankrupt()) {
            ++activeCount;
        }
    }
    return activeCount;
}

std::vector<std::unique_ptr<Properti>>& Game::getDaftarProperti(){
    return daftarProperti;
}

int Game::getJailFine() const {
    Petak* jailPetak = board.getPetakAt(board.getPenjaraIndex());
    auto* penjara = dynamic_cast<PetakPenjara*>(jailPetak);
    if (penjara == nullptr) {
        return 0;
    }
    return penjara->getDenda();
}

bool Game::handleJailTurn(User& user) {
    if (!user.isJailed()) {
        return true;
    }

    const int jailFine = getJailFine();
    if (jailFine <= 0) {
        user.releaseFromJail();
        return true;
    }

    if (user.mustPayJailFine()) {
        if (user.getUang() < jailFine) {
            throw UangTidakCukupException("Uang pemain tidak cukup untuk membayar denda penjara!");
        }

        user -= jailFine;
        user.releaseFromJail();
        std::cout << user.getUsername() << " membayar denda penjara M" << jailFine << " dan keluar dari penjara.\n";
        return true;
    }

    std::cout << user.getUsername() << " sedang di penjara.\n";
    std::cout << "Pilih aksi: [1] Bayar denda M" << jailFine << " [2] Coba lempar double\n";
    std::cout << "> ";

    int pilihan = 2;
    std::cin >> pilihan;

    if (pilihan == 1) {
        if (user.getUang() < jailFine) {
            throw UangTidakCukupException("Uang pemain tidak cukup untuk membayar denda penjara!");
        }

        user -= jailFine;
        user.releaseFromJail();
        std::cout << user.getUsername() << " membayar denda penjara dan keluar.\n";
        return true;
    }

    dadu.shuffle();
    std::cout << "Hasil percobaan keluar penjara: " << dadu.getAngka1() << " + " << dadu.getAngka2() << "\n";
    if (dadu.isDouble()) {
        user.releaseFromJail();
        std::cout << user.getUsername() << " mendapatkan double dan keluar dari penjara.\n";
        return true;
    }

    user.incrementJailTurns();
    std::cout << user.getUsername() << " gagal keluar dari penjara.\n";
    return false;
}

void Game::sendPlayerToJail(User& user) {
    if (user.isShieldActive()) {
        std::cout << "[KARTU] ShieldCard aktif. Efek masuk penjara dibatalkan.\n";
        return;
    }

    const int jailIndex = board.getPenjaraIndex();
    if (jailIndex >= 0) {
        user.sendToJail(jailIndex);
    }
}

void Game::leave(User& user) {
    user.setStatus(2);

    if (getActivePlayerCount() <= 1) {
        end = true;
    }
}

Board* Game::getBoard() {return &board;}
std::vector<User>& Game::getPemain() {return pemain;}
const std::vector<User>& Game::getPemain() const {return pemain;}
Logger& Game::getLogger() {
    return this->gameLogger;
}
const Logger& Game::getLogger() const {
    return this->gameLogger;
}
Dadu* Game::getDadu() {return &dadu;}
std::map<std::string, PetakProperti*>& Game::getLokasiKode() {return lokasiKode;}
std::map<std::string, std::vector<PetakProperti*>>& Game::getLokasiColorGroup() {return lokasiColorGroup;}
CardDeck<KartuSpesial>& Game::getDeckKartuSpesial() {return deckKartuSpesial;}
const CardDeck<KartuSpesial>& Game::getDeckKartuSpesial() const {return deckKartuSpesial;}

void Game::move(int langkah, User& user) {
    user.move(langkah, &board);
    board.getPetakAt(user.getKoordinat())->onLanded(&user, this);
}

void Game::prosesGadai(User& user, Properti* properti) {
    // Implementasi proses gadai
    if (properti == nullptr) {
        throw SyaratPropertiInvalidException("Gagal Gadai: Properti tidak valid!");
    }
    if (properti->getOwner() != &user) {
        throw BukanPemilikException("Gagal Gadai: Properti ini bukan milik pemain aktif!");
    }
    properti->gadaikan();

    gameLogger.addLog(turn, user.getUsername(), "Gadai Properti", "Menggadaikan " + properti->getNama() + " dan mendapat M" + std::to_string(properti->getNilaiGadai()));
}

void Game::prosesTebus(User& user, Properti* properti) {
    // Implementasi proses tebus
    if (properti == nullptr) {
        throw SyaratPropertiInvalidException("Gagal Tebus: Properti tidak valid!");
    }
    if (properti->getOwner() != &user) {
        throw BukanPemilikException("Gagal Tebus: Properti ini bukan milik pemain aktif!");
    }
    properti->tebus();

    gameLogger.addLog(turn, user.getUsername(), "Tebus Properti", "Menebus " + properti->getNama() + " dengan biaya M" + std::to_string(properti->getHargaBeli()));
}

void Game::prosesBangun(Properti* properti) {
    // Implementasi proses bangun
    if (properti == nullptr) {
        throw SyaratPropertiInvalidException("Gagal Bangun: Properti tidak valid!");
    }

    Street* street = dynamic_cast<Street*>(properti);
    if (street == nullptr) {
        throw SyaratPembangunanException("Gagal Bangun: Hanya properti Street yang dapat dibangun!");
    }

    if (street->getOwner() == nullptr) {
        throw BukanPemilikException("Gagal Bangun: Properti belum dimiliki pemain!");
    }

    if (street->getStatus() != PropStatus::OWNED) {
        throw SyaratPropertiInvalidException("Gagal Bangun: Properti harus berstatus OWNED!");
    }

    auto groupIt = lokasiColorGroup.find(street->getWarna());
    if (groupIt == lokasiColorGroup.end()) {
        throw SyaratPembangunanException("Gagal Bangun: Color group tidak ditemukan!");
    }

    std::vector<Street*> grupWarna;
    for (PetakProperti* petak : groupIt->second) {
        if (petak == nullptr) {
            continue;
        }
        Street* anggota = dynamic_cast<Street*>(petak->getSertifikat());
        if (anggota != nullptr) {
            grupWarna.push_back(anggota);
        }
    }

    if (grupWarna.empty()) {
        throw SyaratPembangunanException("Gagal Bangun: Color group tidak valid!");
    }

    for (Street* anggota : grupWarna) {
        if (anggota == nullptr || anggota->getOwner() != street->getOwner() || anggota->getStatus() != PropStatus::OWNED) {
            throw SyaratPembangunanException("Gagal Bangun: Pemain harus memiliki seluruh Street dalam color group dan semuanya berstatus OWNED!");
        }
    }

    Bangun::eksekusiBangun(street->getOwner(), street, grupWarna);

    gameLogger.addLog(turn, street->getOwner()->getUsername(), "Bangun Properti", "Membangun/Upgrade di " + street->getNama());
}

void Game::prosesPakaiKartu(User& user, KartuSpesial* kartu) {
    if (kartu == nullptr) {
        throw SyaratPropertiInvalidException("Gagal Pakai Kartu: Kartu tidak valid!");
    }
    if (sudahPakaiKartuKemampuan) {
        throw SyaratPropertiInvalidException("Gagal Pakai Kartu: Kartu kemampuan hanya boleh digunakan 1 kali dalam 1 giliran!");
    }

    KartuSpesial* kartuDipakai = user.removeKartuSpesial(kartu);
    if (kartuDipakai == nullptr) {
        throw SyaratPropertiInvalidException("Gagal Pakai Kartu: Kartu tidak ada di tangan pemain!");
    }

    try {
        kartuDipakai->apply(this, user);
    } catch (...) {
        deckKartuSpesial.discard(kartuDipakai);
        sudahPakaiKartuKemampuan = true;
        throw;
    }

    deckKartuSpesial.discard(kartuDipakai);
    sudahPakaiKartuKemampuan = true;
}

void Game::prosesLoad() {
    // Implementasi proses load
}

void Game::prosesSave() {
    // Implementasi proses save
}
