#include <algorithm>
#include <iostream>

#include "core/game.hpp"
#include "core/lelang.hpp"
#include "utils/gameException.hpp"

Lelang::Lelang(Properti* target, Game* game, User* pemicu) {
  this->targetProperti = target;
  this->highestBidder = nullptr;
  this->currentHighestBid = 0;
  this->consecutivePasses = 0;
  this->isFirstBid = true;
  this->currentPlayerIndex = 0;

  // 1. Filter pemain yang belum bangkrut langsung di dalam konstruktor
  std::vector<User*> semuaPemain;
  if (game != nullptr) {
      for (auto& u : game->getPemain()) {
          if (!u.isBankrupt()) {
              semuaPemain.push_back(&u);
          }
      }
  }

  // 2. Putar urutan supaya index [0] adalah pemain SETELAH pemicu
  if (pemicu != nullptr && !semuaPemain.empty()) {
      auto it = std::find(semuaPemain.begin(), semuaPemain.end(), pemicu);
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

void Lelang::mulaiLelang() {
    std::cout << "\n=== LELANG DIBUKA UNTUK " << targetProperti->getNama() << " ===\n";

    while (!this->isEnd()) {
        User* currentUser = this->getCurrentPlayer();
        
        std::cout << "\nGiliran bid: " << currentUser->getUsername() << " (Uang: M" << currentUser->getUang() << ")\n";
        std::cout << "Ketik nominal untuk bid (atau ketik -1 untuk PASS): ";
        
        int nominal;
        std::cin >> nominal;

        if (nominal == -1) {
            this->pass(currentUser);
            std::cout << currentUser->getUsername() << " memilih PASS.\n";
            this->nextTurn(); // Lanjut ke pemain berikutnya
        } else {
            try {
                this->bid(currentUser, nominal);
                std::cout << "[SUCCESS] " << currentUser->getUsername() << " memimpin lelang dengan bid M" << nominal << "!\n";
                this->nextTurn(); // Lanjut ke pemain berikutnya jika sukses bid
            } catch (const std::exception& e) {
                std::cout << "[ERROR] " << e.what() << "\n";
                std::cout << "Silakan masukkan bid yang valid atau PASS.\n";
            }
        }
    }

    // Selesaikan lelang dan proses pemenang
    User* pemenang = this->getWinner();
    if (pemenang != nullptr) {
        int hargaFinal = this->getFinalPrice();
        std::cout << "\n=== LELANG SELESAI ===\n";
        std::cout << "Pemenang: " << pemenang->getUsername() << " dengan harga M" << hargaFinal << "!\n";
        
        // Potong uang pemenang dan berikan sertifikat
        *(pemenang) -= hargaFinal;
        targetProperti->setOwner(pemenang);
        targetProperti->setStatus(PropStatus::OWNED); 
    } else {
        std::cout << "\n=== LELANG BATAL ===\n";
        std::cout << "Semua pemain PASS. Properti kembali ke Bank.\n";
    }
}
