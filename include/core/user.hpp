#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include "properti.hpp"

class Kartu; 
class Street;
class RailRoad;
class Utility;

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
    User(const std::string& username, int uangAwal);
    ~User();

    void move(int dadu); // Bergerak sesuai jumlah dadu

    int getUang() const;

    std::string getUsername() const;

    int getKoordinat() const;

    void addProperti(Properti* p); // Tambahkan properti ke daftar milik pemain

    void removeProperti(Properti* p); // Hapus properti dari daftar milik pemain

    const std::vector<Properti*>& getListProperti() const;

    std::vector<Street*> getStreetByColor(const std::string& warna) const;

    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; 
    
    int getRailroadCount() const; //Menghitung jumalh Railraod yang dimiliki pemain (maks 4)

    int getUtilityCount() const; // Menghitung jumlah Utility yang dimiliki pemain (maks 2)

    User& operator+=(int jumlahUang);

    User& operator-=(int jumlahUang);

};

// Transaction Logger
class Logger {
private:
    std::vector<LogEntry> logs;
    
public:
    Logger();
    ~Logger();
    
    void addLog(int turn, const std::string& username, const std::string& jenisAksi, const std::string& detail);
    
    void cetakLogPenuh() const;
    
    void cetakLogTerbaru(int jumlah) const;
    
    const std::vector<LogEntry>& getLogs() const;
};

#endif