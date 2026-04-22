#include "game.hpp"

// Inisialisasi Static Variabel
int Game::turn = 1;

Game::Game() : MAX_TURN(100), end(false) {} // Asumsi default batas giliran
Game::Game(int Maxturn) : MAX_TURN(Maxturn),end(false){}

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