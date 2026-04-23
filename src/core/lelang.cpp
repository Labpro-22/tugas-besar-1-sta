#include <algorithm>

#include "core/lelang.hpp"
#include "utils/gameException.hpp"

Lelang::Lelang(Properti* target, std::vector<User*> semuaPemain, User* pemicu) {
  this->targetProperti = target;
  this->highestBidder = nullptr;
  this->currentHighestBid = 0;
  this->consecutivePasses = 0;
  this->isFirstBid = true;
  this->currentPlayerIndex = 0;

  if (pemicu != nullptr) {
    auto it = std:find(semuaPemain.begin(), semuaPemain.end(), pemicu);
    if (it != semuaPemain.end()) {
      std::rotate(semuaPemain.begin(), it + 1, semuaPemain.end());
    }
  }
  this->peserta = semuaPemain;
}

void Lelang::bid(User* user, int nominal) {
    if (nominal < 0) {
        throw BidTidakValidException();
    }
    
    // Jika bukan bid pertama, nominal harus lebih besar dari bid tertinggi sebelumnya
    if (!isFirstBid && nominal <= currentHighestBid) {
        throw BidTidakValidException();
    }

    if (user->getUang() < nominal) {
        throw UangTidakCukupException();
    }

    currentHighestBid = nominal;
    highestBidder = user;
    isFirstBid = false;
    consecutivePasses = 0; 
}

void Lelang::pass(User* user) {
    (void)user;
    consecutivePasses++;
}

bool Lelang::isEnd() {
    if (peserta.size() <= 1) return true; // Cuma 1/0 pemain sejak awal
    
    // Jika belum ada yang bid (masih M0 awal), lelang batal jika SEMUA orang pass
    if (isFirstBid && consecutivePasses >= (int)peserta.size()) {
        return true;
    }
    // Jika sudah ada yang bid, lelang dimenangkan jika SEMUA PEMAIN LAIN pass beruntun (N - 1)
    if (!isFirstBid && consecutivePasses >= (int)peserta.size() - 1) {
        return true;
    }
    
    return false;
}

User* Lelang::getWinner() {
  if (!this->isEnd()) {
    throw LelangBelumSelesaiException();
  }

  return this->highestBidder;
}

int Lelang::getFinalPrice() {
   if (!this->isEnd()) {
    throw LelangBelumSelesaiException();
  }

  return this->currentHighestBid;
}

std::vector<User*> Lelang::getPesertaAktif() {
  return this->peserta;
}

Properti* Lelang::getTargetProperti() const {
  return this->targetProperti;
}

User* Lelang::getCurrentPlayer() {
    if (peserta.empty()) return nullptr;
    return peserta[currentPlayerIndex];
}

void Lelang::nextTurn() {
    if (!peserta.empty()) {
        currentPlayerIndex = (currentPlayerIndex + 1) % peserta.size();
    }
}
