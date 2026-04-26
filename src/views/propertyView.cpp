#include "views/propertyView.hpp"
#include "core/properti.hpp"
#include "core/user.hpp"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
    std::string formatUang(int uang) {
        std::string angka = std::to_string(uang);
        for (int i = static_cast<int>(angka.length()) - 3; i > 0; i -= 3) {
            angka.insert(i, ".");
        }
        return "M" + angka;
    }

    std::string centerText(const std::string& text) {
        const int width = 32;
        if (static_cast<int>(text.length()) >= width) {
            return text.substr(0, width);
        }
        int leftPad = (width - static_cast<int>(text.length())) / 2;
        int rightPad = width - static_cast<int>(text.length()) - leftPad;
        return std::string(leftPad, ' ') + text + std::string(rightPad, ' ');
    }

    std::string statusText(const Properti* properti) {
        if (properti == nullptr) {
            return "-";
        }

        std::string status;
        if (properti->getStatus() == PropStatus::BANK) {
            status = "BANK";
        } else if (properti->getStatus() == PropStatus::OWNED) {
            status = "OWNED";
        } else {
            status = "MORTGAGED";
        }

        if (properti->getOwner() != nullptr) {
            status += " (" + properti->getOwner()->getUsername() + ")";
        }
        return status;
    }

    void printRow(const std::string& label, const std::string& value) {
        const int width = 32;
        std::ostringstream row;
        row << " " << std::left << std::setw(18) << label << ": " << value;
        std::string content = row.str();
        if (static_cast<int>(content.length()) > width) {
            content = content.substr(0, width);
        }
        std::cout << "|" << content << std::string(width - content.length(), ' ') << "|\n";
    }

    void printStatus(const Properti* properti) {
        const int width = 32;
        std::string content = " Status : " + statusText(properti);
        if (static_cast<int>(content.length()) > width) {
            content = content.substr(0, width);
        }
        std::cout << "|" << content << std::string(width - content.length(), ' ') << "|\n";
    }
}

void PropertyView::cetakAkta(Properti* properti) {
    if (properti == nullptr) {
        std::cout << "Properti tidak ditemukan.\n";
        return;
    }

    Street* street = dynamic_cast<Street*>(properti);

    std::cout << "+================================+\n";
    std::cout << "|" << centerText("AKTA KEPEMILIKAN") << "|\n";
    std::cout << "|" << centerText("[" + properti->getWarna() + "] " + properti->getNama() + " (" + properti->getKode() + ")") << "|\n";
    std::cout << "+================================+\n";
    printRow("Harga Beli", formatUang(properti->getHargaBeli()));
    printRow("Nilai Gadai", formatUang(properti->getNilaiGadai()));

    if (street != nullptr) {
        std::vector<int> hargaSewa = street->getHargaSewa();
        std::cout << "+--------------------------------+\n";
        if (hargaSewa.size() > 0) printRow("Sewa (unimproved)", formatUang(hargaSewa[0]));
        if (hargaSewa.size() > 1) printRow("Sewa (1 rumah)", formatUang(hargaSewa[1]));
        if (hargaSewa.size() > 2) printRow("Sewa (2 rumah)", formatUang(hargaSewa[2]));
        if (hargaSewa.size() > 3) printRow("Sewa (3 rumah)", formatUang(hargaSewa[3]));
        if (hargaSewa.size() > 4) printRow("Sewa (4 rumah)", formatUang(hargaSewa[4]));
        if (hargaSewa.size() > 5) printRow("Sewa (hotel)", formatUang(hargaSewa[5]));
        std::cout << "+--------------------------------+\n";
        printRow("Harga Rumah", formatUang(street->getHargaBangunan()));
        printRow("Harga Hotel", formatUang(street->getHargaHotel()));
    }

    std::cout << "+================================+\n";
    printStatus(properti);
    std::cout << "+================================+\n";
}

void PropertyView::cetakProperti(const User& user) {
    const std::vector<Properti*>& properti = user.getListProperti();

    std::cout << "=== Properti Milik " << user.getUsername() << " ===\n";
    if (properti.empty()) {
        std::cout << "Belum memiliki properti.\n";
        return;
    }

    for (size_t i = 0; i < properti.size(); ++i) {
        Properti* prop = properti[i];
        if (prop == nullptr) {
            continue;
        }

        std::cout << i + 1 << ". "
                  << prop->getNama() << " (" << prop->getKode() << ")"
                  << " [" << prop->getWarna() << "]"
                  << " | Harga: " << formatUang(prop->getHargaBeli())
                  << " | Gadai: " << formatUang(prop->getNilaiGadai())
                  << " | Status: " << statusText(prop);

        Street* street = dynamic_cast<Street*>(prop);
        if (street != nullptr) {
            if (street->isHotel()) {
                std::cout << " | Bangunan: Hotel";
            } else {
                std::cout << " | Rumah: " << street->getJumlahBangunan();
            }
        }

        std::cout << "\n";
    }
}
