#include "../../include/core/petak.hpp"
#include "../../include/core/user.hpp"
#include "../../include/core/game.hpp"

// [1] Abstract Class : Petak
Petak::Petak() {}
Petak::Petak(int index, std::string kodePetak, std::string name, std::string kategori)
    :index(index),kodePetak(kodePetak),name(name),kategori(kategori){}
Petak::~Petak() {}

int Petak::getIndex() { return index;}
std::string Petak::getKodePetak() const {return kodePetak;}
std::string Petak::getName() const { return name;}
std::string Petak::getKategori() const {return kategori;}
void Petak::setIndex(int i) { index = i;}



// [2] Abstract Class: PetakProperti {Inheritance dari Petak}
PetakProperti::PetakProperti(){}
PetakProperti::PetakProperti(int index, std::string kodePetak, std::string name, std::string kategori,float hargaBeli,std::vector<float> hargaSewa,int nilaiGadai,Properti* sertifikat)
    :Petak(index,kodePetak,name,kategori),hargaBeli(hargaBeli),hargaSewa(hargaSewa),nilaiGadai(nilaiGadai),sertifikat(sertifikat){}
PetakProperti::~PetakProperti(){}

float PetakProperti::getHargaBeli() const{
    return hargaBeli;
}
int PetakProperti::getNilaiGadai() const{
    return nilaiGadai;
}
std::vector<float> PetakProperti::getHargaSewa() const{
    return hargaSewa;
}
Properti* PetakProperti::getSertifikat() const{
    return sertifikat;
}


// [2.1] Class PetakLahan (Inheritance dari PetakProperti)
PetakLahan::PetakLahan() {kategori="Lahan";kodePetak="LHN";}
PetakLahan::PetakLahan(int index, std::string name, float hargaBeli,std::vector<float> hargaSewa,int nilaiGadai,Properti* sertifikat,std::string warna) 
    :PetakProperti(index,"LHN",name,"Lahan",hargaBeli,hargaSewa,nilaiGadai,sertifikat),warna(warna){} 
PetakLahan::PetakLahan(std::string warna) : PetakLahan(){this->warna=warna;}
PetakLahan::~PetakLahan() {}

std::string PetakLahan::getWarna() const { return warna;}
std::string PetakLahan::getOwnerName() const {
    if (sertifikat != nullptr && sertifikat->getOwner() != nullptr) {
        return sertifikat->getOwner()->getUsername(); 
    }
    return "";
}

void PetakLahan::beliLahan() {}
void PetakLahan::hitungSewa() {}
void PetakLahan::onLanded(User* user, Game* game) {}

void PetakLahan::hancurkanBangunan() {
    Street* jalan = dynamic_cast<Street*>(sertifikat);
    if (jalan != nullptr) {
        jalan->hancurkanSatuTingkatBangunan();
    } else {
        std::cout << "[ERROR] Petak ini bukan jenis Street, tidak memiliki bangunan.\n";
    }
}

// [2.2] Class PetakStasiun (Inheritance dari PetakProperti)
PetakStasiun::PetakStasiun() { kategori = "Stasiun"; kodePetak = "STA"; }
PetakStasiun::~PetakStasiun() {}

void PetakStasiun::beliLahan() {}
void PetakStasiun::hitungSewa() {}
void PetakStasiun::onLanded(User* user, Game* game) {}

// [2.3] Class PetakUtilitas (Inheritance dari PetakProperti)
PetakUtilitas::PetakUtilitas() { kategori = "Utilitas"; kodePetak = "UTL"; }
PetakUtilitas::~PetakUtilitas() {}

void PetakUtilitas::beliLahan() {}
void PetakUtilitas::hitungSewa() {}
void PetakUtilitas::onLanded(User* user, Game* game) {}




// [3] Abstract Class : Class PetakAksi {Inheritance dari Petak}


// [3.1] Class PetakKartu {Inheritance dari PetakAksi}
PetakKartu::PetakKartu() { kategori = "Kartu"; kodePetak = "KRT"; }
PetakKartu::~PetakKartu() {}
void PetakKartu::onLanded(User* user, Game* game) {}

// [3.2] Class PetakFestival {Inheritance dari PetakAksi}
PetakFestival::PetakFestival() {
    name = "Festival";
    kodePetak = "FES";
    kategori = "Aksi";
}
PetakFestival::~PetakFestival() {}

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

// [3.3] Class PetakPajak {Inheritance dari PetakAksi}
PetakPajak::PetakPajak() {}
PetakPajak::~PetakPajak() {}
// [3.3.1] Class PetakPPH {Inheritance dari PetakPajak}
PetakPPH::PetakPPH(float flat, float percent) {
    name = "Pajak Penghasilan";
    kodePetak = "PPH";
    kategori = "Pajak";
    pajakFlat = flat;
    pajakPercent = percent;
}
PetakPPH::~PetakPPH() {}

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
            user -= (pajakFlat);
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
            user -= (pajakPersentase);
            std::cout << "[SUCCESS] Pajak persentase terbayar. Sisa uang: M" << user.getUang() << "\n";
        } else {
            std::cout << "[WARNING] Saldo tidak mencukupi!\n";
            // TODO: Implementasi trigger kebangkrutan
        }
    }
}
// [3.3.2] Class PetakPBM {Inheritance dari PetakPajak}
PetakPBM::PetakPBM(float flat) {
    name = "Pajak Barang Mewah";
    kodePetak = "PBM";
    kategori = "Pajak";
    pajakFlat = flat;
}
PetakPBM::~PetakPBM() {}

void PetakPBM::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Mendarat di Pajak Barang Mewah (PBM)\n";
    std::cout << "[INFO] Potongan pajak flat: M" << pajakFlat << "\n";
    bayarPajak(*user);
}
void PetakPBM::bayarPajak(User& user) {
    if (user.getUang() >= pajakFlat) {
        user -= (pajakFlat);
        std::cout << "[SUCCESS] Pajak terbayar. Sisa uang: M" << user.getUang() << "\n";
    } else {
        std::cout << "[WARNING] Saldo tidak mencukupi untuk membayar pajak!\n";
        // TODO: Implementasi trigger kebangkrutan
    }
}



// [4] ===PetakSpesial===
PetakSpesial::PetakSpesial() { kategori = "Spesial"; kodePetak = "SPS"; }
PetakSpesial::~PetakSpesial() {}
void PetakSpesial::onLanded(User* user, Game* game) {}