#include "views/boardView.hpp"
#include "core/game.hpp"
#include "core/petak.hpp"
#include <iostream>

int BoardViewRenderer::perimeterLength(int rows, int cols) const {
    return 2 * (rows + cols) - 4;
}

void BoardViewRenderer::buildLayout() {
    const int totalPetak = board.getSize();
    rowCount = 3;
    colCount = 3;

    int bestRows = 3;
    int bestCols = 3;
    int bestWaste = 1000000;
    int bestBalance = 1000000;

    for (int rows = 3; rows <= totalPetak + 2; ++rows) {
        for (int cols = 3; cols <= totalPetak + 2; ++cols) {
            const int perimeter = perimeterLength(rows, cols);
            if (perimeter < totalPetak) {
                continue;
            }

            const int waste = perimeter - totalPetak;
            const int balance = rows > cols ? rows - cols : cols - rows;
            if (waste < bestWaste || (waste == bestWaste && balance < bestBalance) ||
                (waste == bestWaste && balance == bestBalance && cols >= rows && bestCols < bestRows)) {
                bestRows = rows;
                bestCols = cols;
                bestWaste = waste;
                bestBalance = balance;
            }
        }
    }

    rowCount = bestRows;
    colCount = bestCols;
    middleSpaceWidth = (colCount - 2) * (CELL_WIDTH + 1) - 1;
    if (middleSpaceWidth < 24) {
        middleSpaceWidth = 24;
    }

    layout.assign(rowCount, std::vector<int>(colCount, -1));

    int index = 0;
    for (int col = colCount - 1; col >= 0 && index < totalPetak; --col) {
        layout[rowCount - 1][col] = index++;
    }
    for (int row = rowCount - 2; row >= 1 && index < totalPetak; --row) {
        layout[row][0] = index++;
    }
    for (int col = 0; col < colCount && index < totalPetak; ++col) {
        layout[0][col] = index++;
    }
    for (int row = 1; row <= rowCount - 2 && index < totalPetak; ++row) {
        layout[row][colCount - 1] = index++;
    }
}

std::string BoardViewRenderer::fit(const std::string& text, int width) const {
    if (static_cast<int>(text.length()) >= width) {
        return text.substr(0, width);
    }
    return text + std::string(width - text.length(), ' ');
}

std::string BoardViewRenderer::center(const std::string& text, int width) const {
    if (static_cast<int>(text.length()) >= width) {
        return text.substr(0, width);
    }
    int leftPad = (width - static_cast<int>(text.length())) / 2;
    int rightPad = width - static_cast<int>(text.length()) - leftPad;
    return std::string(leftPad, ' ') + text + std::string(rightPad, ' ');
}

std::string BoardViewRenderer::colorize(const std::string& text, const std::string& color) const {
    if (color.empty() || color == RESET) {
        return text;
    }
    return color + text + RESET;
}

Properti* BoardViewRenderer::getProperti(Petak* petak) const {
    PetakProperti* petakProperti = dynamic_cast<PetakProperti*>(petak);
    if (petakProperti == nullptr) {
        return nullptr;
    }
    return petakProperti->getSertifikat();
}

std::string BoardViewRenderer::getPetakColor(Petak* petak) const {
    Properti* properti = getProperti(petak);
    if (properti != nullptr) {
        return BoardView::getColorCode(properti->getWarna());
    }

    if (petak == nullptr) {
        return "";
    }
    return BoardView::getColorCode(petak->getWarna());
}

std::string BoardViewRenderer::playerCode(const User* owner) const {
    if (owner == nullptr) {
        return "BANK";
    }

    for (size_t i = 0; i < players.size(); ++i) {
        if (&players[i] == owner) {
            return "P" + std::to_string(i + 1);
        }
    }

    std::string username = owner->getUsername();
    if (username.empty()) {
        return "?";
    }
    return username.substr(0, 4);
}

std::string BoardViewRenderer::buildingCode(Properti* properti) const {
    Street* street = dynamic_cast<Street*>(properti);
    if (street == nullptr) {
        return "";
    }

    if (street->isHotel()) {
        return "*";
    }

    int jumlahRumah = street->getJumlahBangunan();
    if (jumlahRumah <= 0) {
        return "";
    }
    return std::string(jumlahRumah, '^');
}

std::string BoardViewRenderer::propertyStatus(Petak* petak) const {
    Properti* properti = getProperti(petak);
    if (properti == nullptr) {
        return categoryCode(petak);
    }

    if (properti->getStatus() == PropStatus::BANK) {
        return "BANK";
    }

    std::string result = playerCode(properti->getOwner());
    if (properti->getStatus() == PropStatus::MORTGAGED) {
        return result + " GDI";
    }

    std::string bangunan = buildingCode(properti);
    if (!bangunan.empty()) {
        return result + " O " + bangunan;
    }
    return result + " OWN";
}

std::string BoardViewRenderer::categoryCode(Petak* petak) const {
    if (petak == nullptr) {
        return "KOSONG";
    }

    std::string kategori = petak->getKategori();
    if (kategori == "KARTU") {
        return "KARTU";
    }
    if (kategori == "PAJAK") {
        return "PAJAK";
    }
    if (kategori == "FESTIVAL") {
        return "FEST";
    }
    if (kategori == "SPESIAL") {
        return "SPCL";
    }
    if (kategori == "Stasiun") {
        return "STSN";
    }
    if (kategori == "Utilitas") {
        return "UTIL";
    }
    return kategori.substr(0, 6);
}

std::vector<std::string> BoardViewRenderer::pieceLines(Petak* petak) const {
    std::vector<std::string> lines(2, "");
    if (petak == nullptr) {
        return lines;
    }

    for (size_t i = 0; i < players.size(); ++i) {
        if (players[i].getKoordinat() != petak->getIndex()) {
            continue;
        }

        std::string token = "P" + std::to_string(i + 1);
        if (petak->getKodePetak() == "PEN") {
            token += players[i].isJailed() ? "IN" : "V";
        }

        int targetLine = 0;
        if (!lines[0].empty() && static_cast<int>(lines[0].length() + token.length() + 1) > CELL_WIDTH) {
            targetLine = 1;
        }

        if (!lines[targetLine].empty()) {
            lines[targetLine] += " ";
        }
        lines[targetLine] += token;
    }

    return lines;
}

std::string BoardViewRenderer::cellContent(Petak* petak, int lineIndex) const {
    if (petak == nullptr) {
        return lineIndex == 0 ? "KOSONG" : "";
    }

    if (lineIndex == 0) {
        std::string kode = petak->getKodePetak();
        return "[" + kode.substr(0, 3) + "]";
    }

    if (lineIndex == 1) {
        return propertyStatus(petak);
    }

    std::vector<std::string> bidak = pieceLines(petak);
    return bidak[lineIndex - 2];
}

std::string BoardViewRenderer::cellLine(int petakIndex, int lineIndex) const {
    Petak* petak = petakIndex >= 0 ? board.getPetakAt(petakIndex) : nullptr;
    std::string content = fit(cellContent(petak, lineIndex), CELL_WIDTH);
    if (lineIndex == 0) {
        content = colorize(content, getPetakColor(petak));
    }
    return "|" + content;
}

std::vector<std::string> BoardViewRenderer::centerContent() const {
    std::vector<std::string> content;
    content.push_back("===========================================");
    content.push_back("||              NIMONSPOLI               ||");
    content.push_back("===========================================");
    content.push_back("");

    if (turn >= 0 && maxTurn > 0) {
        content.push_back("TURN " + std::to_string(turn) + " / " + std::to_string(maxTurn));
    } else if (turn >= 0) {
        content.push_back("TURN " + std::to_string(turn) + " / ?");
    } else {
        content.push_back("TURN: -");
    }

    if (currentPlayerIndex >= 0 && currentPlayerIndex < static_cast<int>(players.size())) {
        content.push_back("GILIRAN: P" + std::to_string(currentPlayerIndex + 1) + " - " + players[currentPlayerIndex].getUsername());
    } else {
        content.push_back("GILIRAN: -");
    }

    content.push_back("");
    content.push_back("-------------------------------------------");
    content.push_back("LEGENDA PETAK");
    content.push_back("[XXX]  : Kode petak");
    content.push_back("BANK   : Belum dimiliki");
    content.push_back("P1 OWN : Properti milik pemain 1");
    content.push_back("P1 GDI : Properti pemain 1 digadaikan");
    content.push_back("P1 O ^ : P1 punya 1 rumah");
    content.push_back("^ ^^ ^^^ ^^^^ : Rumah level 1-4");
    content.push_back("*      : Hotel");
    content.push_back("");
    content.push_back("LEGENDA BIDAK");
    content.push_back("P1-P4  : Posisi pemain");
    content.push_back("P1IN   : P1 tahanan di penjara");
    content.push_back("P1V    : P1 mampir penjara");
    content.push_back("");
    content.push_back("LEGENDA WARNA");
    content.push_back("COKLAT, BIRU_MUDA, MERAH_MUDA, ORANGE");
    content.push_back("MERAH, KUNING, HIJAU, BIRU_TUA");
    content.push_back("ABU_ABU=Utilitas, DEFAULT=Aksi/Stasiun");
    content.push_back("Layout mengikuti ukuran board dinamis");
    content.push_back("-------------------------------------------");
    return content;
}

std::string BoardViewRenderer::getCenterLine(int lineIndex) const {
    std::vector<std::string> content = centerContent();
    if (lineIndex >= 0 && lineIndex < static_cast<int>(content.size())) {
        return center(content[lineIndex], middleSpaceWidth);
    }
    return std::string(middleSpaceWidth, ' ');
}

void BoardViewRenderer::printFullBorder() const {
    for (int i = 0; i < colCount; ++i) {
        std::cout << "+" << std::string(CELL_WIDTH, '-');
    }
    std::cout << "+\n";
}

void BoardViewRenderer::printMiddleBorder(int& centerLineCounter) const {
    std::cout << "+" << std::string(CELL_WIDTH, '-')
              << "+" << getCenterLine(centerLineCounter++)
              << "+" << std::string(CELL_WIDTH, '-') << "+\n";
}

void BoardViewRenderer::printHorizontalRow(int rowIndex) const {
    printFullBorder();
    for (int line = 0; line < CELL_HEIGHT; ++line) {
        for (int col = 0; col < colCount; ++col) {
            std::cout << cellLine(layout[rowIndex][col], line);
        }
        std::cout << "|\n";
    }
    printFullBorder();
}

BoardViewRenderer::BoardViewRenderer(Board& board, const std::vector<User>& players, int turn, int maxTurn, int currentPlayerIndex)
    : board(board),
      players(players),
      turn(turn),
      maxTurn(maxTurn),
      currentPlayerIndex(currentPlayerIndex),
      rowCount(0),
      colCount(0),
      middleSpaceWidth(0) {
    buildLayout();
}

void BoardViewRenderer::print() const {
    if (board.getSize() < 4) {
        std::cout << "Papan tidak valid.\n";
        return;
    }

    std::cout << '\n';
    printHorizontalRow(0);

    int centerLineCounter = 0;
    for (int row = 1; row < rowCount - 1; ++row) {
        for (int line = 0; line < CELL_HEIGHT; ++line) {
            std::cout << cellLine(layout[row][0], line) << "|";
            std::cout << getCenterLine(centerLineCounter++);
            std::cout << cellLine(layout[row][colCount - 1], line) << "|\n";
        }

        if (row < rowCount - 2) {
            printMiddleBorder(centerLineCounter);
        }
    }

    printHorizontalRow(rowCount - 1);
    std::cout << "\n";
}

std::string BoardView::getColorCode(const std::string& warna) {
    if (warna == "Coklat" || warna == "COKLAT") return "\033[38;5;94m";
    if (warna == "BiruMuda" || warna == "BIRU_MUDA") return "\033[38;5;117m";
    if (warna == "MerahMuda" || warna == "MERAH_MUDA" || warna == "Pink") return "\033[38;5;213m";
    if (warna == "Orange" || warna == "ORANGE") return "\033[38;5;208m";
    if (warna == "Merah" || warna == "MERAH") return "\033[31m";
    if (warna == "Kuning" || warna == "KUNING") return "\033[33m";
    if (warna == "Hijau" || warna == "HIJAU") return "\033[32m";
    if (warna == "BiruTua" || warna == "BIRU_TUA") return "\033[34m";
    if (warna == "ABU_ABU" || warna == "AbuAbu") return "\033[38;5;250m";
    return "\033[0m";
}

void BoardView::cetakPapan(Game& game) {
    BoardViewRenderer renderer(*game.getBoard(), game.getPemain(), game.getTurn(), game.getMaxTurn(), game.getCurrentPemainIndex());
    renderer.print();
}

void BoardView::cetakPapan(Board& board, const std::vector<User>& players) {
    BoardViewRenderer renderer(board, players);
    renderer.print();
}
