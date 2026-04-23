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
    std::string warna;
public:
    Petak();
    Petak(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    virtual ~Petak();

    
    virtual void onLanded(User* user, Game* game) = 0;

    int getIndex();
    std::string getKodePetak() const;
    std::string getName() const;
    std::string getKategori() const;
    std::string getWarna() const;
    void setIndex(int i);
    void setWarna(const std::string& warna);
};


// [2] Inheritance Kelas Petak :: Kelas PetakProperti
class PetakProperti : public Petak{
protected:
    Properti* sertifikat;

public:
    PetakProperti();
    PetakProperti(int index, std::string kodePetak, std::string name, std::string kategori, Properti* sertifikat, std::string warna);
    virtual ~PetakProperti();

    virtual void onLanded(User* user, Game* game) = 0;

    Properti* getSertifikat() const;
};
// Inheritance PetakProperti: Petak Lahan, Petak Stasiun, Petak utilitas. 
// [2.1] PetakLahan :: Street
class PetakLahan : public PetakProperti{
private:
    void beliLahan(User* user);
    void bayarSewa(User* user);
public:
    PetakLahan();
    PetakLahan(int index, std::string kodePetak, std::string name, std::string kategori, Street* sertifikat, std::string warna);
    ~PetakLahan();

    std::string getOwnerName() const;

    void onLanded(User* user, Game* game) override;
    void hancurkanBangunan(); // Semua Dihancurkan (Oleh bank)
    void hancurkanSatuBangunan(); // Oleh Pemain (Card)
};
// [2.2]
class PetakStasiun : public PetakProperti{
private:
    void bayarSewa(User* user);
public:
    PetakStasiun();
    PetakStasiun(int index, std::string kodePetak, std::string name, std::string kategori, RailRoad* sertifikat, std::string warna);
    ~PetakStasiun();

    void onLanded(User* user, Game* game) override;
};
// [2.3]
class PetakUtilitas : public PetakProperti{
private:
    void bayarSewa(User* user, Game* game);
public:
    PetakUtilitas();
    ~PetakUtilitas();
    PetakUtilitas(int index, std::string kodePetak, std::string name, std::string kategori, Utility* sertifikat, std::string warna);

    void onLanded(User* user, Game* game) override;
};


// [3] Inheritance Kelas Petak :: Kelas PetakAksi
class PetakAksi: public Petak{
private:
    
public:
    PetakAksi();
    PetakAksi(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
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
    PetakKartu(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index,kodePetak,name,kategori,warna), deck(){}
    ~PetakKartu();

    void onLanded(User* user, Game* game) override;
};

// [3.2]
class PetakFestival : public PetakAksi{
private:
    void terapkanEfek(Properti* targetProperti);
public:
    PetakFestival();
    PetakFestival(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    ~PetakFestival();

    void onLanded(User* user, Game* game) override;
};

// [3.3]
class PetakPajak : public PetakAksi{
private:
    virtual void bayarPajak(User& user) = 0;
public:
    PetakPajak();
    PetakPajak(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    virtual ~PetakPajak();

    
};
// [3.3.1]
class PetakPPH : public PetakPajak{
private:
    float pajakFlat;
    float pajakPercent;
    
    void bayarPajak(User& user) override;
public:
    PetakPPH();
    PetakPPH(float flat, float percent);
    PetakPPH(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, float flat, float percent);
    ~PetakPPH();

    void onLanded(User* user, Game* game) override;
};
// [3.3.2]
class PetakPBM : public PetakPajak{ 
private:   
    float pajakFlat;

    void bayarPajak(User& user) override;
public:
    PetakPBM();
    PetakPBM(float flat);
    PetakPBM(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, float flat);
    ~PetakPBM();

    void onLanded(User* user, Game* game) override;
};


// [3.4] === PetakSpesial ===
class PetakSpesial : public PetakAksi{
private:
    
public:
    PetakSpesial();
    PetakSpesial(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    virtual ~PetakSpesial();

    virtual void onLanded(User* user, Game* game) = 0;
};

// [3.4.1] === PetakGo ===
class PetakGo : public PetakSpesial{
private:
    int earnMoney;
public:
    PetakGo();
    PetakGo(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, int earnMoney);
    ~PetakGo();

    void onLanded(User* user, Game* game) override;
};

// [3.4.2] === PetakPenjara ===
class PetakPenjara : public PetakSpesial{
private:
    int denda;
public:
    PetakPenjara();
    PetakPenjara(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, int denda);
    ~PetakPenjara();

    void onLanded(User* user, Game* game) override;
};

// [3.4.3] === PetakBebasParkir ===
class PetakBebasParkir : public PetakSpesial{
public:    
    PetakBebasParkir();
    PetakBebasParkir(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    ~PetakBebasParkir();

    void onLanded(User* user, Game* game) override;
};

// [3.4.4] === PetakPergiPenjara ===
class PetakPergiPenjara : public PetakSpesial{
public:
    PetakPergiPenjara();
    PetakPergiPenjara(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna);
    ~PetakPergiPenjara();

    void onLanded(User* user, Game* game) override;
};
#endif
