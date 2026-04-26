#include "../../include/core/petak.hpp"
#include "../../include/core/user.hpp"
#include "../../include/core/game.hpp"
#include "views/propertyView.hpp"
#include "../../include/core/logger.hpp"

namespace {
    int hitungTagihanSetelahEfekKartu(User* user, int tagihan, bool pembayaranSewa) {
        if (user == nullptr || tagihan <= 0) {
            return tagihan;
        }

        if (user->isShieldActive()) {
            std::cout << "[KARTU] ShieldCard aktif. Tagihan M" << tagihan
                      << " dibatalkan.\n";
            return 0;
        }

        if (pembayaranSewa && user->getActiveDiscount() > 0) {
            const int diskon = user->getActiveDiscount();
            const int potongan = tagihan * diskon / 100;
            const int tagihanAkhir = tagihan - potongan;
            std::cout << "[KARTU] DiscountCard aktif. Sewa M" << tagihan
                      << " mendapat diskon " << diskon
                      << "% menjadi M" << tagihanAkhir << ".\n";
            return tagihanAkhir;
        }

        return tagihan;
    }
}

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
void PetakLahan::bayarSewa(User* user, Game* game) {
    if (sertifikat->getOwner() == nullptr) {
        throw BukanPemilikException();
    }
    int biayaSewa = sertifikat->hitungSewa(0); // * sertifikat->getFestivalMultiplier();
    biayaSewa = user->hitungTagihanSetelahEfekKartu(biayaSewa, true);
    if (biayaSewa == 0) {
        return;
    }

    std::cout << "Sewa: " << sertifikat->getNama()
              << " (" << sertifikat->getKode() << ")\n";
    std::cout << "Harga sewa: M" << biayaSewa << "\n";

    if (game != nullptr) {
        game->prosesPembayaran(*user, sertifikat->getOwner(), biayaSewa);
    } else {
        if (user->getUang() < biayaSewa) {
            throw UangTidakCukupException();
        }
        *(user) -= biayaSewa;
        *(sertifikat->getOwner()) += biayaSewa;
    }

    game->getLogger().addLog(game->getTurn(), user->getUsername(), "Bayar Sewa", "Sewa " + getName() + " milik " + sertifikat->getOwner()->getUsername() + " sebesar M" + std::to_string(biayaSewa));
}
void PetakLahan::onLanded(User* user, Game* game) {
    if (sertifikat->getStatus()==PropStatus::BANK){
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Properti: " << sertifikat->getNama()
                  << " (" << sertifikat->getKode() << ")\n";
        std::cout << "Harga beli: M" << sertifikat->getHargaBeli() << "\n";
        std::cout << "Apakah anda mau membelinya (y/n)";
        std::string input = "belum";
        while (input != "y" && input != "n") {
            std::cin >> input;
        }   
        if (input == "y") {
            try {
                beliLahan(user);
                std::cout << "[SUCCESS] Properti berhasil dibeli!\n";

                game->getLogger().addLog(game->getTurn(), user->getUsername(), "Beli Properti", "Membeli " + getName() + " seharga M" + std::to_string(sertifikat->getHargaBeli()));

            }catch (const UangTidakCukupException& e){
                std::cout << "Kasian gak ada duit!\nWAKTUNYA LELANG!!!\n";
                // WAKTUNYA LELANG
                if (game != nullptr) {
                    game->mulaiLelang(sertifikat, user);
                }
            }catch (const std::exception& e) {
                std::cout << "[ERROR] Kegagalan: " << e.what() << "\n";
            }
        } else {
            std::cout << "[INFO] Anda memilih untuk tidak membeli properti.\n";
            // WAKTUNYA LELANG
            if (game != nullptr) {
                game->mulaiLelang(sertifikat, user);
            }
        }                    
    }

    else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Waktunya bayar sewa!\n";
        try {
            bayarSewa(user, game);
        } catch (const UangTidakCukupException& e) {
            std::cout << "[ERROR] Uang tidak cukup untuk membayar sewa.\n";
        }
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

// [2.2] Class PetakStasiun (Inheritance dari PetakProperti)
PetakStasiun::PetakStasiun() {}
PetakStasiun::PetakStasiun(int index, std::string kodePetak, std::string name, std::string kategori, RailRoad* sertifikat, std::string warna)
: PetakProperti(index, kodePetak, name, kategori, sertifikat, warna){}
PetakStasiun::~PetakStasiun() {}

void PetakStasiun::bayarSewa(User* user, Game* game) {
    RailRoad* stasiun = dynamic_cast<RailRoad*>(sertifikat);
    int biayaSewa = stasiun->hitungSewa(0);
    biayaSewa = user->hitungTagihanSetelahEfekKartu(biayaSewa, true);
    if (biayaSewa == 0) {
        return;
    }

    std::cout << "Sewa: " << sertifikat->getNama()
              << " (" << sertifikat->getKode() << ")\n";
    std::cout << "Harga sewa: M" << biayaSewa << "\n";

    if (game != nullptr) {
        game->prosesPembayaran(*user, sertifikat->getOwner(), biayaSewa);
    } else {
        if (user->getUang() < biayaSewa) {
            throw UangTidakCukupException();
        }
        *(user) -= biayaSewa;
        *(sertifikat->getOwner()) += biayaSewa;
    }

    game->getLogger().addLog(game->getTurn(), user->getUsername(), "Bayar Sewa", "Sewa " + getName() + " milik " + sertifikat->getOwner()->getUsername() + " sebesar M" + std::to_string(biayaSewa));
}
void PetakStasiun::onLanded(User* user, Game* game) {
    if (sertifikat->getStatus()==PropStatus::BANK){
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Properti: " << sertifikat->getNama()
                  << " (" << sertifikat->getKode() << ")\n";
        std::cout << "Harga beli: M" << sertifikat->getHargaBeli() << "\n";
        std::cout << "Kamu mendapatkan stasiun kereta api "<< getName() << "!\n";
        sertifikat->setOwner(user);
        sertifikat->setStatus(PropStatus::OWNED);

        game->getLogger().addLog(game->getTurn(), user->getUsername(), "Beli Properti", "Mendapat otomatis stasiun " + getName());

    }else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Waktunya bayar sewa!\n";
        try {
            bayarSewa(user, game);
        } catch (const UangTidakCukupException& e) {
            std::cout << "[ERROR] Uang tidak cukup untuk membayar sewa.\n";

        }
    }
    else{return;}
}

// [2.3] Class PetakUtilitas (Inheritance dari PetakProperti)
PetakUtilitas::PetakUtilitas() {}
PetakUtilitas::PetakUtilitas(int index, std::string kodePetak, std::string name, std::string kategori, Utility* sertifikat, std::string warna)
: PetakProperti(index, kodePetak, name, kategori, sertifikat, warna){}
PetakUtilitas::~PetakUtilitas() {}


void PetakUtilitas::bayarSewa(User* user, Game* game) {
    Utility* utilitas = dynamic_cast<Utility*>(sertifikat);
    int biayaSewa = utilitas->hitungSewa(game->getDadu()->getTotal());
    biayaSewa = user->hitungTagihanSetelahEfekKartu(biayaSewa, true);
    if (biayaSewa == 0) {
        return;
    }

    std::cout << "Sewa: " << sertifikat->getNama()
              << " (" << sertifikat->getKode() << ")\n";
    std::cout << "Harga sewa: M" << biayaSewa << "\n";

    if (game != nullptr) {
        game->prosesPembayaran(*user, sertifikat->getOwner(), biayaSewa);
    } else {
        if (user->getUang() < biayaSewa) {
            throw UangTidakCukupException();
        }
        *(user) -= biayaSewa;
        *(sertifikat->getOwner()) += biayaSewa;
    }
}
void PetakUtilitas::onLanded(User* user, Game* game) {
    if (sertifikat->getStatus()==PropStatus::BANK){
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Properti: " << sertifikat->getNama()
                  << " (" << sertifikat->getKode() << ")\n";
        std::cout << "Harga beli: M" << sertifikat->getHargaBeli() << "\n";
        std::cout << "Kamu mendapatkan utilitas "<< getName() << "!\n";
        sertifikat->setOwner(user);
        sertifikat->setStatus(PropStatus::OWNED);

        game->getLogger().addLog(game->getTurn(), user->getUsername(), "Beli Properti", "Mendapat otomatis utilitas " + getName());
        
    }else if ((sertifikat->getStatus()==PropStatus::OWNED) && sertifikat->getOwner()!=user){
        std::cout << "[INFO] Properti dimiliki oleh " << sertifikat->getOwner()->getUsername() << ".\n";
        PropertyView::cetakAkta(sertifikat);
        std::cout << "Waktunya bayar sewa!\n";
        try {
            bayarSewa(user, game);
        } catch (const UangTidakCukupException& e) {
            std::cout << "[ERROR] Uang tidak cukup untuk membayar sewa.\n";
        }
    }
    else{return;}
}




// [3] Abstract Class : Class PetakAksi {Inheritance dari Petak}
PetakAksi::PetakAksi() {}
PetakAksi::PetakAksi(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
: Petak(index, kodePetak, name, kategori, warna){}
PetakAksi::~PetakAksi() {}

// [3.1] Class PetakKartu {Inheritance dari PetakAksi}
template <class T>
PetakKartu<T>::PetakKartu() : PetakAksi() {}
template <class T>
PetakKartu<T>::PetakKartu(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index,kodePetak,name,kategori,warna), deck(){}
template <class T>
void PetakKartu<T>::onLanded(User* user, Game* game) {
    if (user == nullptr || game == nullptr) {
            return;
        }

    T* kartu = deck.draw();
    if (kartu == nullptr) {
        std::cout << "[KARTU] Deck kosong.\n";
        return;
    }

    std::cout << "[KARTU] " << user->getUsername() << " mengambil "
                << kartu->getNama() << ": " << kartu->getDeskripsi() << "\n";
    kartu->apply(game, *user);
    deck.discard(kartu);
}

template class PetakKartu<KartuKesempatan>;
template class PetakKartu<KartuDanaUmum>;

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

        game->getLogger().addLog(game->getTurn(), user->getUsername(), "Aktivasi Festival", target->getNama() + " | Multiplier: " + std::to_string(target->getFestivalMultiplier()) + "x");

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
    (void) game;
    std::cout << "[INFO] Mendarat di Pajak Penghasilan (PPH)\n";
    bayarPajak(*user, game);
}
void PetakPPH::bayarPajak(User& user, Game* game) {
    int pilihan;
    std::cout << "Opsi Pembayaran PPH:\n"
              << "1. Bayar flat (M" << pajakFlat << ")\n"
              << "2. Bayar " << pajakPercent << "% dari total aset/kekayaan\n"
              << "> Pilihan (1/2): ";
    std::cin >> pilihan;
    while (pilihan != 1 && pilihan != 2) {
        std::cout << "Pilihan tidak valid. Masukkan 1 atau 2: ";
        std::cin >> pilihan;
    }
    int tagihanPajak;
    if (pilihan == 1) {
        tagihanPajak = user.hitungTagihanSetelahEfekKartu(static_cast<int>(pajakFlat), false);   
    } else {
        float totalKekayaan = user.getTotalKekayaan(); 
        float pajakPersentase = totalKekayaan * (pajakPercent / 100.0f);
        
        std::cout << "[INFO] Total aset: M" << totalKekayaan << " | Potongan: M" << pajakPersentase << "\n";

        tagihanPajak = user.hitungTagihanSetelahEfekKartu(static_cast<int>(pajakPersentase), false);
    }

    if (tagihanPajak == 0) {
        std::cout << "[SUCCESS] Pajak dibatalkan oleh ShieldCard.\n";
        return;
    }
    if (game != nullptr) {
        game->prosesPembayaran(user, nullptr, tagihanPajak);
        if (!user.isBankrupt()) {
            std::cout << "[SUCCESS] Pajak terbayar. Sisa uang: M" << user.getUang() << "\n";

        game->getLogger().addLog(game->getTurn(), user.getUsername(), "Bayar Pajak", "Membayar Pajak sebesar M" + std::to_string(tagihanPajak));

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
    (void) game;
    std::cout << "[INFO] Mendarat di Pajak Barang Mewah (PBM)\n";
    std::cout << "[INFO] Potongan pajak flat: M" << pajakFlat << "\n";
    bayarPajak(*user, game);
}
void PetakPBM::bayarPajak(User& user, Game* game) {
    const int tagihanPajak = user.hitungTagihanSetelahEfekKartu(static_cast<int>(pajakFlat), false);
    if (tagihanPajak == 0) {
        std::cout << "[SUCCESS] Pajak dibatalkan oleh ShieldCard.\n";
        return;
    }
    if (game != nullptr) {
        game->prosesPembayaran(user, nullptr, tagihanPajak);
        if (!user.isBankrupt()) {
            std::cout << "[SUCCESS] Pajak terbayar. Sisa uang: M" << user.getUang() << "\n";

        game->getLogger().addLog(game->getTurn(), user.getUsername(), "Bayar Pajak", "Membayar Pajak sebesar M" + std::to_string(tagihanPajak));

        }
    }
}



// [3.4] ===PetakSpesial===
PetakSpesial::PetakSpesial() : PetakSpesial(0, "SPS", "Spesial", "Spesial", "NONE") {}
PetakSpesial::PetakSpesial(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakAksi(index, kodePetak, name, kategori, warna) {}
PetakSpesial::~PetakSpesial() {}

// [3.4.1] Class PetakGo {Inheritance dari PetakSpesial}
PetakGo::PetakGo(){}
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
        std::cout << "[INFO] Kamu berada di dalam Penjara. Percobaan keluar: "
                  << user->getJailTurns() << "/3.\n";
        // Implementasi Penjara
    } else {
        std::cout << "[INFO] Kamu hanya mampir di Penjara. Tidak ada denda atau status tahanan.\n";
    }
}

int PetakPenjara::getDenda() const {
    return denda;
}


// [3.4.3] Class PetakBebasParkir {Inheritance dari PetakSpesial}
PetakBebasParkir::PetakBebasParkir() : PetakBebasParkir(0, "PKR", "Bebas Parkir", "Spesial", "NONE") {}
PetakBebasParkir::PetakBebasParkir(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakSpesial(index, kodePetak, name, kategori, warna) {}
PetakBebasParkir::~PetakBebasParkir() {}
void PetakBebasParkir::onLanded(User* user, Game* game) {
    (void) user;
    (void) game;
    std::cout << "[INFO] Kamu mendarat di Bebas Parkir! Nikmati waktu santaimu tanpa denda atau bonus.\n";
    // Tidak ada efek khusus, hanya tempat istirahat
}

// [3.4.4] Class PetakPergiPenjara {Inheritance dari PetakSpesial}
PetakPergiPenjara::PetakPergiPenjara() : PetakPergiPenjara(0, "PJP", "Pergi Penjara", "Spesial", "NONE") {}
PetakPergiPenjara::PetakPergiPenjara(int index, std::string kodePetak, std::string name, std::string kategori, std::string warna)
    : PetakSpesial(index, kodePetak, name, kategori, warna) {}
PetakPergiPenjara::~PetakPergiPenjara() {}
void PetakPergiPenjara::onLanded(User* user, Game* game) {
    if (user == nullptr || game == nullptr) {
        return;
    }

    std::cout << "[INFO] Kamu mendarat di Petak Pergi Penjara! Kamu akan langsung dipindahkan ke penjara.\n";
    if (user->isShieldActive()) {
        std::cout << "[INFO] ShieldCard menahan efek Pergi Penjara.\n";
    } else {
        std::cout << "[INFO] Kamu tidak mendapatkan gaji GO meskipun posisi dipindahkan melewati papan.\n";
        std::cout << "[INFO] Giliran pemain berakhir setelah masuk penjara.\n";
    }
    game->sendPlayerToJail(*user);
}
