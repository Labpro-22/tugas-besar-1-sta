#include "utils/command.hpp"
#include "views/propertyView.hpp"
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
        BoardView::cetakPapan(game);
        return true;
    }
    else if ((first == "LEMPAR_DADU" || first == "ATUR_DADU") && consecutiveDadu >= 0){
        if (consecutiveDadu < 0) {
            std::cout << "CURANG!!! Anda sudah melempar dadu, tidak boleh melempar lagi kecuali jika mendapatkan double.\n";
            return true;
        }
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
        std::cout << "Masukkan kode properti: ";
        std::cin >> input;
        if (game.getLokasiKode().find(input) != game.getLokasiKode().end()) {
            // CETAK AKTA untuk Properti
            Properti* properti = game.getLokasiKode()[input]->getSertifikat();
            PropertyView::cetakAkta(properti);
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
            PropertyView::cetakProperti(*it);
            return true;
        }
        std::cout << "User tidak ditemukan.\n";
        return true;
    }
    else if (first == "GADAI"){
        // Tampilin Daftar Properti 
        std::cout << "=== Properti yang Dapat Digadaikan ===\n";
        int num = 1;
        for (Properti* prop : user.getListProperti()) {
            std::cout << num << ". " << prop->getNama() << " (" << prop->getKode() << ")" << "[" << prop->getWarna() << "]" << "Nilai Gadai: " << prop->getNilaiGadai() << "\n";
            num++;
        }
        std::cout << "Uang kamu saat ini: M" << user.getUang() << "\n" << "Pilih nomor properti (0 untuk batal): ";
        int pilihan;
        std::cin >> pilihan;
        while (pilihan < 0 || pilihan > static_cast<int>(user.getListProperti().size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih nomor properti: ";
            std::cin >> pilihan;
        }
        if (pilihan == 0) {
            std::cout << "Gadai dibatalkan.\n";
            return true;
        }
        game.prosesGadai(user, user.getListProperti()[pilihan - 1]);
        return true;
    }
    else if (first == "TEBUS"){
        // Tampilin Daftar Properti
        std::cout << "=== Properti yang Sedang Digadaikan (Dapat Ditebus) ===\n";
        int num = 1, index = 0;
        std::vector<int> indexes;
        for (Properti* prop : user.getListProperti()) {
            if (prop->getStatus() == PropStatus::MORTGAGED) {
                std::cout << num << ". " << prop->getNama() << " (" << prop->getKode() << ")" << "[" << prop->getWarna() << "]" << "Nilai Tebus: " << prop->getNilaiGadai() << "\n";
                indexes.push_back(index);
                num++;
            }
            index++;
        }
        if (indexes.empty()) {
            std::cout << "Tidak ada properti yang sedang digadaikan.\n";
            return true;
        }
        std::cout << "Uang kamu saat ini: M" << user.getUang() << "\n";
        std::cout << "Pilih nomor properti (0 untuk batal): ";
        int pilihan;
        std::cin >> pilihan;
        while (pilihan < 0 || pilihan > static_cast<int>(indexes.size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih nomor properti: ";
            std::cin >> pilihan;
        }
        if (pilihan == 0) {
            std::cout << "Tebus dibatalkan.\n";
            return true;
        }
        game.prosesTebus(user, user.getListProperti()[indexes[pilihan - 1]]);
        return true;
    }
    else if (first == "BANGUN"){
        // Tampilin Daftar Properti yang layak!
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
