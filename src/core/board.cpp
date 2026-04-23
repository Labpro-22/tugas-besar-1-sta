#include "../../include/core/board.hpp"


Board::Board() {
    // Default: 40
    this->size = 40;
    // Isi papan dengan nullptr sementara agar tidak error saat di-loop
    for (int i = 0; i < size; ++i) {
        kotak.push_back(nullptr); 
    }
}

Board::Board(int size) : size(size) {
    // Isi papan dengan nullptr sementara agar tidak error saat di-loop
    for (int i = 0; i < size; ++i) {
        kotak.push_back(nullptr); 
    }
}

Board::~Board() {
    for (auto p : kotak) {
        delete p;
    }
    kotak.clear();
}


Petak* Board::getPetakAt(int index) const {
    if (index >= 0 && index < size) {
        return kotak[index];
    }
    return nullptr;
}

int Board::getSize() const { 
    return size;
}
int Board::getGoIndex() const { 
    for (auto it = kotak.begin(); it != kotak.end(); ++it) {
        if (*it != nullptr && (*it)->getName() == "go") {
            return static_cast<int>(it - kotak.begin());
        }
    }
    return -1;
}
int Board::getPenjaraIndex() const {
    // [!] HardCode
    for (auto it = kotak.begin(); it != kotak.end(); ++it) {
        if (*it != nullptr && (*it)->getName() == "penjara") {
            return static_cast<int>(it - kotak.begin());
        }
    }
    return -1;
}

void Board::setPetak(int index, Petak* petak){
    kotak.insert(kotak.begin()+index,petak);
    return;
}   