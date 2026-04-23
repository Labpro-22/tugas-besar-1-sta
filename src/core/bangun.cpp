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
