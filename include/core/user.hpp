#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <vector>
#include <string>

class User {
private:
    std::string username;
    int uang;
    int koordinat;                               // Kotak ke-?
    std::string status;                          // "ACTIVE", "BANKRUPT", atau "JAILED"
    
    // Atribut Khusus Efek Kartu
    int activeDiscount;                          
    bool shieldActive;                           
    
    std::vector<Properti*> listProperti;         
    std::vector<Kartu*> listKartu;               

public:
    User();
    ~User();

    std::string getUsername() const;
    void setUsername(const std::string& name);

    std::string getStatus() const;
    void setStatus(const std::string& newStatus);

    int getUang() const;
    void kurangiUang(int jumlah);
    void tambahUang(int jumlah);
    
    // Untuk PPH: Menghitung total kekayaan
    int getTotalKekayaan() const; 

    // Untuk Festival: Mencari properti berdasarkan kode
    Properti* getPropertiByKode(const std::string& kode);
    
    // Getter untuk listProperti
    const std::vector<Properti*>& getListProperti() const;

    std::vector<Street*> getStreetByColor(const std::string& warna) const;

    bool hasMonopoli(const std::string& warna, int totalDiPapan) const; 
    
    int getRailroadCount() const; 

    int getUtilityCount() const; 

    int getKoordinat() const;
    void setKoordinat(int index);
    void move(int langkah);      

    void setActiveDiscount(int discountPercentage);
    int getActiveDiscount() const;

    void setShieldActive(bool active);
    bool isShieldActive() const;
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