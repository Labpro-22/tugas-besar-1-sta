#include "../include/core/game.hpp"
#include "../include/utils/io.hpp"
#include "../include/utils/builder.hpp"
// === PERMAINAN MONOPOLI === 

int main(){
    std::cout << "Selamat Datang di Permainan Monopoli!" << std::endl;
    bool match = true;
    std::cout << "Path untuk folder konfigurasi (Ketik default jika ditaruh di dalam folder data sesuai dengan ketentuan pada readme)" << std::endl;
    std::cout << "Masukkan Path: ";

    std::string path;
    std::cin >> path;
    configBase config(path);
    gameBuilder build;
    
    std::string option;
    std::cout << "Apakah anda mau load/save (y/n): ";
    std::cin >> option;
    Game Nimonspoli;
    if (option=="y") {
        Nimonspoli;
    }
    else {
        Nimonspoli = build.buildNewGame(&config);
    }
    
    // === Game Nimonspoli ===
    while (match){
        // Command Match
        


        // Validasi Kalau jadi end
        match = !(Nimonspoli.isEnd());
    }

    std::cout << "==== GAME OVER ====";
}