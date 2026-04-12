#ifndef DADU_HPP
#define DADU_HPP

class Dadu{
private:
    int angka;
public:
    Dadu();
    ~Dadu();
    void shuffle();
    int getAngka();
};

#endif