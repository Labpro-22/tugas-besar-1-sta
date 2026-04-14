#include "properti.hpp"

Properti::Properti(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai) {
    this->id = id;
    this->kode = kode;
    this->nama = nama;
    this->hargaBeli = hargaBeli;
    this->nilaiGadai = nilaiGadai;
    this->status = PropStatus::BANK;
    this->owner = nullptr;
    
    // Inisialisasi atribut festival
    this->durasiFestival = 0;
    this->festivalMultiplier = 1; 
}

void Properti::gadaikan() {
    if (status == PropStatus::OWNED) {
        status = PropStatus::MORTGAGED;
    }
}

void Properti::tebus() {
    if (status == PropStatus::MORTGAGED) {
        status = PropStatus::OWNED;
    }
}

PropStatus Properti::getStatus() const { 
    return status; 
}

void Properti::setOwner(User* newOwner) {
    this->owner = newOwner;
    this->status = (newOwner != nullptr) ? PropStatus::OWNED : PropStatus::BANK;
}

User* Properti::getOwner() const {
    return owner;
}
std::string Properti::getNama() const {
    return nama;
}
int Properti::getHargaBeli() const { 
    return hargaBeli;
}
int Properti::getId() const { 
    return id; 
}

int Properti::getFestivalMultiplier() const { 
    return festivalMultiplier;
}

void Properti::setFestivalMultiplier(int m) { 
    festivalMultiplier = m;
}

int Properti::getFestivalDuration() const { 
    return durasiFestival;
}

void Properti::setFestivalDuration(int d) { 
    durasiFestival = d;
}

int Properti::getSewaSaatIni(int dadu) const {
    return hitungSewa(dadu) * festivalMultiplier;
};