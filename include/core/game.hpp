#ifndef GAME_HPP
#define GAME_HPP

#include "user.hpp" 
#include "petak.hpp"
#include "board.hpp"
#include "../utils/dadu.hpp"
#include <vector>
#include <map>

class Game{
private:
    int MAX_TURN;
    static int turn;
    std::vector<User> pemain;
    std::vector<Logger> Log;
    std::vector<Properti> daftarProperti;
    Board board;
    Dadu dadu;
    
    bool end;
    std::map<Properti*, PetakProperti*> lokasiSertifikat;
public:
    Game();
    Game(int Maxturn);
    ~Game();

    bool isEnd(); // Kondisi 1 Player atau Config : Bangkrut
    void setMAXTURN(int max);
    void nextturn();

    int getTurn();
    void leave(User& user);
    Board* getBoard();
    std::vector<User>& getPemain();
    std::vector<Logger> getLog();
    Dadu* getDadu();
};

#endif