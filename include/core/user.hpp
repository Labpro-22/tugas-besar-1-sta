#ifndef USER_HPP
#define USER_HPP

#include "petak.hpp"
#include "properti.hpp"
#include "kartu.hpp"
#include <vector>
// Rancangan Awal [Bisa berubah ya nanti!]
// 1. Vector: Simpan state user dan simpan data di class lain.
// 2. Map : Hubungin user dan data di class lain
// 3. Map :: user -> data lain (Vector)




class User{
private:
    std::string username;
    int uang;
    int koordinat;                              // Kotak ke-?
    int status; // Penjara, ...
    std::vector<Properti> listProperti;         //Properti yang dipunya
    std::vector<Kartu> listKartu;

public:
    User();
    ~User();

    void move(int dadu); // Bergerak sesuai jumlah dadu

    std::vector<Street*> getStreetByColor(const std::string& warna) const;

    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; //Mengecek apakah pemain memonopoli satu color group
    
    int getRailroadCount() const; //Menghitung jumalh Railraod yang dimiliki pemain (maks 4)

    int getUtilityCount() const; // Menghitung jumlah Utility yang dimiliki pemain (maks 2)

};

// Transcation Logger
class Logger{
private:
    
public:
};

#endif