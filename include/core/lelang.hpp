#pragma once

#include "user.hpp"
#include "petak.hpp"

class Lelang{
private:
    int koordinat; // Lokasi Properti
    int bid; // Nilai Bid
    int round;
    int continuesPass;
public:
    Lelang(int koordinat);
    ~Lelang();
    void pass(User& User);
    void bid(User& User);
    void isEnd();
};