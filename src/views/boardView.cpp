#include "views/boardView.hpp"
#include "core/petak.hpp"
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

    // [REFACTOR 1]: Optimasi Memori menggunakan 'static const'
    std::string getCenterLine(int lineIdx, int turn, int maxTurn) {
        // Karena array ini isinya tetap (kecuali baris turn), kita jadikan static
        // agar C++ tidak perlu membuat ulang array ini setiap kali fungsi dipanggil.
        static const std::vector<std::string> staticContent = {
            "", // 0
            "===========================================", // 1
            "||              NIMONSPOLI               ||", // 2
            "===========================================", // 3
            "", // 4
            "", // 5 -> Dikosongkan karena nilainya dinamis
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
        };

        // Inject teks dinamis khusus untuk baris ke-5
        if (lineIdx == 5) {
            std::string turnText = "TURN " + std::to_string(turn) + " / " + std::to_string(maxTurn);
            return centerText(centerText(turnText, LEGEND_WIDTH), MIDDLE_SPACE_WIDTH);
        }

        // Cetak teks statis jika indeksnya ada di dalam vector
        if (lineIdx >= 0 && lineIdx < static_cast<int>(staticContent.size()) && !staticContent[lineIdx].empty()) {
            return centerText(staticContent[lineIdx], MIDDLE_SPACE_WIDTH);
        }
        
        return std::string(MIDDLE_SPACE_WIDTH, ' '); // Sisa baris diisi spasi kosong
    }

    // [REFACTOR 2]: Abstraksi fungsi untuk mencetak baris horizontal (Atas / Bawah)
    // Ini menghilangkan duplikasi looping for yang panjang.
    void printHorizontalRow(Board& board, const std::vector<User>& players, int startIdx, int endIdx, int step) {
        printFullBorder();
        for (int i = startIdx; i != endIdx + step; i += step) {
            std::cout << formatLine1(board.getPetakAt(i));
        }
        std::cout << "|\n";
        for (int i = startIdx; i != endIdx + step; i += step) {
            std::cout << formatLine2(board.getPetakAt(i), players);
        }
        std::cout << "|\n";
        printFullBorder();
    }
}

std::string BoardView::getColorCode(const std::string& warna) {
    if (warna == "Coklat") return "\033[38;5;94m";
    if (warna == "BiruMuda") return "\033[38;5;117m"; 
    if (warna == "MerahMuda" || warna == "Pink") return "\033[38;5;213m";
    if (warna == "Orange") return "\033[38;5;208m";
    if (warna == "Merah") return "\033[31m";
    if (warna == "Kuning") return "\033[33m";
    if (warna == "Hijau") return "\033[32m";
    if (warna == "BiruTua") return "\033[34m";
    return RESET;
}

void BoardView::cetakPapan(Board& board, const std::vector<User>& players, int turn, int maxTurn) {
    std::cout << '\n';
    
    int totalPetak = 40; 
    int S = totalPetak / 4;
    
    int topStart   = 2 * S;         // 20
    int topEnd     = 3 * S;         // 30
    int botStart   = S;             // 10
    int botEnd     = 0;             // 0
    
    int leftStart  = 2 * S - 1;     // 19
    int leftEnd    = S + 1;         // 11
    int rightStart = 3 * S + 1;     // 31
    int rightEnd   = totalPetak - 1;// 39

    // 1. CETAK BARIS ATAS (Memanggil helper, step maju +1)
    printHorizontalRow(board, players, topStart, topEnd, 1);

    // 2. CETAK BARIS TENGAH (Vertikal)
    int leftIdx = leftStart;
    int rightIdx = rightStart;
    int centerLineCounter = 0; 

    while (leftIdx >= leftEnd && rightIdx <= rightEnd) {
        std::cout << formatLine1(board.getPetakAt(leftIdx)) << "|";
        std::cout << getCenterLine(centerLineCounter++, turn, maxTurn);
        std::cout << formatLine1(board.getPetakAt(rightIdx)) << "|\n";

        std::cout << formatLine2(board.getPetakAt(leftIdx), players) << "|";
        std::cout << getCenterLine(centerLineCounter++, turn, maxTurn);
        std::cout << formatLine2(board.getPetakAt(rightIdx), players) << "|\n";

        if (leftIdx > leftEnd) {
            std::cout << "+----------+" << getCenterLine(centerLineCounter++, turn, maxTurn) << "+----------+\n";
        }
        leftIdx--;
        rightIdx++;
    }

    // 3. CETAK BARIS BAWAH (Memanggil helper, step mundur -1)
    printHorizontalRow(board, players, botStart, botEnd, -1);
    
    std::cout << "\n";
}