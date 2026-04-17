#include "user.hpp"
#include "gameException.hpp"
#include <algorithm>

User::User() : username(""), uang(1500), koordinat(0), status(0) {}

User::User(const std::string& username, int uangAwal)
    : username(username), uang(uangAwal), koordinat(0), status(0) {}

User::~User() {}

void User::move(int dadu) {
  koordinat = (koordinat + dadu) % 40;
}


int User::getUang() const {
  return this->uang;
}

std::string User::getUsername() const {
  return this->username;
}

int User::getKoordinat() const {
  return this->koordinat;
}

void User::addProperti(Properti* p) {
  this->listProperti.push_back(p);
}

void User::removeProperti(Properti* p) {
  this->listProperti.erase(
    std::remove(this->listProperti.begin(), this->listProperti.end(), p),
    this->listProperti.end());
}

const std::vector<Properti*>& User::getListProperti() const {
  return this->listProperti;
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

User& User::operator+=(int jumlahUang) {
  this->uang += jumlahUang;
  return *this;
}

User& User::operator-=(int jumlahUang) {
  this->uang -= jumlahUang;
  return *this;
}