#include "core/game.hpp"
#include <utility>

Game::Game() : MAX_TURN(100), turn(1), end(false), currentPemain(0) {} // Asumsi default batas giliran
Game::Game(int Maxturn) : MAX_TURN(Maxturn), turn(1), end(false), currentPemain(0) {}
Game::Game(int maxTurn,int turn,bool end,std::vector<User> pemain,std::vector<std::unique_ptr<Properti>>&& daftarProperti,
Board board,Dadu dadu,std::map<std::string, PetakProperti*> lokasiKode,std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup) 
: MAX_TURN(maxTurn),turn(turn),end(end),pemain(std::move(pemain)),
daftarProperti(std::move(daftarProperti)),currentPemain(0),board(std::move(board)),
dadu(std::move(dadu)),lokasiKode(std::move(lokasiKode)),lokasiColorGroup(std::move(lokasiColorGroup)) {
    urutanPemain.reserve(this->pemain.size());
    for (size_t i = 0; i < this->pemain.size(); ++i) {
        urutanPemain.push_back(static_cast<int>(i));
    }
}

bool Game::isEnd() {
    // Jika Pemain tinggal satu atau end true
    return (this->end || getActivePlayerCount() <= 1);
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
            return;
        }
    }
}

int Game::getTurn() {
    return turn;
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
std::vector<Logger> Game::getLog(){return Log;}
Dadu* Game::getDadu() {return &dadu;}
std::map<std::string, PetakProperti*>& Game::getLokasiKode() {return lokasiKode;}
std::map<std::string, std::vector<PetakProperti*>>& Game::getLokasiColorGroup() {return lokasiColorGroup;}
