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

    virtual void onLanded(User& user) = 0;
    virtual std::string getType() = 0;
    virtual int getIndex();

};

// Inheritance Kelas Petak :: Kelas PetakProperti
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
    virtual void hitungSewa(User& user) = 0;
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
    PetakUtilitas();
    ~PetakUtilitas();

    void onLanded(User& user) override;
    void hitungSewa(User& user) override;
    std::string getType() override;
    
};

// Inheritance Petak :: Kelas PetakAksi
class PetakAksi: public Petak{
public:
    PetakAksi();
    ~PetakAksi();
};

// Inheritance PetakAksi: PetakKartu, PetakFestival, PetakPajak.
class PetakKartu : public PetakAksi{
private:

public:
    PetakKartu();
    ~PetakKartu();

    void onLanded(User& user) override;
    std::string getType() override;
};

class PetakFestival : public PetakAksi{
private:

public:
    PetakFestival();
    ~PetakFestival();

    std::string getType() override;
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

class PetakPBM : public PetakPajak{    float pajakFlat;
public:
    PetakPBM();
    ~PetakPBM();
    virtual void bayarPajak(User& user) = 0;
};

class PetakSpesial : public Petak{
private:
    
public:
    PetakSpesial();
    ~PetakSpesial();

    virtual void onLanded(User& user);
    std::string getType() override;
};

class PetakGO : public PetakSpesial{
public:
    void onLanded(User& user) override;
};

class PetakPenjara : public PetakSpesial{
public:
    void onLanded(User& user) override;
};

class PetakBebasParkir : public PetakSpesial{
public:
    void onLanded(User& user) override;
};

class PetakPergiPenjara : public PetakSpesial{
public:
    void onLanded(User& user) override;
};

#endif