#ifndef GAME_HPP
#define GAME_HPP

#include "user.hpp" 
#include "petak.hpp"
#include "board.hpp"
#include "../utils/dadu.hpp"
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <memory>

class Game{
private:
    // Atribut Langsung game
    int MAX_TURN;
    int turn;
    bool end;
    std::vector<User> pemain;
    std::vector<Logger> Log;
    std::vector<std::unique_ptr<Properti>> daftarProperti;
    
    // Urutan Pemain (Angka melambangkan indeks di vector pemain)
    std::vector<int> urutanPemain;
    int currentPemain;

    // Composition
    Board board;
    Dadu dadu;
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

    bool isEnd(); // Kondisi Max Turn atau Config : Bangkrut
    void setMAXTURN(int max);
    void nextturn(); // Ganti cycle
    void nextPlayer();

    int getTurn();
    int getCurrentPemainIndex() const;
    int getActivePlayerCount() const;
    int getJailFine() const;
    bool handleJailTurn(User& user);
    void sendPlayerToJail(User& user);
    void leave(User& user);
    Board* getBoard();
    std::vector<User>& getPemain();
    std::vector<Logger> getLog();
    Dadu* getDadu();
};

#endif
