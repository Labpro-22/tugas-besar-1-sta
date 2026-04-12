#ifndef PROPERTI_HPP
#define PROPERTI_HPP

#include "petak.hpp"
#include "user.hpp"

class Properti{
private:
    std::string status;
    User& owner;
    int jumlahBangunan;
    int durasiFestival;
    float festivalMultiplier;

public:
    Properti();
    Properti(User& owner);
    ~Properti();

    virtual void beli() = 0;
};

#endif