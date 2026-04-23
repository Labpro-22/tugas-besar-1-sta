#include "utils/dadu.hpp"
#include <cstdlib>
#include <ctime>

Dadu::Dadu(){
    static bool seeded = false;
    if (!seeded) {
        srand(time(NULL));
        seeded = true;
    }
    this->angka1 = 0;
    this->angka2 = 0;
}

Dadu::~Dadu(){}

void Dadu::shuffle(){
    this->angka1 = ((rand() % 6) + 1);
    this->angka2 = ((rand() % 6) + 1);
}

void Dadu::manual(int angka1,int angka2)  {
    this->angka1 = angka1;
    this->angka2 = angka2;
}

int Dadu::getAngka1(){
    return this->angka1;
}

int Dadu::getAngka2(){
    return this->angka2;
}

int Dadu::getTotal(){
    return this->angka1 + this->angka2;
}

bool Dadu::isDouble() const {
    return this->angka1 == this->angka2;
}
