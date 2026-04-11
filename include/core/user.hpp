#include "petak.hpp"
#include "properti.hpp"
#include <vector>
// Rancangan Awal [Bisa berubah ya nanti!]
// 1. Vector: Simpan state user dan simpan data di class lain.
// 2. Map : Hubungin user dan data di class lain
// 3. Map :: user -> data lain (Vector)




class User{
private:
    int uang;
    int koordinat;                              // Kotak ke-?
    std::vector<Properti> listProperti;         //Properti yang dipunya


public:
    User();
    ~User();

    void move(int dadu); // Bergerak sesuai jumlah dadu

};

// Transcation Logger
class Logger{
private:
    
public:
};