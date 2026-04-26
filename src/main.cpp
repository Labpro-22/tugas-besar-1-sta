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
    std::cout << "Apakah anda mau load/save (ketik \"MUAT\" jika mau): ";
    std::cin >> option;
    Game Nimonspoli;
    if (option=="MUAT") {
        std::string pathLoad;
        std::cout << "Masukkan path file save: ";
        std::cin >> pathLoad;

        try {
            config.load(pathLoad);
            Nimonspoli = build.buildLoadGame(&config, config.getLoadSaveConfig());
        } catch (const std::exception& e) {
            std::cout << "[ERROR] Gagal memuat game: " << e.what() << "\n";
            return 1;
        }
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
        Nimonspoli.bagikanKartuSpesial(Nimonspoli.getPemain()[Nimonspoli.getCurrentPemainIndex()]);
        // Command Match
        while (loop) {
            std::cout << ">> " << std::flush;
            std::getline(std::cin >> std::ws, command);
            loop = cmd.execute(Nimonspoli.getPemain()[Nimonspoli.getCurrentPemainIndex()], command, Nimonspoli, consecutiveDadu);
        }
        Nimonspoli.getPemain()[Nimonspoli.getCurrentPemainIndex()].resetEfekKartuSpesial();

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
