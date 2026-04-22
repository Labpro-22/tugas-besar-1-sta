#include "boardView.hpp"
#include "petak.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace {
    const std::string RESET = "\033[0m";
    const int CELL_WIDTH = 8;
    const int MIDDLE_SPACE_WIDTH = 98;
    const int LEGEND_WIDTH = 43;

    // Helper untuk padding rata kiri
    std::string pad(const std::string& s, size_t width) {
        if (s.length() >= width) return s.substr(0, width);
        return s + std::string(width - s.length(), ' ');
    }

    // Helper untuk rata tengah
    std::string centerText(const std::string& s, int width) {
        if (s.length() >= (size_t)width) return s.substr(0, width);
        int padLeft = (width - s.length()) / 2;
        int padRight = width - s.length() - padLeft;
        return std::string(padLeft, ' ') + s + std::string(padRight, ' ');
    }

    void printFullBorder() {
        for (int i = 0; i < 11; ++i) std::cout << "+----------";
        std::cout << "+\n";
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

    // Fungsi canggih untuk memetakan teks legenda ke tengah papan
    std::string getCenterLine(int lineIdx, int turn, int maxTurn) {
        std::string turnText = centerText("TURN " + std::to_string(turn) + " / " + std::to_string(maxTurn), LEGEND_WIDTH);

        std::vector<std::string> content = {
            "", // 0
            "===========================================", // 1
            "||              NIMONSPOLI               ||", // 2
            "===========================================", // 3
            "", // 4
            turnText, // 5
            "", // 6
            "-------------------------------------------", // 7
            pad("LEGENDA KEPEMILIKAN & STATUS", LEGEND_WIDTH), // 8
            pad("P1-P4 : Properti milik Pemain 1-4", LEGEND_WIDTH), // 9
            pad("^     : Rumah Level 1", LEGEND_WIDTH), // 10
            pad("^^    : Rumah Level 2", LEGEND_WIDTH), // 11
            pad("^^^   : Rumah Level 3", LEGEND_WIDTH), // 12
            pad("* : Hotel (Maksimal)", LEGEND_WIDTH), // 13
            pad("(1)-(4): Bidak (IN=Tahanan, V=Mampir)", LEGEND_WIDTH), // 14
            "-------------------------------------------"  // 15
            // BAGIAN KODE WARNA SUDAH DIHAPUS DARI SINI
        };

        if (lineIdx >= 0 && lineIdx < content.size() && !content[lineIdx].empty()) {
            return centerText(content[lineIdx], MIDDLE_SPACE_WIDTH);
        }
        // Sisa baris kosong di bawahnya akan otomatis diisi spasi
        return std::string(MIDDLE_SPACE_WIDTH, ' ');
    }
}

// IMPLEMENTASI YANG TERHAPUS DIKEMBALIKAN DI SINI
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

void BoardView::cetakPapan(Board& board, const std::vector<User>& players, int turn, int maxTurn) {
    std::cout << '\n';
    
    printFullBorder();
    for (int i = 20; i <= 30; ++i) std::cout << formatLine1(board.getPetakAt(i));
    std::cout << "|\n";
    for (int i = 20; i <= 30; ++i) std::cout << formatLine2(board.getPetakAt(i), players);
    std::cout << "|\n";
    printFullBorder();

    int leftIdx = 19;
    int rightIdx = 31;
    int centerLineCounter = 0; // Menghitung baris ke-berapa yang sedang dicetak

    while (leftIdx >= 11 && rightIdx <= 39) {
        // Cetak Baris 1: Nama Petak + Legenda Tengah
        std::cout << formatLine1(board.getPetakAt(leftIdx)) << "|";
        std::cout << getCenterLine(centerLineCounter++, turn, maxTurn);
        std::cout << formatLine1(board.getPetakAt(rightIdx)) << "|\n";

        // Cetak Baris 2: Bidak Pemain + Legenda Tengah
        std::cout << formatLine2(board.getPetakAt(leftIdx), players) << "|";
        std::cout << getCenterLine(centerLineCounter++, turn, maxTurn);
        std::cout << formatLine2(board.getPetakAt(rightIdx), players) << "|\n";

        // Cetak Baris 3: Garis Pembatas + Legenda Tengah
        if (leftIdx > 11) {
            std::cout << "+----------+" << getCenterLine(centerLineCounter++, turn, maxTurn) << "+----------+\n";
        }

        leftIdx--;
        rightIdx++;
    }

    printFullBorder();
    for (int i = 10; i >= 0; --i) std::cout << formatLine1(board.getPetakAt(i));
    std::cout << "|\n";
    for (int i = 10; i >= 0; --i) std::cout << formatLine2(board.getPetakAt(i), players);
    std::cout << "|\n";
    printFullBorder();
    std::cout << "\n";
}