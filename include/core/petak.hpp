#ifndef PETAK_HPP
#define PETAK_HPP

#include <iostream>

// Kelas Abstrak - Petak
class Petak {
private:
    int index;
    std::string kodePetak;
public:
    Petak();
    ~Petak();
    virtual std::string getType() = 0;
    virtual int getIndex();

};

// Inheritance Kelas Petak :: Kelas PetakProperti dan PetakAksi
class PetakProperti : public Petak{
private:

public:

};

class PetakAksi: public Petak{

};

// Inheritance PetakProperti: Petak Lahan, Petak Stasiun, Petak utilitas. 
class PetakLahan : public PetakProperti{
private:
    std::string warna;
public:
    PetakLahan();
    PetakLahan(std::string warna);
    ~PetakLahan();
};

class PetakStasiun : public PetakProperti{
private:

public:

};

class PetakUtilitas : public PetakProperti{
private:

public:

};

// Inheritance PetakAksi: PetakKartu, PetakFestival, PetakPajak, PetakSpesial.
class PetakKartu : public PetakAksi{
private:

public:

};

class PetakFestival : public PetakAksi{
private:

public:

};

class PetakPajak : public PetakAksi{
private:

public:

};

class PetakSpesial : public PetakAksi{
private:
    std::string category; // Start, Penjara, Bebas Parkir, dan Petak going to penjara
public:
    PetakSpesial();
    ~PetakSpesial();

    std::string getCategory() const;
    
};







#endif