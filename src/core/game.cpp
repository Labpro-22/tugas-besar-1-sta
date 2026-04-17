#include "game.hpp"

// Inisialisasi Static Variabel
int Game::turn = 1;

Game::Game() {
    this->MAX_TURN = 100; // Asumsi default batas giliran
    this->end = false;
    // Objek 'board' akan otomatis memanggil constructor Board() miliknya sendiri
}

Game::~Game() {
    pemain.clear();
    Log.clear();
}


bool Game::isEnd() {
    if (pemain.size() <= 1 && !pemain.empty()) {
        return true;
    }
    // Atau jika sudah mencapai MAX_TURN (variabel end = true)
    return this->end;
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