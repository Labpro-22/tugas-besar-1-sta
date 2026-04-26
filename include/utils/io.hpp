#ifndef configBase_HPP
#define configBase_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "core/game.hpp"

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

class AksiConfig{
private:
    int id;
    std::string kode;
    std::string nama;
    std::string jenis;
    std::string warna;
public:
    AksiConfig(int id, std::string kode, std::string nama, std::string jenis, std::string warna) 
    : id(id), kode(kode), nama(nama), jenis(jenis), warna(warna){}

    int getId() const {return id;}
    std::string getKode() const {return kode;}
    std::string getNama() const {return nama;}
    std::string getJenis() const {return jenis;}
    std::string getWarna() const {return warna;}
};

// === Load and Save ===
class confKartu{
public:
    std::string jenis;
    int nilai;
    int sisa_durasi;
};

class StatePemain{
public:
    std::string username;
    int uang;
    int koordinat;
    int status;
    std::vector<confKartu> kartu;
};

class StateProperti{
public:
    int jumlahProperti;
    std::vector<std::string> kodeProperti;
};

class confProperti{
public:
    std::string kode;
    std::string jenis;
    std::string owner;
    int status;
    int fmult;
    int fdur;
    int jumlahBangunan;
};

class StateDeck{
public:
    int jumlahKartuDeck = 0;
    std::vector<std::string> jenis;
    std::vector<std::string> drawJenis;
    std::vector<std::string> discardJenis;
};

class confLog{
public:
    int turn;
    std::string username;
    std::string jenisAksi;
    std::string detail;
};

class StateLog{
public:
    int jumlahLog;
    std::vector<confLog> log;
};

class configLoadSave{
private:
    int currentTurn = 0;
    int MAX_TURN = 0;
    int countPemain = 0;
    int currentPemainIndex = 0;
    bool kartuSpesialSudahDibagikan = true;

    std::vector<StatePemain> pemain;
    std::vector<StateProperti> properti;
    StateDeck deckKartuSpesial;
    StateLog log;
public:
    int getCurrentTurn() const { return currentTurn; }
    void setCurrentTurn(int value) { currentTurn = value; }

    int getMaxTurn() const { return MAX_TURN; }
    void setMaxTurn(int value) { MAX_TURN = value; }

    int getCountPemain() const { return countPemain; }
    void setCountPemain(int value) { countPemain = value; }

    int getCurrentPemainIndex() const { return currentPemainIndex; }
    void setCurrentPemainIndex(int value) { currentPemainIndex = value; }

    bool getKartuSpesialSudahDibagikan() const { return kartuSpesialSudahDibagikan; }
    void setKartuSpesialSudahDibagikan(bool value) { kartuSpesialSudahDibagikan = value; }

    const std::vector<StatePemain>& getPemain() const { return pemain; }
    void setPemain(const std::vector<StatePemain>& value) { pemain = value; }

    const std::vector<StateProperti>& getProperti() const { return properti; }
    void setProperti(const std::vector<StateProperti>& value) { properti = value; }

    const StateDeck& getDeckKartuSpesial() const { return deckKartuSpesial; }
    void setDeckKartuSpesial(const StateDeck& value) { deckKartuSpesial = value; }

    const StateLog& getLog() const { return log; }
    void setLog(const StateLog& value) { log = value; }
};

class configBase {
private:
    std::string path;

    // Config
    std::vector<PropertyConfig> confProperty;
    std::map<int,int> confRailroad;
    std::map<int,int> confUtility;
    std::vector<AksiConfig> confAksi;
    int pphFlat;
    int pphPersentase;
    int pbmFlat;
    int goSalary;
    int jailFine;
    int MAX_TURN;
    int SALDO_AWAL;

    std::unique_ptr<configLoadSave> confLoadSave;

public:
    explicit configBase(std::string path);

    // Load dan Save Game
    void load(const std::string& pathLoad);
    void save(const std::string &pathSave , const Game& game);

    void setLoadSaveConfig(const configLoadSave& config);
    const configLoadSave* getLoadSaveConfig() const { return confLoadSave.get(); }
    bool hasLoadSaveConfig() const { return confLoadSave != nullptr; }

    // Getter
    const std::vector<PropertyConfig>& getPropertyConfig() const { return confProperty; }
    const std::map<int,int>& getRailroadConfig() const { return confRailroad; }
    const std::map<int,int>& getUtilityConfig() const { return confUtility; }
    const std::vector<AksiConfig>& getAksiConfig() const {return confAksi;}
    int getPphFlat() const { return pphFlat; }
    int getPphPersentase() const { return pphPersentase; }
    int getPbmFlat() const { return pbmFlat; }
    int getGoSalary() const { return goSalary; }
    int getJailFine() const { return jailFine; }
    int getMaxTurn() const { return MAX_TURN; }
    int getSaldoAwal() const { return SALDO_AWAL; }
};

#endif
