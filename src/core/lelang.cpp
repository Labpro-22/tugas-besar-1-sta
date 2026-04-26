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

        bool isMandatoryBid = (this->isFirstBid && this->consecutivePasses == (int)this->peserta.size() - 1);

        if (isMandatoryBid) {
            std::cout << "\n[WAJIB BID] Semua pemain lain telah PASS. Kamu WAJIB melakukan bid!\n";
        }
        
        std::cout << "\nGiliran bid: " << currentUser->getUsername() << " (Uang: M" << currentUser->getUang() << ")\n";

        if (!isFirstBid && highestBidder != nullptr) {
            std::cout << "Pemimpin lelang saat ini: " << highestBidder->getUsername() << " (M" << currentHighestBid << ")\n";
        }

        std::cout << "Ketik nominal untuk bid (atau ketik -1 untuk PASS): ";
        
        int nominal;
        std::cin >> nominal;

        try {
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                throw InputTidakValidException();
            }

            if (nominal == -1) {
                if (isMandatoryBid) {
                    std::cout << "Kamu pemain terakhir! Tidak boleh PASS, wajib melakukan bid!\n";
                    continue;
                }
                this->pass(currentUser);
                std::cout << currentUser->getUsername() << " memilih PASS.\n";
                this->nextTurn(); // Lanjut ke pemain berikutnya
            } else { 
                this->bid(currentUser, nominal);
                std::cout << "[SUCCESS] " << currentUser->getUsername() << " memimpin lelang dengan bid M" << nominal << "!\n";

                if (gameInstance) gameInstance->getLogger().addLog(gameInstance->getTurn(), currentUser->getUsername(), "Bid Lelang", "Bid M" + std::to_string(nominal) + " untuk " + targetProperti->getNama());

                this->nextTurn();
            } 
        } catch (const gameException& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
            std::cout << "Silahkan coba lagi.\n";
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
        
        if (gameInstance) gameInstance->getLogger().addLog(gameInstance->getTurn(), pemenang->getUsername(), "Menang Lelang", "Memenangkan lelang " + targetProperti->getNama() + " seharga M" + std::to_string(hargaFinal));
        
    } else {
        std::cout << "\n=== LELANG BATAL ===\n";
        std::cout << "Semua pemain PASS. Properti kembali ke Bank.\n";
    }
}
