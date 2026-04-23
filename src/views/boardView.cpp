#include "views/boardView.hpp"
#include "core/petak.hpp"
#include <iostream>
#include <string>

namespace {
    const std::string RESET = "\033[0m";
    const int CELL_WIDTH = 8;
    const int MIDDLE_SPACE_WIDTH = 98;

    std::string pad(const std::string& s, size_t width) {
        if (s.length() >= width) return s.substr(0, width);
        return s + std::string(width - s.length(), ' ');
    }

    void printFullBorder() {
        for (int i = 0; i < 11; ++i) std::cout << "+----------";
        std::cout << "+\n";
    }

    void printMiddleBorder() {
        std::cout << "+----------+" << std::string(MIDDLE_SPACE_WIDTH, ' ') << "+----------+\n";
    }

    std::string formatLine1(Petak* petak) {
        if (!petak) return "| " + pad("KOSONG", CELL_WIDTH) + " ";
        
        std::string warna = "Default";
        if (auto* lahan = dynamic_cast<PetakLahan*>(petak)) {
            warna = lahan->getWarna();
        }

        std::string color = BoardView::getColorCode(warna);
        std::string kode = petak->getKodePetak();
        kode.resize(3, ' '); 
        
        return "| " + color + pad("[" + kode + "]", CELL_WIDTH) + RESET + " ";
    }

    std::string formatLine2(Petak* petak, const std::vector<User>& players) {
        if (!petak) return "| " + pad("", CELL_WIDTH) + " ";
        
        std::string bidak;
        for (size_t i = 0; i < players.size(); ++i) {
            if (players[i].getKoordinat() == petak->getIndex()) {
                bidak += "P" + std::to_string(i + 1) + " ";
            }
        }
        return "| " + pad(bidak, CELL_WIDTH) + " ";
    }
}

std::string BoardView::getColorCode(const std::string& warna) {
    if (warna == "Coklat") return "\033[38;5;94m";
    if (warna == "BiruMuda") return "\033[38;5;117m"; 
    if (warna == "MerahMuda") return "\033[38;5;213m";
    if (warna == "Orange") return "\033[38;5;208m";
    if (warna == "Merah") return "\033[31m";
    if (warna == "Kuning") return "\033[33m";
    if (warna == "Hijau") return "\033[32m";
    if (warna == "BiruTua") return "\033[34m";
    return RESET;
}

void BoardView::cetakPapan(Board& board, const std::vector<User>& players) {
    std::cout << '\n';
    
    printFullBorder();
    for (int i = 20; i <= 30; ++i) std::cout << formatLine1(board.getPetakAt(i));
    std::cout << "|\n";
    for (int i = 20; i <= 30; ++i) std::cout << formatLine2(board.getPetakAt(i), players);
    std::cout << "|\n";
    printFullBorder();

    int leftIdx = 19;
    int rightIdx = 31;
    while (leftIdx >= 11 && rightIdx <= 39) {
        std::cout << formatLine1(board.getPetakAt(leftIdx)) << "|";
        
        if (leftIdx == 15) {
            std::cout << pad(" ", 27) << "===========================================" << pad(" ", 28);
        } else if (leftIdx == 14) {
            std::cout << pad(" ", 27) << "||              NIMONSPOLI               ||" << pad(" ", 28);
        } else if (leftIdx == 13) {
            std::cout << pad(" ", 27) << "===========================================" << pad(" ", 28);
        } else {
            std::cout << std::string(MIDDLE_SPACE_WIDTH, ' ');
        }
        
        std::cout << formatLine1(board.getPetakAt(rightIdx)) << "|\n";

        std::cout << formatLine2(board.getPetakAt(leftIdx), players) << "|";
        std::cout << std::string(MIDDLE_SPACE_WIDTH, ' ');
        std::cout << formatLine2(board.getPetakAt(rightIdx), players) << "|\n";

        if (leftIdx > 11) printMiddleBorder();

        leftIdx--;
        rightIdx++;
    }

    printFullBorder();
    for (int i = 10; i >= 0; --i) std::cout << formatLine1(board.getPetakAt(i));
    std::cout << "|\n";
    for (int i = 10; i >= 0; --i) std::cout << formatLine2(board.getPetakAt(i), players);
    std::cout << "|\n";
    printFullBorder();

    std::cout << "\nLEGENDA:\n"
              << "P1-P4 : Bidak Pemain 1-4\n"
              << "WARNA : Mengikuti blok properti lahan\n\n";
}
