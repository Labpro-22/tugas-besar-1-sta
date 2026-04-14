#ifndef PETAK_HPP
#define PETAK_HPP

#include <iostream>
#include <vector>
#include <string>
#include "properti.hpp"

class User;
class Game;

// Kelas Abstrak - Petak
class Petak {
protected:
    int index;
    std::string kodePetak;
    std::string name;
    std::string kategori;
public:
    Petak();

    virtual ~Petak();

    virtual std::string getType() = 0;

    virtual int getIndex();

    virtual void onLanded(User* user, Game* game) = 0;

    std::string getKodePetak() const;

    std::string getName() const;

    void setIndex(int i);

};

// Inheritance Kelas Petak :: Kelas PetakProperti dan PetakAksi
class PetakProperti : public Petak{
protected:
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

    std::string getWarna() const;

    void beliLahan() override;

    void hitungSewa() override;
    
    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

class PetakStasiun : public PetakProperti{
private:

public:
    PetakStasiun();

    ~PetakStasiun();

    void beliLahan() override;

    void hitungSewa() override;

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

class PetakUtilitas : public PetakProperti{
private:

public:
    PetakUtilitas();

    ~PetakUtilitas();

    void beliLahan() override;

    void hitungSewa() override;

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

// Inheritance PetakAksi: PetakKartu, PetakFestival, PetakPajak, PetakSpesial.
class PetakKartu : public PetakAksi{
private:

public:
    PetakKartu();

    ~PetakKartu();

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

class PetakFestival : public PetakAksi{
private:

public:
    PetakFestival();

    ~PetakFestival();

    std::string getType() override;

    void onLanded(User* user, Game* game) override;

    void terapkanEfek(Properti* targetProperti);
};

class PetakPajak : public PetakAksi{
protected:
    float pajakFlat;
public:
    PetakPajak();

    virtual ~PetakPajak();

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

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

class PetakPBM : public PetakPajak{
public:
    PetakPBM();

    PetakPBM(float flat);

    ~PetakPBM();

    void bayarPajak(User& user) override;

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

class PetakSpesial : public PetakAksi{
private:
    std::string category; // Start, Penjara, Bebas Parkir, dan Petak going to penjara
public:
    PetakSpesial();

    ~PetakSpesial();

    std::string getCategory() const;

    std::string getType() override;

    void onLanded(User* user, Game* game) override;
};

#endif