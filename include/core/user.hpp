#ifndef USER_HPP
#define USER_HPP

#include <vector>
#include <string>

class Properti;
class Street;
class Kartu;
class Logger;
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
    std::vector<Properti*> listProperti;         //Properti yang dipunya
    std::vector<Kartu*> listKartu;

public:
    User();
    ~User();

    // Pajak & Festival:
    int getUang() const;
    void kurangiUang(int jumlah);
    void tambahUang(int jumlah);
    
    // Untuk PPH: Menghitung total kekayaan
    int getTotalKekayaan() const; 

    // Untuk Festival: Mencari properti berdasarkan kode
    Properti* getPropertiByKode(const std::string& kode);
    
    // Getter untuk listProperti
    const std::vector<Properti*>& getListProperti() const;

    int getKoordinat() const;

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
    Logger() {}
    
    ~Logger() {}
};

#endif