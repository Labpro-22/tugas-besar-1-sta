#include "../../include/core/petak.hpp"
#include "../../include/core/user.hpp"
#include "../../include/core/game.hpp"

// [1] Abstract Class : Petak
Petak::Petak() : index(0), kodePetak(""), name(""), kategori(""), warna("") {}
Petak::Petak(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    :index(index),kodePetak(kodePetak),name(name),kategori(kategori),warna(warna){}
Petak::~Petak() {}

int Petak::getIndex() { return index;}
std::string Petak::getKodePetak() const {return kodePetak;}
std::string Petak::getName() const { return name;}
std::string Petak::getKategori() const {return kategori;}
std::string Petak::getWarna() const {return warna;}
void Petak::setIndex(int i) { index = i;}
void Petak::setWarna(const std::string& warnaBaru) { warna = warnaBaru;}



// [2] Abstract Class: PetakProperti {Inheritance dari Petak}
PetakProperti::PetakProperti(){}
PetakProperti::PetakProperti(int index, std::string kodePetak, std::string name, std::string kategori, Properti* sertifikat, std::string warna)
    :Petak(index,kodePetak,name,kategori,warna),sertifikat(sertifikat){
    if (this->warna.empty() && sertifikat != nullptr) {
        this->warna = sertifikat->getWarna();
    }
}
PetakProperti::~PetakProperti(){}
Properti* PetakProperti::getSertifikat() const{
    return sertifikat;
}


// [2.1] Class PetakLahan (Inheritance dari PetakProperti)
PetakLahan::PetakLahan() {kategori="Lahan";kodePetak="LHN";}
PetakLahan::PetakLahan(int index, std::string kodePetak, std::string name, std::string kategori, Street* sertifikat, std::string warna)
    :PetakProperti(index, kodePetak, name, kategori, sertifikat, warna){}
PetakLahan::~PetakLahan() {}

std::string PetakLahan::getOwnerName() const {
    if (sertifikat != nullptr && sertifikat->getOwner() != nullptr) {
        return sertifikat->getOwner()->getUsername(); 
    }
    return "";
}

void PetakLahan::beliLahan(User* user) {
    if (user->getUang() < sertifikat->getHargaBeli()) {
        throw UangTidakCukupException();
    }

    *(user) -= sertifikat->getHargaBeli();
    sertifikat->setOwner(user);
    sertifikat->setStatus(PropStatus::OWNED);
}
void PetakLahan::bayarSewa(User* user) {
    if (sertifikat->getOwner() == nullptr) {
        throw BukanPemilikException();
    }
    int biayaSewa = sertifikat->hitungSewa(0); 
    // [!] Monopoly Color Group
   
   
    if (user->getUang() < biayaSewa) {
        throw UangTidakCukupException();
    }
    *(user) -= biayaSewa;
    *(sertifikat->getOwner()) += biayaSewa;
}
void PetakLahan::onLanded(User* user, Game* game) {
    if (sertifikat->getStatus()==PropStatus::BANK){
        // Print Akta Kepemilikan
        std::cout << "Apakah anda mau membelinya (y/n)";
        std::string input = "belum";
        while (input != "y" && input != "n") {
            std::cin >> input;
        }   
        if (input == "y") {
            try {
                beliLahan(user);
                std::cout << "[SUCCESS] Properti berhasil dibeli!\n";
            }catch (const UangTidakCukupException& e){
                std::cout << "Kasian gak ada duit!\nWAKTUNYA LELANG!!!\n";
                // WAKTUNYA LELANG
            }catch (const std::exception& e) {
                std::cout << "[ERROR] Kegagalan: " << e.what() << "\n";
            }
        } else {
            std::cout << "[INFO] Anda memilih untuk tidak membeli properti.\n";
            // WAKTUNYA LELANG
        }                    
    }

    else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        std::cout << "Waktunya bayar sewa!\n";
        bayarSewa(user);
    }
    else{return;}
}
void PetakLahan::hancurkanBangunan() {
    Street* jalan = dynamic_cast<Street*>(sertifikat);
    if (jalan != nullptr) {
        jalan->setJumlahRumah(0);
        jalan->setHotel(false);
    }else {
        std::cout << "[ERROR] Petak ini bukan jenis Street, tidak memiliki bangunan.\n";
    }
}

void PetakLahan::hancurkanSatuBangunan() {
    Street* jalan = dynamic_cast<Street*>(sertifikat);
    if (jalan != nullptr) {
        if (jalan->isHotel()) {
            jalan->setHotel(false);
            std::cout << "[INFO] Hotel di properti " << getName() << " telah dihancurkan.\n";
        } else if (jalan->getJumlahBangunan() > 0) {
            jalan->setJumlahRumah(jalan->getJumlahBangunan() - 1);
            std::cout << "[INFO] Satu rumah di properti " << getName() << " telah dihancurkan. Sisa rumah: " << jalan->getJumlahBangunan() << "\n";
        } else {
            std::cout << "[INFO] Tidak ada bangunan yang dapat dihancurkan di properti ini.\n";
        }
    } else {
        std::cout << "[ERROR] Petak ini bukan jenis Street, tidak memiliki bangunan.\n";
    }
}

// [2.2] Class PetakStasiun (Inheritance dari PetakProperti)
PetakStasiun::PetakStasiun() {}
PetakStasiun::PetakStasiun(int index, std::string kodePetak, std::string name, std::string kategori, RailRoad* sertifikat, std::string warna)
: PetakProperti(index, kodePetak, name, kategori, sertifikat, warna){}
PetakStasiun::~PetakStasiun() {}

void PetakStasiun::bayarSewa(User* user) {
    RailRoad* stasiun = dynamic_cast<RailRoad*>(sertifikat);
    int biayaSewa = stasiun->hitungSewa(0);

    if (user->getUang() < biayaSewa) {
        throw UangTidakCukupException();
    }
    *(user) -= biayaSewa;
    *(sertifikat->getOwner()) += biayaSewa;
}
void PetakStasiun::onLanded(User* user, Game* game) {
    (void) game;
    if (sertifikat->getStatus()==PropStatus::BANK){
        std::cout << "Kamu mendapatkan stasiun kereta api "<< getName() << "!\n";
        sertifikat->setOwner(user);
        sertifikat->setStatus(PropStatus::OWNED);
    }else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        std::cout << "Waktunya bayar sewa!\n";
        bayarSewa(user);
    }
    else{return;}
}

// [2.3] Class PetakUtilitas (Inheritance dari PetakProperti)
PetakUtilitas::PetakUtilitas() {}
PetakUtilitas::PetakUtilitas(int index, std::string kodePetak, std::string name, std::string kategori, Utility* sertifikat, std::string warna)
: PetakProperti(index, kodePetak, name, kategori, sertifikat, warna){}
PetakUtilitas::~PetakUtilitas() {}

// [!] TODO : [TASK 3]
void PetakUtilitas::bayarSewa(User* user, Game* game) {
    Utility* utilitas = dynamic_cast<Utility*>(sertifikat);
    int biayaSewa = utilitas->hitungSewa(game->getDadu()->getTotal());

    if (user->getUang() < biayaSewa) {
        throw UangTidakCukupException();
    }
    *(user) -= biayaSewa;
    *(sertifikat->getOwner()) += biayaSewa;
}
void PetakUtilitas::onLanded(User* user, Game* game) {
    if (sertifikat->getStatus()==PropStatus::BANK){
        std::cout << "Kamu mendapatkan utilitas "<< getName() << "!\n";
        sertifikat->setOwner(user);
        sertifikat->setStatus(PropStatus::OWNED);
    }else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        std::cout << "Waktunya bayar sewa!\n";
        bayarSewa(user, game);
    }
    else{return;}
}




// [3] Abstract Class : Class PetakAksi {Inheritance dari Petak}
PetakAksi::PetakAksi() {}
PetakAksi::PetakAksi(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
: Petak(index, kodePetak, name, kategori, warna){}

// [3.1] Class PetakKartu {Inheritance dari PetakAksi}
// Ada di HPP
// PetakKartu::PetakKartu() : PetakAksi() {}
// PetakKartu::~PetakKartu() {}
template <class T>
void PetakKartu<T>::onLanded(User* user, Game* game) {}

// [3.2] Class PetakFestival {Inheritance dari PetakAksi}
PetakFestival::PetakFestival() : PetakFestival(0, "FES", "Festival", "Aksi", "NONE") {}
PetakFestival::PetakFestival(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index, kodePetak, name, kategori, warna) {}
PetakFestival::~PetakFestival() {}

void PetakFestival::onLanded(User* user, Game* game) {
    (void) game;
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
    std::cout << "[INFO] Nilai sewa saat ini menjadi: M" << targetProperti->hitungSewa(0)
              << " (Aktif 3 giliran)\n";
}

// [3.3] Class PetakPajak {Inheritance dari PetakAksi}
PetakPajak::PetakPajak() : PetakPajak(0, "PJK", "Pajak", "Pajak", "NONE") {}
PetakPajak::PetakPajak(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index, kodePetak, name, kategori, warna) {}
PetakPajak::~PetakPajak() {}
// [3.3.1] Class PetakPPH {Inheritance dari PetakPajak}
PetakPPH::PetakPPH() : PetakPPH(0.0f, 0.0f) {}
PetakPPH::PetakPPH(float flat, float percent)
    : PetakPPH(0, "PPH", "Pajak Penghasilan", "Pajak", "NONE", flat, percent) {}
PetakPPH::PetakPPH(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, float flat, float percent)
    : PetakPajak(index, kodePetak, name, kategori, warna), pajakFlat(flat), pajakPercent(percent) {}
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
PetakPBM::PetakPBM() : PetakPBM(0.0f) {}
PetakPBM::PetakPBM(float flat)
    : PetakPBM(0, "PBM", "Pajak Barang Mewah", "Pajak", "NONE", flat) {}
PetakPBM::PetakPBM(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, float flat)
    : PetakPajak(index, kodePetak, name, kategori, warna), pajakFlat(flat) {}
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



// [3.4] ===PetakSpesial===
PetakSpesial::PetakSpesial() : PetakSpesial(0, "SPS", "Spesial", "Spesial", "NONE") {}
PetakSpesial::PetakSpesial(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index, kodePetak, name, kategori, warna) {}
PetakSpesial::~PetakSpesial() {}

// [3.4.1] Class PetakGo {Inheritance dari PetakSpesial}
PetakGo::PetakGo(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, int earnMoney)
    : PetakSpesial(index, kodePetak, name, kategori, warna), earnMoney(earnMoney) {}
PetakGo::~PetakGo() {}

void PetakGo::onLanded(User* user, Game* game) {
    (void) game;
    if (user->getKoordinat()==index) {
        *(user) += earnMoney;
        std::cout << "[INFO] Selamat datang di GO! Kamu mendapatkan M" << earnMoney << ". Total uang: M" << user->getUang() << "\n";
    } else{
        // Logika Melewati GO : Case dapat uang dan tidak 
    }
}

int PetakGo::getEarnMoney() const {
    return earnMoney;
}

// [3.4.2] Class PetakPenjara {Inheritance dari PetakSpesial}
PetakPenjara::PetakPenjara(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna, int denda)
    : PetakSpesial(index, kodePetak, name, kategori, warna), denda(denda) {}
PetakPenjara::~PetakPenjara() {}
void PetakPenjara::onLanded(User* user, Game* game) {
    (void) game;
    if (user->getStatus() == 1) {
        std::cout << "[INFO] Kamu berada di dalam Penjara.\n";
    } else {
        std::cout << "[INFO] Kamu hanya mampir di Penjara. Tidak ada denda atau status tahanan.\n";
    }
}


// [3.4.3] Class PetakBebasParkir {Inheritance dari PetakSpesial}
PetakBebasParkir::PetakBebasParkir() : PetakBebasParkir(0, "PKR", "Bebas Parkir", "Spesial", "NONE") {}
PetakBebasParkir::PetakBebasParkir(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakSpesial(index, kodePetak, name, kategori, warna) {}
PetakBebasParkir::~PetakBebasParkir() {}
void PetakBebasParkir::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Kamu mendarat di Bebas Parkir! Nikmati waktu santaimu tanpa denda atau bonus.\n";
    // Tidak ada efek khusus, hanya tempat istirahat
}

// [3.4.4] Class PetakPergiPenjara {Inheritance dari PetakSpesial}
PetakPergiPenjara::PetakPergiPenjara() : PetakPergiPenjara(0, "PJP", "Pergi Penjara", "Spesial", "NONE") {}
PetakPergiPenjara::PetakPergiPenjara(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakSpesial(index, kodePetak, name, kategori, warna) {}
PetakPergiPenjara::~PetakPergiPenjara() {}
void PetakPergiPenjara::onLanded(User* user, Game* game) {
    std::cout << "[INFO] Kamu mendarat di Petak Pergi Penjara! Kamu akan langsung dipindahkan ke penjara.\n";
    std::cout << "[INFO] Kamu tidak mendapatkan gaji GO meskipun posisi dipindahkan melewati papan.\n";
    std::cout << "[INFO] Giliran pemain berakhir setelah masuk penjara.\n";
    user->setKoordinat(game->getBoard()->getPenjaraIndex());
    user->setStatus(1); // Status penjara
}
