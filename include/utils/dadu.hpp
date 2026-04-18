#ifndef DADU_HPP
#define DADU_HPP

class Dadu{
private:
    int angka1;
    int angka2;
    
public:
    Dadu();
    ~Dadu();
    void shuffle();
    int getAngka1();
    int getAngka2();
    int getTotal();
    bool isDouble() const;
};

#endif