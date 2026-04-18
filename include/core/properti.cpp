#include <algorithm>
#include <cmath>

#include "properti.hpp"
#include "user.hpp"
#include "gameException.hpp"


Properti::Properti(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai) {
  this->id = id;
  this->kode = kode;
  this->nama = nama;
  this->hargaBeli = hargaBeli;
  this->nilaiGadai = nilaiGadai;
  this->status = PropStatus::BANK;
  this->owner = nullptr;
  this->durasiFestival = 0;
  this->festivalMultiplier = 1;
}


void Properti::gadaikan() {
  if (this->status != PropStatus::OWNED) {
    throw StatusPropertiInvalidException("Gagal Gadai: Properti harus berstatus OWNED!");
  }

  this->status = PropStatus::MORTGAGED;

  *(this->owner) += this->nilaiGadai;

}

void Properti::tebus() {
  if (this->status != PropStatus::MORTGAGED){
    throw SyaratPropertiInvalidException("Gagal Tebus: Properti ini tidak sedang digadaikan!");
  }

  if (this->owner->getUang() < this->hargaBeli) {
    throw UangTidakCukupException();
  }

  *(this->owner) -= this->hargaBeli;
  this->status = PropStatus::OWNED;
}

PropStatus Properti::getStatus() const {
  return this->status;
}

User* Properti::getOwner() const {
  return this->owner;
}

std::string Properti::getNama() const {
  return this->nama;
}

int Properti::getHargaBeli() const {
  return this->hargaBeli;
}

int Properti::getId() const {
  return this->id;
}

void Properti::setOwner(User* newOwner) {
  this->owner = newOwner;
}

Street::Street(int id, std::string kode, std::string nama, std::string warna, int hargaBeli, int nilaiGadai, int hargaBangunan, std::vector<int> sewa)
    : Properti(id, kode, nama, hargaBeli, nilaiGadai) {
  this->warna = warna;
  this->hargaBangunan = hargaBangunan;
  this->hargaSewa = sewa;
  this->jumlahRumah = 0;
  this->hasHotel = false;
}

RailRoad::RailRoad(int id, std::string kode, std::string nama, int nilaiGadai, int hargaBeli, int hargaSewaDasar) : Properti(id, kode, nama, hargaBeli, nilaiGadai) {
  this->hargaSewaDasar = hargaSewaDasar;
}

Utility::Utility(int id, std::string kode, std::string nama, int nilaiGadai)
    : Properti(id, kode, nama, 150, nilaiGadai) {}

int Street::hitungSewa(int lemparanDadu) const {
  if (this->status == PropStatus::MORTGAGED) {
    return 0;
  }

  int currentBiayaSewa = 0;

  if (this->hasHotel) {
    currentBiayaSewa = this->hargaSewa[5];
  } else if (jumlahRumah > 0) {
    currentBiayaSewa = this->hargaSewa[this->jumlahRumah];
  } else {
    currentBiayaSewa = this->hargaSewa[0];
  }

  if (this->durasiFestival > 0) {
    currentBiayaSewa *= this->festivalMultiplier;
  }

  return currentBiayaSewa;
}

int Street::hitungSewa(int lemparanDadu, const std::vector<Street*>& grupWarna) const {
  if (this->status == PropStatus::MORTGAGED) {
    return 0;
  }

  int currentBiayaSewa = 0;

  if (this->hasHotel) {
    currentBiayaSewa = this->hargaSewa[5];
  } else if (jumlahRumah > 0) {
    currentBiayaSewa = this->hargaSewa[this->jumlahRumah];
  } else {
    currentBiayaSewa = this->hargaSewa[0];

    if (this->owner != nullptr && this->owner->hasMonopoli(this->warna, grupWarna.size())) {
      currentBiayaSewa *= 2;
    }
  }

  if (this->durasiFestival > 0) {
    currentBiayaSewa *= this->festivalMultiplier;
  }

  return currentBiayaSewa;
}

void Street::bangunRumah(const std::vector<Street*>& grupWarna) {
  if (!this->canBuild(grupWarna)) {
    throw SyaratPembangunanException();
  }

  this->jumlahRumah += 1;
}

bool Street::canBuild(const std::vector<Street*>& grupWarna) const {
  if (this->owner == nullptr) {
    return false;
  }

  if (!this->owner->hasMonopoli(this->warna, grupWarna.size())) {
    return false;
  }

  if (this->hasHotel) {
    return false;
  }

  if (this->getJumlahBangunan() >= 4) {
    return false;
  }

  int simulasiRumah = this->getJumlahBangunan() + 1;

  for (Street* otherPetak : grupWarna) {
    if (otherPetak == this) {
      continue;
    }

    int jumlahOtherPetak = 0;
    if (otherPetak->hasHotel) {
      return false;
    } else {
      jumlahOtherPetak = otherPetak->getJumlahBangunan();
    }

    if ((simulasiRumah - jumlahOtherPetak) > 1) {
      return false;
    }
  }

  return true;
}

void Street::bangunHotel(const std::vector<Street*>& grupWarna) {
  if (this->jumlahRumah != 4) {
    throw SyaratPembangunanException();
  }

  if (this->hasHotel) {
    throw SyaratPembangunanException();
  }

  for (Street* otherPetak : grupWarna) {
    if (otherPetak == this) {
      continue;
    } 

    if (otherPetak->getJumlahBangunan() != 4) {
      throw SyaratPemerataanException();
    }
  }

  this->hasHotel = true;
}

void Street::jualBangunan(const std::vector<Street*>& grupWarna) {
  if (this->owner == nullptr) {
    throw BukanPemilikException();
  }

  if (!this->hasHotel && this->jumlahRumah == 0) {
    throw TidakAdaBangunanException();
  }

  int simulasiRumah = 0;
  if (this->hasHotel) {
    simulasiRumah = 4;
  } else {
    simulasiRumah = this->getJumlahBangunan() - 1;
  }

  for (Street* otherPetak : grupWarna) {
    if (otherPetak == this) {
      continue;
    }

    int jumlahOtherPetak = 0;
    if (otherPetak->hasHotel) {
      jumlahOtherPetak = 5;
    } else {
      jumlahOtherPetak = otherPetak->getJumlahBangunan();
    }

    if ((jumlahOtherPetak - simulasiRumah) > 1 ) {
      throw SyaratPemerataanException();
    }
  }

  if (this->hasHotel) {
    this->hasHotel = false;
  } else {
    this->jumlahRumah -= 1;
  }

  *(this->owner) += this->getHargaBangunan() * 0.5;

}

void Street::gadaikan() {
  if (this->owner == nullptr) {
    throw BukanPemilikException();
  }

  if (this->status != PropStatus::OWNED) {
    throw SyaratPropertiInvalidException("Gagal Gadai: Status tanah tidak valid!");
  }

  int duitGusur = 0;
  for (Street* otherPetak : this->owner->getStreetByColor(this->warna)) {
    if (otherPetak->hasHotel) {
      duitGusur += (otherPetak->getHargaBangunan() * 5) * 0.5;
      otherPetak->hasHotel = false;
      otherPetak->jumlahRumah = 0;
    } else if (otherPetak->getJumlahBangunan() > 0) {
      duitGusur += (otherPetak->getHargaBangunan() * otherPetak->getJumlahBangunan()) * 0.5;
      otherPetak->jumlahRumah = 0;
    }
  }

  if (duitGusur > 0) {
    *(this->owner) += duitGusur;
  }

  this->status = PropStatus::MORTGAGED;

  *(this->owner) += this->nilaiGadai;

}

std::string Street::getWarna() const {
  return this->warna;
}

int Street::getJumlahBangunan() const {
  return this->jumlahRumah;
}

int Street::getHargaBangunan() const {
  return this->hargaBangunan;
}

int RailRoad::hitungSewa(int lemparanDadu) const {
  if (this->owner == nullptr) {
    return 0;
  }

  if (this->status == PropStatus::MORTGAGED) {
    return 0;
  }

  int jumlahStasiun = this->owner->getRailroadCount();

  int currentBiayaSewa = this->hargaSewaDasar * std::pow(2, jumlahStasiun - 1);

  return currentBiayaSewa;
}

int Utility::hitungSewa(int lemparanDadu) const {
  if (this->owner == nullptr) {
    return 0;
  }

  if (this->status == PropStatus::MORTGAGED) {
    return 0;
  }

  int jumlahUtil = this->owner->getUtilityCount();

  if (jumlahUtil == 1) {
    return 4 * lemparanDadu;
  } else if (jumlahUtil == 2) {
    return 10 * lemparanDadu;
  } else {
    return 0;
  }
}