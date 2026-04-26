#ifndef GAME_HPP
#define GAME_HPP

#include "user.hpp" 
#include "petak.hpp"
#include "board.hpp"
#include "../utils/dadu.hpp"
#include "lelang.hpp"
#include "logger.hpp"
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <memory>

class Game{
private:
    // Atribut Game
    int MAX_TURN;
    int turn;
    bool end;
    std::vector<User> pemain;
    Logger gameLogger;
    std::vector<std::unique_ptr<Properti>> daftarProperti;
    
    // Urutan Pemain (Angka melambangkan indeks di vector pemain)
    int currentPemain;

    // Composition
    Board board;
    Dadu dadu;
    CardDeck<KartuSpesial> deckKartuSpesial;
    bool sudahPakaiKartuKemampuan;
    // Mapping
    std::map<std::string, PetakProperti*> lokasiKode; // Kode -> Petak
    std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup; // Color X -> Semua Petak dengan color X
public:
    Game();
    Game(int Maxturn);
    Game(int maxTurn,int turn,bool end,std::vector<User> pemain,std::vector<std::unique_ptr<Properti>>&& daftarProperti,
        Board board,Dadu dadu,std::map<std::string, PetakProperti*> lokasiKode,std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup);
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) noexcept = default;
    Game& operator=(Game&&) noexcept = default;
    ~Game() = default;

    // Getter
    bool isEnd(); // Kondisi Max Turn atau Config : Bangkrut
    int getTurn();
    int getTurn() const;
    int getMaxTurn() const;
    Board* getBoard();
    std::vector<User>& getPemain();
    const std::vector<User>& getPemain() const;
    Logger& getLogger();
    const Logger& getLogger() const;
    Dadu* getDadu();
    std::map<std::string, PetakProperti*>& getLokasiKode();
    std::map<std::string, std::vector<PetakProperti*>>& getLokasiColorGroup();
    CardDeck<KartuSpesial>& getDeckKartuSpesial();
    const CardDeck<KartuSpesial>& getDeckKartuSpesial() const;
    std::vector<std::unique_ptr<Properti>>& getDaftarProperti();

    int getCurrentPemainIndex() const;
    int getActivePlayerCount() const;
    int getJailFine() const;

    void setMAXTURN(int max);
    void nextturn(); // Ganti cycle
    void nextPlayer();
    void bagikanKartuSpesial(User& user);
    void mulaiLelang(Properti* targetProperti, User* pemicu);
    bool handleJailTurn(User& user);
    void sendPlayerToJail(User& user);
    void leave(User& user);

    // Method 
    void move(int langkah, User& user);
    void prosesGadai(User& user, Properti* properti);
    void prosesTebus(User& user, Properti* properti);
    void prosesBangun(Properti* properti);
    void prosesPakaiKartu(User& user, KartuSpesial* kartu);
    void prosesLoad();
    void prosesSave();
};

#endif
