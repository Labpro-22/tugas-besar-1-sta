#include "utils/command.hpp"
#include "utils/bangunCommandHelper.hpp"
#include "core/bangun.hpp"
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
            const bool shieldAktif = user.isShieldActive();
            game.sendPlayerToJail(user);
            if (shieldAktif) {
                std::cout << "Anda melempar dadu 3 kali berturut-turut, tetapi ShieldCard menahan efek masuk penjara.\n";
            } else {
                std::cout << "Anda melempar dadu 3 kali berturut-turut! Anda dikirim ke penjara.\n";
            }
            return false;
        }
        user.move(game.getDadu()->getTotal(),game.getBoard());

        std::string petakTujuan = game.getBoard()->getPetakAt(user.getKoordinat())->getName();
        std::string detailGerak = "Dadu: " + std::to_string(game.getDadu()->getAngka1()) + " & " + std::to_string(game.getDadu()->getAngka2()) + " | Mendarat di: " + petakTujuan;
        game.getLogger().addLog(game.getTurn(), user.getUsername(), "Pergerakan", detailGerak);

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
        int num = 1, index = 0;
        std::vector<int> indexes;
        for (Properti* prop : user.getListProperti()) {
            if (prop != nullptr && prop->getStatus() == PropStatus::OWNED) {
                std::cout << num << ". " << prop->getNama() << " (" << prop->getKode() << ")" << "[" << prop->getWarna() << "]" << "Nilai Gadai: " << prop->getNilaiGadai() << "\n";
                indexes.push_back(index);
                num++;
            }
            index++;
        }
        if (indexes.empty()) {
            std::cout << "Tidak ada properti yang dapat digadaikan.\n";
            return true;
        }
        std::cout << "Uang kamu saat ini: M" << user.getUang() << "\n" << "Pilih nomor properti (0 untuk batal): ";
        int pilihan;
        std::cin >> pilihan;
        while (pilihan < 0 || pilihan > static_cast<int>(indexes.size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih nomor properti: ";
            std::cin >> pilihan;
        }
        if (pilihan == 0) {
            std::cout << "Gadai dibatalkan.\n";
            return true;
        }
        try {
            game.prosesGadai(user, user.getListProperti()[indexes[pilihan - 1]]);
            std::cout << "Properti berhasil digadaikan.\n";
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
        return true;
    }
    else if (first == "TEBUS"){
        // Tampilin Daftar Properti
        std::cout << "=== Properti yang Sedang Digadaikan (Dapat Ditebus) ===\n";
        int num = 1, index = 0;
        std::vector<int> indexes;
        for (Properti* prop : user.getListProperti()) {
            if (prop->getStatus() == PropStatus::MORTGAGED) {
                std::cout << num << ". " << prop->getNama() << " (" << prop->getKode() << ")" << "[" << prop->getWarna() << "]" << "Nilai Tebus: " << prop->getHargaBeli() << "\n";
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
        try {
            game.prosesTebus(user, user.getListProperti()[indexes[pilihan - 1]]);
            std::cout << "Properti berhasil ditebus.\n";
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
        return true;
    }
    else if (first == "BANGUN"){
        std::vector<BuildGroup> groups;
        for (const auto& entry : game.getLokasiColorGroup()) {
            std::vector<Street*> streets = BangunCommandHelper::getStreetGroup(entry.second);
            if (!Bangun::isOwnedBuildableGroup(streets, user)) {
                continue;
            }

            std::vector<Street*> eligible;
            for (Street* street : streets) {
                if (Bangun::canBuildOnStreet(street, streets)) {
                    eligible.push_back(street);
                }
            }

            if (!eligible.empty()) {
                groups.emplace_back(entry.first, streets, eligible);
            }
        }

        if (groups.empty()) {
            std::cout << "Tidak ada color group yang memenuhi syarat untuk dibangun.\n";
            std::cout << "Kamu harus memiliki seluruh petak dalam satu color group terlebih dahulu.\n";
            return true;
        }

        std::cout << "=== Color Group yang Memenuhi Syarat ===\n";
        for (size_t i = 0; i < groups.size(); ++i) {
            std::cout << i + 1 << ". [" << BangunCommandHelper::displayWarna(groups[i].getWarna()) << "]\n";
            for (Street* street : groups[i].getStreets()) {
                std::cout << "   ";
                BangunCommandHelper::printStreetBuildStatus(street);
            }
        }

        std::cout << "\nUang kamu saat ini : " << BangunCommandHelper::formatUangCommand(user.getUang()) << "\n";
        std::cout << "Pilih nomor color group (0 untuk batal): ";
        int pilihanGroup;
        std::cin >> pilihanGroup;
        while (pilihanGroup < 0 || pilihanGroup > static_cast<int>(groups.size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih nomor color group: ";
            std::cin >> pilihanGroup;
        }
        if (pilihanGroup == 0) {
            std::cout << "Bangun dibatalkan.\n";
            return true;
        }

        const BuildGroup& selectedGroup = groups[pilihanGroup - 1];
        std::cout << "\nColor group [" << BangunCommandHelper::displayWarna(selectedGroup.getWarna()) << "]:\n";
        bool semuaSiapHotel = true;
        for (Street* street : selectedGroup.getStreets()) {
            if (street == nullptr || street->isHotel() || street->getJumlahBangunan() != 4) {
                semuaSiapHotel = false;
            }
        }
        if (semuaSiapHotel) {
            std::cout << "Seluruh color group [" << BangunCommandHelper::displayWarna(selectedGroup.getWarna()) << "] sudah memiliki 4 rumah. Siap di-upgrade ke hotel!\n";
        }

        for (size_t i = 0; i < selectedGroup.getEligible().size(); ++i) {
            Street* street = selectedGroup.getEligible()[i];
            std::cout << i + 1 << ". " << street->getNama() << " (" << street->getKode() << ") : " << BangunCommandHelper::statusBangunan(street);
            if (street->getJumlahBangunan() == 4 && !street->isHotel()) {
                std::cout << " <- siap upgrade ke hotel";
            } else {
                std::cout << " <- dapat dibangun";
            }
            std::cout << "\n";
        }

        std::cout << "Pilih petak (0 untuk batal): ";
        int pilihanPetak;
        std::cin >> pilihanPetak;
        while (pilihanPetak < 0 || pilihanPetak > static_cast<int>(selectedGroup.getEligible().size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih petak: ";
            std::cin >> pilihanPetak;
        }
        if (pilihanPetak == 0) {
            std::cout << "Bangun dibatalkan.\n";
            return true;
        }

        Street* target = selectedGroup.getEligible()[pilihanPetak - 1];
        if (target->getJumlahBangunan() == 4 && !target->isHotel()) {
            std::cout << "Upgrade ke hotel? Biaya: " << BangunCommandHelper::formatUangCommand(target->getHargaHotel()) << " (y/n): ";
            std::string konfirmasi;
            std::cin >> konfirmasi;
            if (konfirmasi != "y" && konfirmasi != "Y") {
                std::cout << "Upgrade hotel dibatalkan.\n";
                return true;
            }
        }

        try {
            const bool upgradeHotel = target->getJumlahBangunan() == 4 && !target->isHotel();
            const int biaya = upgradeHotel ? target->getHargaHotel() : target->getHargaBangunan();
            game.prosesBangun(target);
            if (upgradeHotel) {
                std::cout << target->getNama() << " di-upgrade ke Hotel!\n";
            } else {
                std::cout << "Kamu membangun 1 rumah di " << target->getNama() << ". Biaya: " << BangunCommandHelper::formatUangCommand(biaya) << "\n";
            }
            std::cout << "Uang tersisa: " << BangunCommandHelper::formatUangCommand(user.getUang()) << "\n";
            for (Street* street : selectedGroup.getStreets()) {
                BangunCommandHelper::printStreetBuildStatus(street);
            }
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
        return true;
    }
    else if (first == "GUNAKAN_KEMAMPUAN"){
        if (consecutiveDadu != 0) {
            std::cout << "Kartu kemampuan hanya bisa digunakan sebelum melempar dadu.\n";
            return true;
        }

        const std::vector<KartuSpesial*>& kartuSpesial = user.getKartuSpesial();
        if (kartuSpesial.empty()) {
            std::cout << "Kamu tidak memiliki kartu kemampuan spesial.\n";
            return true;
        }

        std::cout << "=== Kartu Kemampuan Spesial ===\n";
        for (size_t i = 0; i < kartuSpesial.size(); ++i) {
            std::cout << i + 1 << ". " << kartuSpesial[i]->getNama()
                      << " - " << kartuSpesial[i]->getDeskripsi() << "\n";
        }

        std::cout << "Pilih kartu (0 untuk batal): ";
        int pilihan;
        std::cin >> pilihan;
        while (pilihan < 0 || pilihan > static_cast<int>(kartuSpesial.size())) {
            std::cout << "Pilihan tidak valid.\n";
            std::cout << "Pilih kartu: ";
            std::cin >> pilihan;
        }
        if (pilihan == 0) {
            std::cout << "Penggunaan kartu dibatalkan.\n";
            return true;
        }

        try {
            game.prosesPakaiKartu(user, kartuSpesial[pilihan - 1]);
        } catch (const std::exception& e) {
            std::cout << "[ERROR] " << e.what() << "\n";
        }
        return true;
    }
    else if (first == "SIMPAN"){
        std::string namaFile;
        std::string extra;
        configBase config("");
        if ((iss >> namaFile) && !(iss >> extra)) {
            config.save(namaFile, game);

            game.getLogger().addLog(game.getTurn(), user.getUsername(), "Save Game", "Permainan disimpan ke file: " + namaFile);

            return true;
        }
        std::cout << "Format SIMPAN salah. Gunakan: SIMPAN <nama_file>\n";
        return true;
    }
    else if (first == "MUAT"){
        std::string namaFile;
        std::string extra;
        if ((iss >> namaFile) && !(iss >> extra)) {
            
            game.getLogger().addLog(game.getTurn(), user.getUsername(), "Load Game", "Permainan dimuat dari file: " + namaFile);

            return true;
        }
        std::cout << "Format MUAT salah. Gunakan: MUAT <nama_file>\n";
        return true;
    }
    else if (first == "CETAK_LOG"){
        std::string extra;
        if (iss.eof()) {
            // CETAK_LOG tanpa jumlah, cetak semua log
            game.getLogger().cetakLogPenuh();
            return true;
        }
        int jumlah;
        if ((iss >> jumlah) && !(iss >> extra)) {
            // CETAK_LOG dengan jumlah, cetak log sesuai jumlah
            game.getLogger().cetakLogTerbaru(jumlah);
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
