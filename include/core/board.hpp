#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "petak.hpp"

class Board {
private:
    std::vector<Petak*> kotak; // pointer ke semua petak (menggunakan Polymorphism)
    int size;

public:
    Board();
    Board(int size);
    ~Board();

    void buildBoard(); // Default : 40
    void buildBoard(const std::string& configFilename); // Dinamis : File Konfig

    Petak* getPetakAt(int index) const;

    int getSize() const;

    int getGoIndex() const;
    
    int getPenjaraIndex() const;
};

#endif