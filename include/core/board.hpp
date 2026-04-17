#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <string>
#include "petak.hpp"

class Board {
private:
    std::vector<Petak*> kotak; // pointer ke semua petak (menggunakan Polymorphism)
    int size;

public:
    Board();

    ~Board();

    void buildBoard(const std::string& configFilename); 

    Petak* getPetakAt(int index) const;

    int getSize() const;

    int getGoIndex() const;
    
    int getPenjaraIndex() const;
};

#endif