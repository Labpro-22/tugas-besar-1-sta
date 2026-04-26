#include "../../include/core/user.hpp"
#include "../../include/core/board.hpp"
#include "../../include/core/petak.hpp"
#include "../../include/core/kartu.hpp"
#include <limits>

User::User() : username(""), uang(1500), koordinat(0), status(3), jailTurns(0) {}
User::User(const std::string& username, int uangAwal)
    : username(username), uang(uangAwal), koordinat(0), status(3), jailTurns(0) {}
User::~User() {}

int User::getUang() const { return uang;}

std::string User::getUsername() const {return username;}

int User::getKoordinat() const { return koordinat;}

int User::getStatus() const {return status;}

int User::getJailTurns() const { return jailTurns; }

bool User::isJailed() const { return status == 1; }

bool User::isBankrupt() const { return status == 2; }

bool User::mustPayJailFine() const { return isJailed() && jailTurns >= 3; }

Properti* getPropertiByKode(const std::string& kode);

const std::vector<Properti*>& User::getListProperti() const { return listProperti;}

std::vector<Street*> User::getStreetByColor (const std::string& warna) const {
  std::vector<Street*> hasilPencarian;
  for (Properti* p : this->listProperti) {
    Street* jalan = dynamic_cast<Street*>(p);
    if (jalan != nullptr) {
      if (jalan->getWarna() == warna) {
        hasilPencarian.push_back(jalan);
      }
    }
  }
  return hasilPencarian;
}

int User::getRailroadCount() const {
  int jumlahStasiun = 0;
  for(Properti* p : this->listProperti) {
    RailRoad* stasiun = dynamic_cast<RailRoad*>(p);
    if(stasiun != nullptr) {
      jumlahStasiun++;
    }
  }

  if (jumlahStasiun > 4) {
    throw SyaratPropertiInvalidException("Jumlah stasiun yang dimiliki pemain melebihi batas maksimal dunia (4)!");
  }

  return jumlahStasiun;
}

int User::getUtilityCount() const {
  int jumlahUtility = 0;
  for(Properti* p : this->listProperti) {
    Utility* utility = dynamic_cast<Utility*>(p);
    if(utility != nullptr) {
      jumlahUtility++;
    }
  }

  if (jumlahUtility > 2) {
    throw SyaratPropertiInvalidException("Jumlah fasilitas umum (Utility) melebihi batas maksimal dunia (2)!");
  }

  return jumlahUtility;
}

int User::getTotalKekayaan() const{
  int total = this->uang;

  for (size_t i = 0; i < listProperti.size(); ++i) {
    Properti* properti = listProperti[i];
    if (properti == nullptr) {
      continue;
    }

    total += properti->getHargaBeli();

    Street* jalan = dynamic_cast<Street*>(properti);
    if (jalan == nullptr) {
      continue;
    }

    if (jalan->isHotel()) {
      total += jalan->getHargaHotel();
    } else {
      total += jalan->getJumlahBangunan() * jalan->getHargaBangunan();
    }
  }

  return total;
}

void User::move(int langkah, Board* board) {
  if (board == nullptr) {
      return;
  }

  const int boardSize = board->getSize();
  this->koordinat += langkah;

  if (this->koordinat < 0) {
      this->koordinat += boardSize; 
  }
  else if (this->koordinat >= boardSize) {
      this->koordinat %= boardSize;
      if (langkah > 0) {
          const int goIndex = board->getGoIndex();
          if (goIndex >= 0 && this->koordinat != goIndex) {
              Petak* goPetak = board->getPetakAt(goIndex);
              PetakGo* petakGo = dynamic_cast<PetakGo*>(goPetak);
              if (petakGo != nullptr) {
                  const int salary = petakGo->getEarnMoney();
                  std::cout << username << " melewati GO! Mendapatkan gaji M" << salary << ".\n";
                  *this += salary;
              }
          }
      }
  }
}

void User::addProperti(Properti* p) {
  if (p == nullptr) {
    return;
  }

  if (std::find(this->listProperti.begin(), this->listProperti.end(), p) == this->listProperti.end()) {
    this->listProperti.push_back(p);
  }
}

void User::removeProperti(Properti* p) {
  this->listProperti.erase(
    std::remove(this->listProperti.begin(), this->listProperti.end(), p),
    this->listProperti.end());
}

bool User::hasMonopoli(const std::string& warna, int totalDiPapan) const {
  int counter = 0;
  for (Properti* p : this->listProperti) {
    Street* jalan = dynamic_cast<Street*>(p);
    if (jalan != nullptr) {
      if (jalan->getWarna() == warna) {
        counter++;
      }
    }
  }
  return counter == totalDiPapan;
}

void User::addKartuSpesial(KartuSpesial* kartu, CardDeck<KartuSpesial>* deck) {
  if (kartu != nullptr) {
    this->kartuSpesial.push_back(kartu);
  }

  if (this->kartuSpesial.size() > 3) {
    KartuSpesial* kartuDibuang = dropKartuSpesial();
    if (deck != nullptr) {
      deck->discard(kartuDibuang);
    } else {
      delete kartuDibuang;
    }
  }
}

KartuSpesial* User::dropKartuSpesial() {
  if (this->kartuSpesial.empty()) {
    return nullptr;
  }

  std::cout << "\nPERINGATAN: Kamu sudah memiliki "
            << this->kartuSpesial.size()
            << " kartu kemampuan di tangan (maksimal 3).\n";
  std::cout << "Kamu diwajibkan membuang 1 kartu.\n";
  std::cout << "Daftar Kartu Kemampuan Anda:\n";
  for (size_t i = 0; i < this->kartuSpesial.size(); ++i) {
    std::cout << i + 1 << ". " << this->kartuSpesial[i]->getNama()
              << " - " << this->kartuSpesial[i]->getDeskripsi() << "\n";
  }

  size_t pilihan = 0;
  while (pilihan < 1 || pilihan > this->kartuSpesial.size()) {
    std::cout << "Pilih nomor kartu yang ingin dibuang (1-"
              << this->kartuSpesial.size() << "): ";
    if (!(std::cin >> pilihan)) {
      std::cin.clear();
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      pilihan = 0;
    }
  }

  KartuSpesial* kartuDibuang = this->kartuSpesial[pilihan - 1];
  this->kartuSpesial.erase(this->kartuSpesial.begin() + (pilihan - 1));
  std::cout << kartuDibuang->getNama()
            << " telah dibuang. Sekarang kamu memiliki "
            << this->kartuSpesial.size() << " kartu di tangan.\n";
  return kartuDibuang;
}

KartuSpesial* User::removeKartuSpesial(KartuSpesial* kartu) {
  auto it = std::find(this->kartuSpesial.begin(), this->kartuSpesial.end(), kartu);
  if (it == this->kartuSpesial.end()) {
    return nullptr;
  }

  KartuSpesial* kartuDihapus = *it;
  this->kartuSpesial.erase(it);
  return kartuDihapus;
}

void User::setKartuSpesial(const std::vector<KartuSpesial*>& kartu) {
  this->kartuSpesial = kartu;
}

const std::vector<KartuSpesial*>& User::getKartuSpesial() const {
  return kartuSpesial;
}

User& User::operator+=(int jumlahUang) {
  this->uang += jumlahUang;
  return *this;
}

User& User::operator-=(int jumlahUang) {
  this->uang -= jumlahUang;
  return *this;
}

void User::setUsername(const std::string& name) {
    this->username = name;
}

void User::setUang(int value) {
    this->uang = value;
}

void User::setStatus(const int newStatus) {
    this->status = newStatus;
    if (newStatus != 1) {
        this->jailTurns = 0;
    }
}

void User::setKoordinat(int index) {
    this->koordinat = index;
}

void User::setJailTurns(int value) {
    this->jailTurns = std::max(0, value);
}

void User::sendToJail(int jailIndex) {
    this->koordinat = jailIndex;
    this->status = 1;
    this->jailTurns = 0;
}

void User::releaseFromJail() {
    this->status = 3;
    this->jailTurns = 0;
}

void User::incrementJailTurns() {
    if (isJailed()) {
        ++jailTurns;
    }
}




Properti* User::getPropertiByKode(const std::string& kode) {
    for (size_t i = 0; i < listProperti.size(); ++i) {
        if (listProperti[i] != nullptr && listProperti[i]->getKode() == kode) {
            return listProperti[i];
        }
    }
    return nullptr;
}



void User::setActiveDiscount(int discountPercentage) {
    this->activeDiscount = discountPercentage;
}

int User::getActiveDiscount() const {
    return activeDiscount;
}

void User::setShieldActive(bool active) {
    this->shieldActive = active;
}

bool User::isShieldActive() const {
    return shieldActive;
}

void User::resetEfekKartuSpesial() {
    this->activeDiscount = 0;
    this->shieldActive = false;
}

LogEntry::LogEntry(int turn, std::string username, std::string jenisAksi, std::string detail)
    : turn(turn), username(username), jenisAksi(jenisAksi), detail(detail) {}

int LogEntry::getTurn() const { return turn; }
std::string LogEntry::getUsername() const { return username; }
std::string LogEntry::getJenisAksi() const { return jenisAksi; }
std::string LogEntry::getDetail() const { return detail; }

Logger::Logger() {}
Logger::~Logger() {}

void Logger::addLog(int turn, const std::string& username, const std::string& jenisAksi, const std::string& detail) {
    LogEntry entry(turn, username, jenisAksi, detail);
    logs.push_back(entry);
}

void Logger::cetakLogPenuh() const {
    std::cout << "Log Transaksi Penuh \n";
    for (size_t i = 0; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.getTurn() << "] " << log.getUsername() 
                  << " | " << log.getJenisAksi() << " | " << log.getDetail() << "\n";
    }
}

void Logger::cetakLogTerbaru(int jumlah) const {
    std::cout << "Log Transaksi (" << jumlah << " Terakhir)\n";
    
    size_t start = 0;
    if (logs.size() > static_cast<size_t>(jumlah)) {
        start = logs.size() - jumlah;
    }
    
    for (size_t i = start; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.getTurn() << "] " << log.getUsername() 
                  << " | " << log.getJenisAksi() << " | " << log.getDetail() << "\n";
    }
}

const std::vector<LogEntry>& Logger::getLogs() const {
    return logs;
}
