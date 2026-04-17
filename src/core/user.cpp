#include "user.hpp"
#include "properti.hpp"

User::User() {
    this->username = "Player";
    this->uang = 1500; // Modal awal standar monopoli
    this->koordinat = 0; // Mulai dari petak GO
    this->status = 0;
}

User::~User() {
    // Note: Properti tidak di-delete di sini karena User hanya menyimpan referensi (pointer), 
    // kepemilikan memori Properti biasanya diurus oleh Board/Game.
}

// Implementasi Pajak & Festival

int User::getUang() const { 
    return uang;
}
void User::kurangiUang(int jumlah) { 
    uang -= jumlah;
}
void User::tambahUang(int jumlah) { 
    uang += jumlah; 
}

int User::getTotalKekayaan() const {
    int total = this->uang; // Kekayaan dasar adalah uang tunai
    
    for (size_t i = 0; i < listProperti.size(); ++i) {
        if (listProperti[i] != nullptr) {
            // Tambahkan harga beli lahan (dan bisa dikembangkan untuk harga rumah nanti)
            total += listProperti[i]->getHargaBeli(); 
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
    return nullptr; // Kembalikan nullptr jika tidak ketemu
}

const std::vector<Properti*>& User::getListProperti() const { 
    return listProperti;
}

int User::getKoordinat() const { 
    return koordinat;
}