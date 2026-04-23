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
    void manual(int angka1,int angka2);
    int getAngka1();
    int getAngka2();
    int getTotal();
    void reset();
    bool isDouble() const;
};

#endif