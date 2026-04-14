#include "petak.hpp"
#include "user.hpp"
#include "game.hpp"

Petak::Petak() {}

Petak::~Petak() {}

int Petak::getIndex() { 
    return index;
}
std::string Petak::getKodePetak() const {
    return kodePetak;
}

std::string Petak::getName() const { 
    return name;
}

void Petak::setIndex(int i) { 
    index = i;
}

PetakPajak::PetakPajak() {}

PetakPajak::~PetakPajak() {}

// Implementasi Petak Pajak barang Mewah (PBM)
PetakPBM::PetakPBM(float flat) {
    name = "Pajak Barang Mewah";
    kodePetak = "PBM";
    kategori = "Pajak";
    pajakFlat = flat;
}

PetakPBM::~PetakPBM() {}

std::string PetakPBM::getType() { return "Pajak"; }

void PetakPBM::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Mendarat di Pajak Barang Mewah (PBM)\n";
    std::cout << "[INFO] Potongan pajak flat: M" << pajakFlat << "\n";
    bayarPajak(*user);
}

void PetakPBM::bayarPajak(User& user) {
    if (user.getUang() >= pajakFlat) {
        user.kurangiUang(pajakFlat);
        std::cout << "[SUCCESS] Pajak terbayar. Sisa uang: M" << user.getUang() << "\n";
    } else {
        std::cout << "[WARNING] Saldo tidak mencukupi untuk membayar pajak!\n";
        // TODO: Implementasi trigger kebangkrutan
    }
}

// Implementasi Petak Pajak Penghasilan (PPH)
PetakPPH::PetakPPH(float flat, float percent) {
    name = "Pajak Penghasilan";
    kodePetak = "PPH";
    kategori = "Pajak";
    pajakFlat = flat;
    pajakPercent = percent;
}

PetakPPH::~PetakPPH() {}

std::string PetakPPH::getType() { return "Pajak"; }

void PetakPPH::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Mendarat di Pajak Penghasilan (PPH)\n";
    bayarPajak(*user);
}

void PetakPPH::bayarPajak(User& user) {
    int pilihan;
    std::cout << "Opsi Pembayaran PPH:\n"
              << "1. Bayar flat (M" << pajakFlat << ")\n"
              << "2. Bayar " << pajakPercent << "% dari total aset/kekayaan\n"
              << "> Pilihan (1/2): ";
    std::cin >> pilihan;

    if (pilihan == 1) {
        if (user.getUang() >= pajakFlat) {
            user.kurangiUang(pajakFlat);
            std::cout << "[SUCCESS] Pajak flat terbayar. Sisa uang: M" << user.getUang() << "\n";
        } else {
            std::cout << "[WARNING] Saldo tidak mencukupi!\n";
            // TODO: Implementasi trigger kebangkrutan
        }
    } 
    else if (pilihan == 2) {
        float totalKekayaan = user.getTotalKekayaan(); 
        float pajakPersentase = totalKekayaan * (pajakPercent / 100.0f);
        
        std::cout << "[INFO] Total aset: M" << totalKekayaan << " | Potongan: M" << pajakPersentase << "\n";

        if (user.getUang() >= pajakPersentase) {
            user.kurangiUang(pajakPersentase);
            std::cout << "[SUCCESS] Pajak persentase terbayar. Sisa uang: M" << user.getUang() << "\n";
        } else {
            std::cout << "[WARNING] Saldo tidak mencukupi!\n";
            // TODO: Implementasi trigger kebangkrutan
        }
    }
}

// Implementasi Petak Festival
PetakFestival::PetakFestival() {
    name = "Festival";
    kodePetak = "FES";
    kategori = "Aksi";
}

PetakFestival::~PetakFestival() {}

std::string PetakFestival::getType() { return "Festival"; }

void PetakFestival::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Mendarat di petak Festival!\n";
    
    const auto& props = user->getListProperti();
    if (props.empty()) {
        std::cout << "[INFO] Belum ada properti yang dapat difestivalkan.\n";
        return;
    }

    std::cout << "Daftar Properti Tersedia:\n";
    for (const auto& p : props) {
        std::cout << "- [" << p->getKode() << "] " << p->getNama() << "\n";
    }

    std::string inputKode;
    std::cout << "> Masukkan kode properti: ";
    std::cin >> inputKode;

    Properti* target = user->getPropertiByKode(inputKode);
    if (target) {
        terapkanEfek(target);
    } else {
        std::cout << "[ERROR] Kode properti invalid atau bukan hak milik.\n";
    }
}

void PetakFestival::terapkanEfek(Properti* targetProperti) {
    int currentMult = targetProperti->getFestivalMultiplier();
    
    if (currentMult < 8) {
        targetProperti->setFestivalMultiplier(currentMult * 2);
        std::cout << "[SUCCESS] Efek festival diaktifkan/diperkuat!\n";
    } else {
        std::cout << "[INFO] Multiplier festival telah mencapai batas maksimum (8x).\n";
    }
    
    targetProperti->setFestivalDuration(3);
    std::cout << "[INFO] Nilai sewa saat ini menjadi: M" << targetProperti->getSewaSaatIni(0) 
              << " (Aktif 3 giliran)\n";
}

// HAPUS AJA KALO UDAH GA DIPAKE, INI CUMA BUAT NGE-TEST CETAK PALETNYA AJA, KARENA BELUM SEMUA IMPLEMENTASI FUNGSI-FUNGSINYA,
// JADI BIAR GA ERROR DULU. ATAU MASUKIN KE IMPLEMENTASI KELAS LAIN YANG BUTUH, 
// KARENA SEMUA KELAS INI TURUNAN DARI PETAK, JADI BISA DIPAKE SEMUA TEMPAT YANG BUTUH OBJEK PETAK

PetakLahan::PetakLahan() {}
PetakLahan::PetakLahan(std::string warna) { 
    this->warna = warna; 
    kategori = "Lahan"; 
    kodePetak = "LHN"; 
}
PetakLahan::~PetakLahan() {}
void PetakLahan::beliLahan() {}
void PetakLahan::hitungSewa() {}
std::string PetakLahan::getWarna() const { return warna;}
std::string PetakLahan::getType() { return "Lahan"; }
void PetakLahan::onLanded(User* user, Game* game) {}

PetakStasiun::PetakStasiun() { kategori = "Stasiun"; kodePetak = "STA"; }
PetakStasiun::~PetakStasiun() {}
void PetakStasiun::beliLahan() {}
void PetakStasiun::hitungSewa() {}
std::string PetakStasiun::getType() { return "Stasiun"; }
void PetakStasiun::onLanded(User* user, Game* game) {}

PetakUtilitas::PetakUtilitas() { kategori = "Utilitas"; kodePetak = "UTL"; }
PetakUtilitas::~PetakUtilitas() {}
void PetakUtilitas::beliLahan() {}
void PetakUtilitas::hitungSewa() {}
std::string PetakUtilitas::getType() { return "Utilitas"; }
void PetakUtilitas::onLanded(User* user, Game* game) {}

PetakKartu::PetakKartu() { kategori = "Kartu"; kodePetak = "KRT"; }
PetakKartu::~PetakKartu() {}
std::string PetakKartu::getType() { return "Kartu"; }
void PetakKartu::onLanded(User* user, Game* game) {}

PetakSpesial::PetakSpesial() { kategori = "Spesial"; kodePetak = "SPS"; }
PetakSpesial::~PetakSpesial() {}
std::string PetakSpesial::getCategory() const { return category; }
std::string PetakSpesial::getType() { return "Spesial"; }
void PetakSpesial::onLanded(User* user, Game* game) {}