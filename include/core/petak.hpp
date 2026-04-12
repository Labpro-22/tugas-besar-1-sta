#ifndef PETAK_HPP
#define PETAK_HPP

#include <iostream>
#include <vector>
#include "properti.hpp"

// Kelas Abstrak - Petak
class Petak {
private:
    int index;
    std::string kodePetak;
    std::string name;
    std::string kategori;
public:
    Petak();
    ~Petak();
    virtual std::string getType() = 0;
    virtual int getIndex();

};

// Inheritance Kelas Petak :: Kelas PetakProperti dan PetakAksi
class PetakProperti : public Petak{
private:
    std::string nama;
    std::string kode;
    float hargaBeli;
    std::vector<float> hargaSewa;
    int nilaiGadai;
    Properti* sertifikat;
public:
    virtual void beliLahan() = 0;
    virtual void hitungSewa() = 0;
};

class PetakAksi: public Petak{

};

// Inheritance PetakProperti: Petak Lahan, Petak Stasiun, Petak utilitas. 
class PetakLahan : public PetakProperti{
private:
    std::vector<float> hargaRumah;
    std::vector<float> hargaHotel;
    std::string warna;

public:
    PetakLahan();
    PetakLahan(std::string warna);
    ~PetakLahan();
    void beliLahan() override;
};

class PetakStasiun : public PetakProperti{
private:

public:
    PetakStasiun();
    ~PetakStasiun();
    void beliLahan() override;
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
    PetakFestival();
    ~PetakFestival();

    std::string getType() override;

    void onLanded(User* user, Game* game);

    void terapkanEfek(Properti* targetProperti);
};

class PetakPajak : public PetakAksi{
private:
    float pajakFlat;
public:
    PetakPajak();
    ~PetakPajak();
    virtual void bayarPajak(User& user) = 0;
};

class PetakPPH : public PetakPajak{
private:
    float pajakPercent;
public:
    PetakPPH();
    PetakPPH(float flat, float percent);
    ~PetakPPH();

    void bayarPajak(User& user) override;
};

class PetakPBM : public PetakPajak{
public:
    PetakPBM();
    PetakPBM(float flat);
    ~PetakPBM();

    void bayarPajak(User& user) override;
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