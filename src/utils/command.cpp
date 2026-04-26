#include "utils/command.hpp"
#include <sstream>

Command::Command() = default;
Command::~Command() = default;

bool Command::execute(User& user, const std::string& input, Game& game, int& consecutiveDadu) {
    std::istringstream iss(input);
    std::string first;
    iss >> first;

    if (first.empty()) {
        std::cout << "Perintah kosong.\n";
        return true;
    }

    if (first == "CETAK_PAPAN"){
        BoardView::cetakPapan(*game.getBoard(), game.getPemain());
        return true;
    }
    else if ((first == "LEMPAR_DADU" || first == "ATUR_DADU") && consecutiveDadu >= 0){
        if (first == "ATUR_DADU") {
            int x, y;
            std::string extra;
            if ((iss >> x >> y) && !(iss >> extra)) {
                game.getDadu()->manual(x,y);
            }else{
                std::cout << "Format ATUR_DADU salah. Gunakan: ATUR_DADU <X> <Y>\n";
                return true;
            }
        }else{
            game.getDadu()->shuffle();
        }
        if (game.getDadu()->isDouble()) consecutiveDadu += 1;
        else consecutiveDadu -= 1;

        if (consecutiveDadu >= 3) {
            user.sendToJail(game.getBoard()->getPenjaraIndex());
            std::cout << "Anda melempar dadu 3 kali berturut-turut! Anda dikirim ke penjara.\n";
            return false;
        }
        user.move(game.getDadu()->getTotal(),game.getBoard());
        game.getBoard()->getPetakAt(user.getKoordinat())->onLanded(&user, &game);
        game.getDadu()->reset();
        return true;
    }
    else if (first == "CETAK_AKTA"){
        std::string input;
        if (iss >> input && (game.getLokasiKode().find(input) != game.getLokasiKode().end())) {
            // CETAK AKTA untuk Properti 
            return true;
        }
        std::cout << "Properti tidak ditemukan.\n";
        return true;
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
        return true;
    }
    else if (first == "GADAI"){
        // JALANIN FUNGSI GADAI
        // Tampilin Daftar Properti 
        // game.prosesGadai(user, game.getLokasiKode()[input]);
        return true;
    }
    else if (first == "TEBUS"){
        // JALANIN FUNGSI TEBUS
        // Tampilin Daftar Properti
        // game.prosesTebus(user, game.getLokasiKode()[input]);
        return true;
    }
    else if (first == "BANGUN"){
        // JALANIN FUNGSI BANGUN
        // Tampilin Daftar Properti
        // game.prosesBangun(game.getLokasiKode()[input]);
        return true;
    }
    else if (first == "GUNAKAN_KEMAMPUAN"){
        // JALANIN FUNGSI GUNAKAN_KEMAMPUAN
        // Tampilin Daftar KEMAMPUAN
        // [!] MASIH BELUM
        return true;
    }
    else if (first == "SIMPAN"){
        std::string namaFile;
        std::string extra;
        configBase config("");
        if ((iss >> namaFile) && !(iss >> extra)) {
            config.save(namaFile, game);
            return true;
        }
        std::cout << "Format SIMPAN salah. Gunakan: SIMPAN <nama_file>\n";
        return true;
    }
    else if (first == "MUAT"){
        std::string namaFile;
        std::string extra;
        if ((iss >> namaFile) && !(iss >> extra)) {
            
            return true;
        }
        std::cout << "Format MUAT salah. Gunakan: MUAT <nama_file>\n";
        return true;
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
        return true;
    }
    else if (first == "SELESAI"){
        return false;
    }
    else {
        std::cout << "Perintah tidak dikenali. Ketik HELP untuk daftar perintah yang tersedia.\n";
        return true;
    }
}
