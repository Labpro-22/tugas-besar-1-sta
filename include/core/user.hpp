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
class KartuSpesial;
template <class T>
class CardDeck;

class User{
private:
    std::string username;
    int uang;
    int koordinat;                              // Kotak ke-?
    int status; // 1: Penjara, 2: Bangkrut, 3: Hidup
    int jailTurns;
    std::vector<Properti*> listProperti;         //Properti yang dipunya
    std::vector<KartuSpesial*> kartuSpesial;

    int activeDiscount = 0;
    bool shieldActive = false;
public:
    User();
    User(const std::string& username, int uangAwal);
    ~User();

    // Getter 
    int getUang() const;
    std::string getUsername() const;
    int getKoordinat() const;
    int getStatus() const;
    int getJailTurns() const;
    Properti* getPropertiByKode(const std::string& kode);
    const std::vector<Properti*>& getListProperti() const;
    std::vector<Street*> getStreetByColor(const std::string& warna) const;
    int getRailroadCount() const; //Menghitung jumalh Railraod yang dimiliki pemain (maks 4)
    int getUtilityCount() const; // Menghitung jumlah Utility yang dimiliki pemain (maks 2)
    int getTotalKekayaan() const; // Menghitung total kekayaan (Uang dan Kepunyaan)
    const std::vector<KartuSpesial*>& getKartuSpesial() const;
    int getActiveDiscount() const;

    // Boolean
    bool isJailed() const;
    bool isBankrupt() const;
    bool mustPayJailFine() const;
    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; 
    bool isShieldActive() const;
    
    
    // Overloading
    User& operator+=(int jumlahUang);
    User& operator-=(int jumlahUang);

    // Aksi - Pergerakan dan Properti
    void move(int dadu, Board* board); // Bergerak sesuai jumlah dadu
    void addProperti(Properti* p); // Tambahkan properti ke daftar milik pemain
    void removeProperti(Properti* p); // Hapus properti dari daftar milik pemain

    // Setters
    void setUsername(const std::string& name);
    void setUang(int value);
    void setStatus(const int newStatus);
    void setKoordinat(int index);
    void setJailTurns(int value);
    void setActiveDiscount(int discountPercentage);
    void setShieldActive(bool active);

    // Aksi - Kartu
    void resetEfekKartuSpesial();
    void addKartuSpesial(KartuSpesial* kartu, CardDeck<KartuSpesial>* deck);
    KartuSpesial* dropKartuSpesial();
    KartuSpesial* removeKartuSpesial(KartuSpesial* kartu);
    void setKartuSpesial(const std::vector<KartuSpesial*>& kartu);
    int hitungTagihanSetelahEfekKartu(int tagihan, bool pembayaranSewa);

    // Aksi - Penjara
    void sendToJail(int jailIndex);
    void releaseFromJail();
    void incrementJailTurns();
};

#endif
