#include "user.hpp"
#include "properti.hpp"
#include <iostream>

User::User() {
    this->username = "Player";
    this->uang = 1500; // Modal awal standar monopoli
    this->koordinat = 0; // Mulai dari petak GO
    this->status = "ACTIVE"; // Menggunakan string sesuai format save/load
    this->activeDiscount = 0;
    this->shieldActive = false;
}

User::~User() {
    // Properti diurus oleh Board/Game
}

std::string User::getUsername() const {
    return username;
}

void User::setUsername(const std::string& name) {
    this->username = name;
}

std::string User::getStatus() const {
    return status;
}

void User::setStatus(const std::string& newStatus) {
    this->status = newStatus;
}

int User::getUang() const { 
    return uang;
}

void User::kurangiUang(int jumlah) { 
    uang -= jumlah;
}

void User::tambahUang(int jumlah) { 
    uang += jumlah; 
}

int User::getKoordinat() const { 
    return koordinat;
}

void User::setKoordinat(int index) {
    this->koordinat = index;
}

void User::move(int langkah) {
    int oldPos = this->koordinat;
    this->koordinat += langkah;

    if (this->koordinat < 0) {
        this->koordinat += 40; // Asumsi papan fix 40
    }
    else if (this->koordinat >= 40) {
        this->koordinat %= 40;
        if (langkah > 0) {
            std::cout << username << " melewati GO! Mendapatkan gaji M200.\n";
            tambahUang(200);
        }
    }
}

int User::getTotalKekayaan() const {
    int total = this->uang; // Kekayaan dasar adalah uang tunai
    
    for (size_t i = 0; i < listProperti.size(); ++i) {
        if (listProperti[i] != nullptr) {
            total += listProperti[i]->getHargaBeli(); 
            // TODO: Tambahkan juga total harga beli bangunan di atas properti ini
            // total += listProperti[i]->getTotalHargaBangunan();
        }
    }
    
    return total;
}

Properti* User::getPropertiByKode(const std::string& kode) {
    for (size_t i = 0; i < listProperti.size(); ++i) {
        if (listProperti[i] != nullptr && listProperti[i]->getKode() == kode) {
            return listProperti[i];
        }
    }
    return nullptr;
}

const std::vector<Properti*>& User::getListProperti() const { 
    return listProperti;
}

void User::setActiveDiscount(int discountPercentage) {
    this->activeDiscount = discountPercentage;
}

int User::getActiveDiscount() const {
    return activeDiscount;
}

void User::setShieldActive(bool active) {
    this->shieldActive = active;
}

bool User::isShieldActive() const {
    return shieldActive;
}

Logger::Logger() {}

Logger::~Logger() {}

void Logger::addLog(int turn, const std::string& username, const std::string& jenisAksi, const std::string& detail) {
    LogEntry entry = {turn, username, jenisAksi, detail};
    logs.push_back(entry);
}

void Logger::cetakLogPenuh() const {
    std::cout << "=== Log Transaksi Penuh ===\n";
    for (size_t i = 0; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.turn << "] " << log.username 
                  << " | " << log.jenisAksi << " | " << log.detail << "\n";
    }
}

void Logger::cetakLogTerbaru(int jumlah) const {
    std::cout << "=== Log Transaksi (" << jumlah << " Terakhir) ===\n";
    
    size_t start = 0;
    if (logs.size() > static_cast<size_t>(jumlah)) {
        start = logs.size() - jumlah;
    }
    
    for (size_t i = start; i < logs.size(); ++i) {
        const auto& log = logs[i];
        std::cout << "[Turn " << log.turn << "] " << log.username 
                  << " | " << log.jenisAksi << " | " << log.detail << "\n";
    }
}

const std::vector<LogEntry>& Logger::getLogs() const {
    return logs;
}