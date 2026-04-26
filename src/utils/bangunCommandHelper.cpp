#include "utils/bangunCommandHelper.hpp"
#include <iostream>
#include <utility>

BuildGroup::BuildGroup(std::string warna, std::vector<Street*> streets, std::vector<Street*> eligible)
    : warna(std::move(warna)), streets(std::move(streets)), eligible(std::move(eligible)) {}

const std::string& BuildGroup::getWarna() const {
    return warna;
}

const std::vector<Street*>& BuildGroup::getStreets() const {
    return streets;
}

const std::vector<Street*>& BuildGroup::getEligible() const {
    return eligible;
}

std::string BangunCommandHelper::formatUangCommand(int uang) {
    std::string angka = std::to_string(uang);
    for (int i = static_cast<int>(angka.length()) - 3; i > 0; i -= 3) {
        angka.insert(i, ".");
    }
    return "M" + angka;
}

std::string BangunCommandHelper::displayWarna(const std::string& warna) {
    std::string result = warna;
    for (char& c : result) {
        if (c == '_') {
            c = ' ';
        }
    }
    return result;
}

std::string BangunCommandHelper::statusBangunan(Street* street) {
    if (street == nullptr) {
        return "-";
    }
    if (street->isHotel()) {
        return "Hotel";
    }
    return std::to_string(street->getJumlahBangunan()) + " rumah";
}

std::vector<Street*> BangunCommandHelper::getStreetGroup(const std::vector<PetakProperti*>& petakGroup) {
    std::vector<Street*> streets;
    for (PetakProperti* petak : petakGroup) {
        if (petak == nullptr) {
            continue;
        }
        Street* street = dynamic_cast<Street*>(petak->getSertifikat());
        if (street != nullptr) {
            streets.push_back(street);
        }
    }
    return streets;
}

void BangunCommandHelper::printStreetBuildStatus(Street* street) {
    if (street == nullptr) {
        return;
    }
    std::cout << "- " << street->getNama() << " (" << street->getKode() << ") : " << statusBangunan(street);
    if (!street->isHotel() && street->getJumlahBangunan() == 4) {
        std::cout << " (Harga hotel: " << formatUangCommand(street->getHargaHotel()) << ")";
    } else if (!street->isHotel()) {
        std::cout << " (Harga rumah: " << formatUangCommand(street->getHargaBangunan()) << ")";
    }
    std::cout << "\n";
}
