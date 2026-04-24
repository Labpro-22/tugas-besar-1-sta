#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "properti.hpp"
#include "utils/gameException.hpp"


class Street;
class RailRoad;
class Utility;
class Board;

class User{
private:
    std::string username;
    int uang;
    int koordinat;                              // Kotak ke-?
    int status; // 1: Penjara, 2: Bangkrut, 3: Hidup
    int jailTurns;
    std::vector<Properti*> listProperti;         //Properti yang dipunya

    int activeDiscount = 0;
    bool shieldActive = false;
public:
    User();
    User(const std::string& username, int uangAwal);
    ~User();


    int getUang() const;
    std::string getUsername() const;
    int getKoordinat() const;
    int getStatus() const;
    int getJailTurns() const;
    bool isJailed() const;
    bool isBankrupt() const;
    bool mustPayJailFine() const;
    Properti* getPropertiByKode(const std::string& kode);
    const std::vector<Properti*>& getListProperti() const;
    std::vector<Street*> getStreetByColor(const std::string& warna) const;
    int getRailroadCount() const; //Menghitung jumalh Railraod yang dimiliki pemain (maks 4)
    int getUtilityCount() const; // Menghitung jumlah Utility yang dimiliki pemain (maks 2)
    int getTotalKekayaan() const; // Menghitung total kekayaan (Uang dan Kepunyaan)

    void move(int dadu, Board* board); // Bergerak sesuai jumlah dadu
    void addProperti(Properti* p); // Tambahkan properti ke daftar milik pemain
    void removeProperti(Properti* p); // Hapus properti dari daftar milik pemain
    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; 
    
    User& operator+=(int jumlahUang);
    User& operator-=(int jumlahUang);

    void setUsername(const std::string& name);
    void setStatus(const int newStatus);
    void setKoordinat(int index);
    void sendToJail(int jailIndex);
    void releaseFromJail();
    void incrementJailTurns();

    void setActiveDiscount(int discountPercentage);
    int getActiveDiscount() const;
    void setShieldActive(bool active);
    bool isShieldActive() const;

};

class LogEntry {
private:
    int turn;
    std::string username;
    std::string jenisAksi;
    std::string detail;

public:
    LogEntry(int turn, std::string username, std::string jenisAksi, std::string detail);
    ~LogEntry() = default;

    int getTurn() const;
    std::string getUsername() const;
    std::string getJenisAksi() const;
    std::string getDetail() const;
};

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
