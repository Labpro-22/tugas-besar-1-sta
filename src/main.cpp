#include "../include/core/game.hpp"
#include "../include/utils/io.hpp"
// === PERMAINAN MONOPOLI === 

int main(){
    std::cout << "Selamat Datang di Permainan Monopoli!" << std::endl;
    bool match = true;
    std::cout << "Path untuk folder konfigurasi (Ketik default jika ditaruh di dalam folder data sesuai dengan ketentuan pada readme)" << std::endl;
    std::cout << "Masukkan Path: ";

    std::string path;
    std::cin >> path;
    IO config(path);

    Game game = config.makeGame();
    while (match){
        // Command Match
        Game monopoli;



        // Validasi Kalau jadi end
        match = !(monopoli.isEnd());
    }

    std::cout << "==== GAME OVER ====";
}