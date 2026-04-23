#include <algorithm>
#include <cmath>

#include "../../include/core/properti.hpp"
#include "../../include/core/user.hpp"

// ===Class Properti===
Properti::Properti(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai, std::string warna)
: index(id),nama(nama),hargaBeli(hargaBeli),nilaiGadai(nilaiGadai),status(PropStatus::BANK),owner(nullptr),kode(kode),warna(warna),durasiFestival(0),festivalMultiplier(1){}

Properti::~Properti() = default;

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
// Properti : Getter
PropStatus Properti::getStatus() const {return this->status;}
User* Properti::getOwner() const {return this->owner;}
std::string Properti::getNama() const {return this->nama;}
int Properti::getHargaBeli() const {return this->hargaBeli;}
int Properti::getId() const {return this->index;}
std::string Properti::getKode() const {return this->kode;}
std::string Properti::getWarna() const {return this->warna;}
int Properti::getFestivalMultiplier() const {return festivalMultiplier;}
int Properti::getFestivalDuration() const {return durasiFestival;}
int Properti::getNilaiGadai() const {return nilaiGadai;}
// Properti : Setter
void Properti::setFestivalMultiplier(int m){this->festivalMultiplier=m;}
void Properti::setOwner(User* newOwner) {
  if (this->owner == newOwner) {
    return;
  }

  if (this->owner != nullptr) {
    this->owner->removeProperti(this);
  }

  this->owner = newOwner;

  if (this->owner != nullptr) {
    this->owner->addProperti(this);
  }
}
void Properti::setFestivalDuration(int d){this->durasiFestival = std::max(0, d);}
void Properti::setStatus(PropStatus newStatus) {this->status = newStatus;}
void Properti::tickFestival() {
  if (durasiFestival <= 0) {
    return;
  }

  durasiFestival--;
  if (durasiFestival == 0) {
    festivalMultiplier = 1;
  }
}


// === Class Street === {Inheritance dari Properti}
Street::Street(int id, std::string kode, std::string nama, std::string warna, int hargaBeli, int nilaiGadai, int hargaBangunan, int hargaHotel, std::vector<int> sewa)
    : Properti(id, kode, nama, hargaBeli, nilaiGadai, warna),hargaSewa(sewa),jumlahRumah(0),hargaBangunan(hargaBangunan),hargaHotel(hargaHotel),hasHotel(false) {}

int Street::hitungSewa(int lemparanDadu) const {
  (void) lemparanDadu;
  if (this->status == PropStatus::MORTGAGED) {return 0;}

  int currentBiayaSewa = 0;
  if (this->hasHotel) {currentBiayaSewa = this->hargaSewa[5];} 
  else if (jumlahRumah > 0) {currentBiayaSewa = this->hargaSewa[this->jumlahRumah];} 
  else {
    currentBiayaSewa = this->hargaSewa[0];
  }

  if (this->durasiFestival > 0) {
    currentBiayaSewa *= this->festivalMultiplier;
  }

  return currentBiayaSewa;
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

int Street::getJumlahBangunan() const {return this->jumlahRumah;}
int Street::getHargaBangunan() const {return this->hargaBangunan;}
int Street::getHargaHotel() const {return this->hargaHotel;}
std::vector<int> Street::getHargaSewa() const {return this->hargaSewa;}
bool Street::isHotel() const{return this->hasHotel;}


// === Class RailRoad === {Inheritance dari Properti}
RailRoad::RailRoad(int id, std::string kode, std::string nama, int nilaiGadai, int hargaBeli, std::string warna, std::vector<int> hargaSewaDasar) 
: Properti(id, kode, nama, hargaBeli, nilaiGadai, warna) ,hargaSewa(hargaSewaDasar){}

int RailRoad::hitungSewa(int lemparanDadu) const {
  (void) lemparanDadu;
  if (this->owner == nullptr || this->status == PropStatus::MORTGAGED) {return 0;}

  const int jumlahRailroad = this->owner->getRailroadCount();
  if (jumlahRailroad <= 0 || jumlahRailroad > static_cast<int>(this->hargaSewa.size())) {
    return 0;
  }

  return this->hargaSewa[jumlahRailroad - 1];
}

// === Class Utility === {Inheritance dari Properti}
Utility::Utility(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai, std::string warna, std::vector<int> faktor)
    : Properti(id, kode, nama, hargaBeli, nilaiGadai, warna),faktorPengali(faktor) {}

int Utility::hitungSewa(int lemparanDadu) const {
  if (this->owner == nullptr || this->status == PropStatus::MORTGAGED) {return 0;}

  int jumlahUtil = this->owner->getUtilityCount();
  if (jumlahUtil == 1) {return faktorPengali[0] * lemparanDadu;} 
  else if (jumlahUtil == 2) {return faktorPengali[1] * lemparanDadu;} 
  else {return 0;}
}
