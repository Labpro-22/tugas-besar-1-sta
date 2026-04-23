#ifndef BOARD_HPP
#define BOARD_HPP

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include "petak.hpp"

class Board {
private:
    std::vector<std::shared_ptr<Petak>> kotak; // shared ownership agar aman saat Board dicopy/dipindah
    int size;

public:
    Board();
    Board(int size);
    ~Board();

    Petak* getPetakAt(int index) const;
    int getSize() const;
    int getGoIndex() const;
    int getPenjaraIndex() const;

    void setPetak(int index, const std::shared_ptr<Petak>& petak);
};

#endif
