#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "properti.hpp"
#include "gameException.hpp"


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

public:
    User();
    User(const std::string& username, int uangAwal);
    ~User();


    int getUang() const;
    std::string getUsername() const;
    int getKoordinat() const;
    int getStatus() const;
    const std::vector<Properti*>& getListProperti() const;
    std::vector<Street*> getStreetByColor(const std::string& warna) const;
    int getRailroadCount() const; //Menghitung jumalh Railraod yang dimiliki pemain (maks 4)
    int getUtilityCount() const; // Menghitung jumlah Utility yang dimiliki pemain (maks 2)
    int getTotalKekayaan() const; // Menghitung total kekayaan (Uang dan Kepunyaan)

    void move(int dadu); // Bergerak sesuai jumlah dadu
    void addProperti(Properti* p); // Tambahkan properti ke daftar milik pemain
    void removeProperti(Properti* p); // Hapus properti dari daftar milik pemain
    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; 
    
    User& operator+=(int jumlahUang);
    User& operator-=(int jumlahUang);

    void User::setUsername(const std::string& name);
    void User::setStatus(const int newStatus);
    void User::setKoordinat(int index);


};

struct LogEntry {
    int turn;
    std::string username;
    std::string jenisAksi;
    std::string detail;
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