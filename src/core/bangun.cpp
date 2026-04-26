#include "../../include/core/bangun.hpp"
#include <iostream>

void Bangun::eksekusiBangun(User* pemain, Street* properti, const std::vector<Street*>& grupWarna) {
    if (!pemain || !properti) {
        return;
    }

    int hargaBangunan = properti->getHargaBangunan();
    if (properti->getJumlahBangunan() == 4 && !properti->isHotel()) {
        hargaBangunan = properti->getHargaHotel();
    }

    // 1. Cek apakah pemain punya cukup uang
    if (pemain->getUang() < hargaBangunan) {
        throw UangTidakCukupException();
    }

    // 2. Bangun rumah atau hotel
    if (properti->getJumlahBangunan() == 4 && !properti->isHotel()) {
        properti->bangunHotel(grupWarna);
        std::cout << "[INFO] Berhasil membangun Hotel di " << properti->getNama() << "!\n";
    } else if (properti->getJumlahBangunan() < 4 && !properti->isHotel()) {
        properti->bangunRumah(grupWarna);
        std::cout << "[INFO] Berhasil membangun Rumah di " << properti->getNama() 
                  << " (Total Rumah: " << properti->getJumlahBangunan() << ")!\n";
    } else {
        throw SyaratPembangunanException("Properti sudah memiliki Hotel, tidak bisa dibangun lagi!");
    }

    // 3. Kurangi uang pemain jika pembangunan berhasil
    *pemain -= hargaBangunan;
    std::cout << "[INFO] Sisa uang " << pemain->getUsername() << ": " << pemain->getUang() << "\n";
}

bool Bangun::isOwnedBuildableGroup(const std::vector<Street*>& streets, const User& user) {
    if (streets.empty()) {
        return false;
    }
    for (Street* street : streets) {
        if (street == nullptr || street->getOwner() != &user || street->getStatus() != PropStatus::OWNED) {
            return false;
        }
    }
    return true;
}

bool Bangun::canUpgradeHotel(Street* target, const std::vector<Street*>& streets) {
    if (target == nullptr || target->isHotel() || target->getJumlahBangunan() != 4) {
        return false;
    }
    for (Street* street : streets) {
        if (street == nullptr) {
            return false;
        }
        if (!street->isHotel() && street->getJumlahBangunan() != 4) {
            return false;
        }
    }
    return true;
}

bool Bangun::canBuildOnStreet(Street* street, const std::vector<Street*>& streets) {
    if (street == nullptr || street->isHotel()) {
        return false;
    }
    if (street->getJumlahBangunan() < 4) {
        return street->canBuild(streets);
    }
    return canUpgradeHotel(street, streets);
}
