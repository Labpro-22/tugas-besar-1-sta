#include "core/game.hpp"
#include "utils/builder.hpp"
#include "utils/command.hpp"
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
        Nimonspoli; // build.buildLoadGame(&config);
    }
    else {
        Nimonspoli = build.buildNewGame(&config);
    }
    
    std::string command;
    Command cmd;
    bool loop;
    int consecutiveDadu;
    // === Game Nimonspoli ===
    while (match){
        loop = true;
        consecutiveDadu = 0;
        // Command Match
        std::cout << ">> " << std::flush;
        std::getline(std::cin >> std::ws, command);
        while (loop) {
            loop = cmd.execute(Nimonspoli.getPemain()[Nimonspoli.getCurrentPemainIndex()], command, Nimonspoli, consecutiveDadu);
        }

        if (Nimonspoli.getCurrentPemainIndex() == static_cast<int>(Nimonspoli.getPemain().size()) - 1) {
            Nimonspoli.nextturn();
            match = !(Nimonspoli.isEnd());
            if (match) {
                Nimonspoli.nextPlayer();
            }
        } else {
            Nimonspoli.nextPlayer();
        }
        
        // Validasi Kalau jadi end
        match = match && !(Nimonspoli.isEnd());
    }

    // Hitung Pemenang

    std::cout << "==== GAME OVER ====";
}
