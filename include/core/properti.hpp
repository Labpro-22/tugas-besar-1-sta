#ifndef PROPERTI_HPP
#define PROPERTI_HPP

#include "petak.hpp"
#include "user.hpp"

class User;

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
    std::string nama;
    int hargaBeli;
    int nilaiGadai;
    PropStatus status;
    User* owner;
    
    //Atribut untuk festival
    int duasiFestival;
    int festivalMultiplier;

public:
    Properti(int id, std::string kode, std::string nama, int hargaBeli, int nilaiGadai);
    ~Properti() = default;

    virtual int hitungSewa(int lemparanDadu) const = 0; //Menghitung biaya sewa

    virtual void gadaikan(); //Menggadaikan properti ke Bank

    virtual void tebus(); //Menebus properti yang sedang digadaikan

    // Getters & Setters
    PropStatus getStatus() const; 

    void setOwner(User* newOwner);

    User* getOwner() const;

    std::string getNama() const;

    int getHargaBeli() const;

    int getId() const;
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

    int hitungSewa(int lembaranDadu) const override;

    void bangunRumah(); //Membangun rumah di properti ini

    void bangunHote(); //Melakkan upgrade dari 4 rumah menjadi hotel

    void jualBangunan(); //Menjual bangunan ke bank dengan harga setengah dari harga beli. Dipanggil wajib sebelum properti digadaikan

    std::string getWarna() const;

    int getJumlahBangunan() const;

    bool canBuild(const std::vector<Street*>& grupWarna) const; //Mengecek apakah rumah bisaa dibangun berdasarkan atruan pemerataan selisih miniaml 1

    void bangunRumah(const std::vector<Street*>& grupWarna); //Membangun rumah di properti ini

    void gadaikan(const std::vector<Street*>& grupWarna); //Menggadaikan properti khusus street
};

class RailRoad : public Properti {
public:
    RailRoad(int id, std::string kode, std::string nama, int nilaiGadai);

    int hitungSewa(int lemparanDadu) const override;
};

class Utility : public Properti {
public:
    Utility(int id, std::string kode, std::string nama, int nilaiGadai);
    int hitungSewa(int lemparanDadu) const override;
};

#endif