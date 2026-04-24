#include "utils/command.hpp"
#include <sstream>

Command::Command() = default;
Command::~Command() = default;

bool Command::execute(const std::string& input, Game& game) {
    std::istringstream iss(input);
    std::string first;
    iss >> first;

    if (first.empty()) {
        std::cout << "Perintah kosong.\n";
        return false;
    }

    if (first == "CETAK_PAPAN"){
        // BoardView
        return true;
    }
    else if (first == "LEMPAR_DADU"){

        return true;
    }
    else if (first == "ATUR_DADU"){
        int x, y;
        std::string input;
        if ((iss >> x >> y) && !(iss >> input)) {
            return true;
        }
        std::cout << "Format ATUR_DADU salah. Gunakan: ATUR_DADU <X> <Y>\n";
        return false;
    }
    else if (first == "CETAK_AKTA"){
        std::string input;
        if (iss >> input && (game.getLokasiKode().find(input) != game.getLokasiKode().end())) {
            // CETAK AKTA untuk Properti 
            return true;
        }
        std::cout << "Properti tidak ditemukan.\n";
        return false;
    }
    else if (first == "CETAK_PROPERTI"){
        std::string input;
        iss >> input;
        auto& pemain = game.getPemain();
        auto it = std::find_if(pemain.begin(), pemain.end(), [&input](const User& user) {
            return user.getUsername() == input;
        });
        if (it != pemain.end()) {
            // CETAK PROPERTI untuk pemain
            return true;
        }
        std::cout << "User tidak ditemukan.\n";
        return false;
    }
    else if (first == "GADAI"){
        return true;
    }
    else if (first == "TEBUS"){
        return true;
    }
    else if (first == "BANGUN"){
        return true;
    }
    else if (first == "GUNAKAN_KEMAMPUAN"){
        return true;
    }
    else if (first == "SIMPAN"){
        std::string namaFile;
        std::string extra;
        if ((iss >> namaFile) && !(iss >> extra)) {
            return true;
        }
        std::cout << "Format SIMPAN salah. Gunakan: SIMPAN <nama_file>\n";
        return false;
    }
    else if (first == "MUAT"){
        std::string namaFile;
        std::string extra;
        if ((iss >> namaFile) && !(iss >> extra)) {
            return true;
        }
        std::cout << "Format MUAT salah. Gunakan: MUAT <nama_file>\n";
        return false;
    }
    else if (first == "CETAK_LOG"){
        std::string extra;
        if (iss.eof()) {
            return true;
        }
        int jumlah;
        if ((iss >> jumlah) && !(iss >> extra)) {
            return true;
        }
        std::cout << "Format CETAK_LOG salah. Gunakan: CETAK_LOG atau CETAK_LOG <jumlah>\n";
        return false;
    }
    else {
        std::cout << "Perintah tidak dikenali. Ketik HELP untuk daftar perintah yang tersedia.\n";
        return false;
    }
}
