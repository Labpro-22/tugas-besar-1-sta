#ifndef IO_HPP
#define IO_HPP

#include <string>
#include <vector>
#include "../core/game.hpp"

class PropertyConfig {
private:
    int id;
    std::string kode;
    std::string nama;
    std::string jenis;
    std::string warna;
    int hargaLahan;
    int nilaiGadai;
    int upgradeRumah;
    int upgradeHotel;
    std::vector<int> rentLevel;
public:
    PropertyConfig();
    PropertyConfig(int id, std::string& kode, std::string& nama, std::string& jenis, const std::string& warna,
    int hargaLahan, int nilaiGadai, int upgradeRumah, int upgradeHotel, std::vector<int>& rentLevel)
    : id(id), kode(kode), nama(nama), jenis(jenis), warna(warna), hargaLahan(hargaLahan), nilaiGadai(nilaiGadai),
    upgradeRumah(upgradeRumah), upgradeHotel(upgradeHotel), rentLevel(rentLevel) {}
    int getId() const { return id; }
    const std::string& getKode() const { return kode; }
    const std::string& getNama() const { return nama; }
    const std::string& getJenis() const { return jenis; }
    const std::string& getWarna() const { return warna; }
    int getHargaLahan() const { return hargaLahan; }
    int getNilaiGadai() const { return nilaiGadai; }
    int getUpgradeRumah() const { return upgradeRumah; }
    int getUpgradeHotel() const { return upgradeHotel; }
    const std::vector<int>& getRentLevel() const { return rentLevel; }
};

class IO {
private:
    std::string path;

    // Config
    std::vector<PropertyConfig> confProperty;
    std::vector<int> confRailroad;
    std::vector<int> confUtility;
    int pphFlat;
    int pphPersentase;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int MAX_TURN;
    int SALDO_AWAL;

public:
    IO();
    explicit IO(std::string path);

    Game makeGame();
    void load(const std::string& pathLoad);
    void save();

    const std::vector<PropertyConfig>& getPropertyConfig() const { return confProperty; }
    const std::vector<int>& getRailroadConfig() const { return confRailroad; }
    const std::vector<int>& getUtilityConfig() const { return confUtility; }
    int getPphFlat() const { return pphFlat; }
    int getPphPersentase() const { return pphPersentase; }
    int getPbmFlat() const { return pbmFlat; }
    int getGoSalary() const { return goSalary; }
    int getJailFine() const { return jailFine; }
    int getMaxTurn() const { return MAX_TURN; }
    int getSaldoAwal() const { return SALDO_AWAL; }
};

#endif
