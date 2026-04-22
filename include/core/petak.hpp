#ifndef PETAK_HPP
#define PETAK_HPP

#include <iostream>
#include <vector>
#include <string>
#include "properti.hpp"
#include "kartu.hpp"

class User;
class Game;

// [1] Kelas Abstrak - Petak
class Petak {
protected:
    int index;
    std::string kodePetak;
    std::string name;
    std::string kategori;
public:
    Petak();
    Petak(int index, std::string kodePetak, std::string name, std::string kategori);
    virtual ~Petak();

    
    virtual void onLanded(User* user, Game* game) = 0;

    int getIndex();
    std::string getKodePetak() const;
    std::string getName() const;
    std::string getKategori() const;
    void setIndex(int i);
};


// [2] Inheritance Kelas Petak :: Kelas PetakProperti
class PetakProperti : public Petak{
protected:
    Properti* sertifikat;
public:
    PetakProperti();
    PetakProperti(int index, std::string kodePetak, std::string name, std::string kategori,Properti* sertifikat);
    virtual ~PetakProperti();

    virtual void beliLahan() = 0;
    virtual void hitungSewa() = 0;
    virtual void onLanded(User* user, Game* game) = 0;

    Properti* getSertifikat() const;
};
// Inheritance PetakProperti: Petak Lahan, Petak Stasiun, Petak utilitas. 
// [2.1]
class PetakLahan : public PetakProperti{
private:
    std::vector<float> hargaRumah;
    std::vector<float> hargaHotel;
    std::string warna;

public:
    PetakLahan();
    PetakLahan(int index, std::string name, float hargaBeli,std::vector<float> hargaSewa,int nilaiGadai,Street* sertifikat,std::string warna);
    ~PetakLahan();

    std::string getWarna() const;
    std::string getOwnerName() const;
    std::vector<float> getHargaRumah() const;
    std::vector<float> getHargaHotel() const;

    void beliLahan() override;
    void hitungSewa() override;
    void onLanded(User* user, Game* game) override;
    void hancurkanBangunan();
};
// [2.2]
class PetakStasiun : public PetakProperti{
public:
    PetakStasiun();
    PetakStasiun(int index, std::string kodePetak, std::string name, std::string kategori,RailRoad* sertifikat);
    ~PetakStasiun();

    void beliLahan() override;
    void hitungSewa() override;
    void onLanded(User* user, Game* game) override;
};
// [2.3]
class PetakUtilitas : public PetakProperti{
private:

public:
    PetakUtilitas();
    ~PetakUtilitas();
    PetakUtilitas(int index, std::string kodePetak, std::string name, std::string kategori,Utility* sertifikat);

    void beliLahan() override;
    void hitungSewa() override;
    void onLanded(User* user, Game* game) override;
};


// [3] Inheritance Kelas Petak :: Kelas PetakAksi
class PetakAksi: public Petak{
private:
    
public:
    PetakAksi();
    PetakAksi(int index, std::string kodePetak, std::string name, std::string kategori);
    virtual ~PetakAksi();

    virtual void onLanded(User* user, Game* game) = 0;
};

// Inheritance PetakAksi: PetakKartu, PetakFestival, PetakPajak.
// [3.1]
template <class T>
class PetakKartu : public PetakAksi{
private:
    CardDeck<T> deck;
public:
    PetakKartu() : PetakAksi(){}
    PetakKartu(int index, std::string kodePetak, std::string name, std::string kategori)
    : PetakAksi(index,kodePetak,name,kategori), deck(){}
    ~PetakKartu();

    void onLanded(User* user, Game* game) override;
};

// [3.2]
class PetakFestival : public PetakAksi{
private:

public:
    PetakFestival();
    ~PetakFestival();

    void onLanded(User* user, Game* game) override;
    void terapkanEfek(Properti* targetProperti);
};
// [3.3]
class PetakPajak : public PetakAksi{
protected:
    float pajakFlat;
public:
    PetakPajak();
    virtual ~PetakPajak();

    virtual void bayarPajak(User& user) = 0;
};
// [3.3.1]
class PetakPPH : public PetakPajak{
private:
    float pajakPercent;
public:
    PetakPPH();
    PetakPPH(float flat, float percent);
    ~PetakPPH();

    void bayarPajak(User& user) override;
    void onLanded(User* user, Game* game) override;
};
// [3.3.2]
class PetakPBM : public PetakPajak{ 
private:   
    float pajakFlat;
public:
    PetakPBM();
    PetakPBM(float flat);
    ~PetakPBM();

    void bayarPajak(User& user) override;
    void onLanded(User* user, Game* game) override;
};


// [3.4] === PetakSpesial ===
class PetakSpesial : public PetakAksi{
private:
    
public:
    PetakSpesial();
    ~PetakSpesial();

    void onLanded(User* user, Game* game) override;
};

#endif