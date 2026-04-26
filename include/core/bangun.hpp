#ifndef BANGUN_HPP
#define BANGUN_HPP

#include "user.hpp"
#include "properti.hpp"
#include <vector>

class Bangun {
public:
    // Menangani:
    // 1. Pengecekan apakah pemain memiliki uang yang cukup untuk membangun.
    // 2. Memanggil bangunRumah atau bangunHotel berdasarkan jumlah bangunan saat ini.
    // 3. Mengurangi uang pemain sebesar harga bangunan.
    static void eksekusiBangun(User* pemain, Street* properti, const std::vector<Street*>& grupWarna);
    static bool isOwnedBuildableGroup(const std::vector<Street*>& streets, const User& user);
    static bool canUpgradeHotel(Street* target, const std::vector<Street*>& streets);
    static bool canBuildOnStreet(Street* street, const std::vector<Street*>& streets);
};

#endif
