#include <algorithm>

#include "core/lelang.hpp"
#include "utils/gameException.hpp"

Lelang::Lelang(Properti* target, std::vector<User*> semuaPemain) {
  this->targetProperti = target;
  this->peserta = semuaPemain;
  this->highestBidder = nullptr;
  this->currentHighestBid = 0;
}

void Lelang::bid(User* user, int nominal) {
  if (nominal <= currentHighestBid) {
    throw BidTidakValidException();
  }

  if (user->getUang() < nominal ) {
    throw UangTidakCukupException();
  }

  currentHighestBid = nominal;
  highestBidder = user;
}

void Lelang::pass(User* user) {
  this->peserta.erase(
    std::remove(this->peserta.begin(), this->peserta.end(), user), peserta.end());
}

bool Lelang::isEnd() {
  return this->peserta.size() <= 1;
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
