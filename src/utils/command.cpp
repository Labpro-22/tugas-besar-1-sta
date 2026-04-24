#include "utils/command.hpp"
#include <sstream>

Command::Command() = default;
Command::~Command() = default;

bool Command::execute(User& user, const std::string& input, Game& game, int consecutiveDadu) {
    std::istringstream iss(input);
    std::string first;
    iss >> first;

    if (first.empty()) {
        std::cout << "Perintah kosong.\n";
        return false;
    }

    if (first == "CETAK_PAPAN"){
        BoardView::cetakPapan(*game.getBoard(), game.getPemain());
        return true;
    }
    else if ((first == "LEMPAR_DADU" || first == "ATUR_DADU") && consecutiveDadu >= 0){
        Dadu dadu;
        if (first == "LEMPAR_DADU") {
            int x, y;
            std::string extra;
            if ((iss >> x >> y) && !(iss >> extra)) {
                dadu.manual(x,y);
            }else{
                std::cout << "Format ATUR_DADU salah. Gunakan: ATUR_DADU <X> <Y>\n";
                return false;
            }
        }else{
            dadu.shuffle();
        }
        if (dadu.isDouble()) consecutiveDadu += 1;
        else consecutiveDadu -= 1;

        if (consecutiveDadu >= 3) {
            user.sendToJail(game.getBoard()->getPenjaraIndex());
            std::cout << "Anda melempar dadu 3 kali berturut-turut! Anda dikirim ke penjara.\n";
            return false;
        }
        user.move(dadu.getTotal(),game.getBoard());
        return true;
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
        // JALANIN FUNGSI GADAI
        return true;
    }
    else if (first == "TEBUS"){
        // JALANIN FUNGSI TEBUS
        return true;
    }
    else if (first == "BANGUN"){
        // JALANIN FUNGSI BANGUN
        return true;
    }
    else if (first == "GUNAKAN_KEMAMPUAN"){
        // JALANIN FUNGSI GUNAKAN_KEMAMPUAN
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
            // CETAK_LOG tanpa jumlah, cetak semua log
            return true;
        }
        int jumlah;
        if ((iss >> jumlah) && !(iss >> extra)) {
            // CETAK_LOG dengan jumlah, cetak log sesuai jumlah
            return true;
        }
        std::cout << "Format CETAK_LOG salah. Gunakan: CETAK_LOG atau CETAK_LOG <jumlah>\n";
        return false;
    }
    else if (first == "SELESAI"){
        return false;
    }
    else {
        std::cout << "Perintah tidak dikenali. Ketik HELP untuk daftar perintah yang tersedia.\n";
        return false;
    }
}
