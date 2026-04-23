#include "game.hpp"
Game::Game() : MAX_TURN(100), turn(1), end(false), currentPemain(0) {} // Asumsi default batas giliran
Game::Game(int Maxturn) : MAX_TURN(Maxturn), turn(1), end(false), currentPemain(0) {}
Game::Game(int maxTurn,int turn,bool end,std::vector<User> pemain,std::vector<std::unique_ptr<Properti>> daftarProperti,
Board board,Dadu dadu,std::map<std::string, PetakProperti*> lokasiKode,std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup) 
: MAX_TURN(maxTurn),turn(turn),end(end),pemain(std::move(pemain)),
daftarProperti(daftarProperti),currentPemain(0),board(std::move(board)),
dadu(std::move(dadu)),lokasiKode(std::move(lokasiKode)),lokasiColorGroup(std::move(lokasiColorGroup)) {}

Game::~Game() {
    pemain.clear();
    Log.clear();
}

bool Game::isEnd() {
    // Jika Pemain tinggal satu atau end true
    return (this->end || pemain.size()==1);
}

void Game::setMAXTURN(int max) {
    this->MAX_TURN = max;
}

void Game::nextturn() {
    turn++;

    if (turn > MAX_TURN) {
        end = true;
    }
}

int Game::getTurn() {
    return turn;
}

void Game::leave(User& user) {
    for (auto it = pemain.begin(); it != pemain.end(); ++it) {
        if (&(*it) == &user) {
            pemain.erase(it);
            break;
        }
    }
    
    if (pemain.size() <= 1) {
        end = true;
    }
}

Board* Game::getBoard() {return &board;}
std::vector<User>& Game::getPemain() {return pemain;}
std::vector<Logger> Game::getLog(){return Log;}
Dadu* Game::getDadu() {return &dadu;}
