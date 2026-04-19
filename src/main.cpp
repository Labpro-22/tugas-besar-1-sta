#include "../include/core/game.hpp"

// === PERMAINAN MONOPOLI === 

int main(){
    std::cout << "Selamat Datang di Permainan Monopoli!";
    bool match = true;

    while (match){
        // Command Match
        Game monopoli;



        // Validasi Kalau jadi end
        match = !(monopoli.isEnd());
    }

    std::cout << "==== GAME OVER ====";
}