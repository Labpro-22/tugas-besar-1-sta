#ifndef PROPERTI_HPP
#define PROPERTI_HPP

#include <string>
#include <vector>
#include "../utils/gameException.hpp"

class User; // Forward declaration untuk menghindari circular include

// Enum untuk mendefinisikan status kepemilikan dari suatu properti
enum class PropStatus {
    BANK,
    OWNED,
    MORTGAGED
};

class Properti{
protected:
    int id;
    std::string kode;


    // [?]  tabel harga sewa per level bangunan
    std::string nama;
    int hargaBeli;
    int nilaiGadai;
    PropStatus status;
    User* owner;
    
    //Atribut untuk festival
    int durasiFestival;
    int festivalMultiplier;

public:
    Properti(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai);
    ~Properti() = default;

    virtual int hitungSewa(int lemparanDadu) const = 0; //Menghitung biaya sewa
    virtual void gadaikan(); //Menggadaikan properti ke Bank
    virtual void tebus(); //Menebus properti yang sedang digadaikan

    // Getters & Setters
    PropStatus getStatus() const; 
    User* getOwner() const;
    std::string getNama() const;
    int getHargaBeli() const;
    int getId() const;
    std::string getKode() const;
    int getFestivalMultiplier() const;
    int getFestivalDuration() const;
    virtual int getSewaSaatIni(int dadu) const;
    void setFestivalMultiplier(int m);
    void setOwner(User* newOwner);
    void setFestivalDuration(int d);
};

class Street : public Properti {
private:
    std::string warna;
    std::vector<int> hargaSewa;
    int hargaBangunan;
    int jumlahRumah;
    bool hasHotel;

public:
    Street(int id, std::string kode, std::string nama, std::string warna, int hargaBeli, int nilaiGadai, int hargaBangunan, std::vector<int> sewa);

    int hitungSewa(int lemparanDadu) const override;
    int hitungSewa(int lemparanDadu, const std::vector<Street*>& grupWarna) const;
    void bangunHotel(const std::vector<Street*>& grupWarna); //Melakkan upgrade dari 4 rumah menjadi hotel
    void bangunRumah(const std::vector<Street*>& grupWarna); //Membangun rumah di properti ini
    bool canBuild(const std::vector<Street*>& grupWarna) const; //Mengecek apakah rumah bisaa dibangun berdasarkan atruan pemerataan selisih minimal 1
    void jualBangunan(const std::vector<Street*>& grupWarna); //Menjual bangunan ke bank dengan harga setengah dari harga beli. Dipanggil wajib sebelum properti digadaikan
    void gadaikan(); //Menggadaikan properti Street (sekalian menjual semua bangunan dalam color group)

    std::string getWarna() const;
    int getJumlahBangunan() const;
    int getHargaBangunan() const;
};

class RailRoad : public Properti {
private:
    int hargaSewaDasar;
public:
    RailRoad(int id, std::string kode, std::string nama, int nilaiGadai, int hargaBeli, int hargaSewaDasar);
    int hitungSewa(int lemparanDadu) const override;
};

class Utility : public Properti {
public:
    Utility(int id, std::string kode, std::string nama, int nilaiGadai);
    int hitungSewa(int lemparanDadu) const override;
};


#endif