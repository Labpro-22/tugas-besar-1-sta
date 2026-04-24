#include "../../include/utils/builder.hpp"
#include <algorithm>
#include <random>

std::vector<std::unique_ptr<Properti>> gameBuilder::buildProperti(configBase* config){
    std::vector<std::unique_ptr<Properti>> daftarProperti;
    std::map<std::string, int> totalStreetPerColor;
    std::vector<int> railroadRent;
    for (const auto& entry : config->getRailroadConfig()) {
        railroadRent.push_back(entry.second);
    }

    std::vector<int> utilityFactor;
    for (const auto& entry : config->getUtilityConfig()) {
        utilityFactor.push_back(entry.second);
    }

    for (const PropertyConfig& pro : config->getPropertyConfig()) {
        if (pro.getJenis() == "STREET") {
            totalStreetPerColor[pro.getWarna()]++;
        }
    }

    for (const PropertyConfig& pro : config->getPropertyConfig()){
        if (pro.getJenis() == "STREET"){
            auto prop = std::make_unique<Street>(pro.getId(), pro.getKode(), pro.getNama(), pro.getWarna(), pro.getHargaLahan(), pro.getNilaiGadai(),
                pro.getUpgradeRumah(), pro.getUpgradeHotel(),pro.getRentLevel());
            prop->setTotalDalamGrup(totalStreetPerColor[pro.getWarna()]);
            daftarProperti.push_back(std::move(prop));
        }else if (pro.getJenis() == "RAILROAD"){
            auto prop = std::make_unique<RailRoad>(pro.getId(), pro.getKode(), pro.getNama(),
                pro.getNilaiGadai(), pro.getHargaLahan(),pro.getWarna(), railroadRent);
            daftarProperti.push_back(std::move(prop));
        }else if (pro.getJenis() == "UTILITY"){
            auto prop = std::make_unique<Utility>(pro.getId(), pro.getKode(), pro.getNama(), pro.getHargaLahan(),
                pro.getNilaiGadai(), pro.getWarna(), utilityFactor);
            daftarProperti.push_back(std::move(prop));
        }else {
            // Case Else - dibuat jadi Utility aja
            auto prop = std::make_unique<Utility>(pro.getId(), pro.getKode(), pro.getNama(), pro.getHargaLahan(),
                pro.getNilaiGadai(), pro.getWarna(), utilityFactor);
            daftarProperti.push_back(std::move(prop));
        }
    }

    return daftarProperti;
}

Board gameBuilder::buildBoard(configBase* config, const std::vector<std::unique_ptr<Properti>>& sertifikat){
    // Buat board dengan size property config + aksi config
    Board board(config->getPropertyConfig().size() + config->getAksiConfig().size());

    if (board.getSize()<20 || board.getSize()>60) {
        // GAGAL NGEBUAT!!! "Throw"
    }

    // Petak Properti
    for (const auto& pro : sertifikat){
        if (Street* street = dynamic_cast<Street*>(pro.get())) {
            const std::string kode = street->getKode().empty() ? "LHN" : street->getKode();
            int petakIndex = street->getId() - 1;
            auto petak = std::make_shared<PetakLahan>(
                petakIndex,
                kode,
                street->getNama(),
                "Lahan",
                street,
                street->getWarna()
            );
            board.setPetak(petakIndex,petak);
        } else if (RailRoad* rail = dynamic_cast<RailRoad*>(pro.get())) {
            const std::string kode = rail->getKode().empty() ? "STA" : rail->getKode();
            int petakIndex = rail->getId() - 1;
            auto petak = std::make_shared<PetakStasiun>(
                petakIndex,
                kode,
                rail->getNama(),
                "Stasiun",
                rail,
                rail->getWarna()
            );
            board.setPetak(petakIndex,petak);
        } else if (Utility* utility = dynamic_cast<Utility*>(pro.get())) {
            const std::string kode = utility->getKode().empty() ? "UTL" : utility->getKode();
            int petakIndex = utility->getId() - 1;
            auto petak = std::make_shared<PetakUtilitas>(
                petakIndex,
                kode,
                utility->getNama(),
                "Utilitas",
                utility,
                utility->getWarna()
            );
            board.setPetak(petakIndex,petak);
        }
    }

    // Petak Aksi
    for (const auto& pro: config->getAksiConfig()){
        if (pro.getNama() == "Festival"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakFestival>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna()
            );

            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Dana_Umum"){
            
        }
        else if (pro.getNama() == "Kesempatan"){

        }
        else if (pro.getNama() == "Pajak_Barang_Mewah"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakPBM>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna(),
                config->getPbmFlat()
            );
            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Pajak_Penghasilan"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakPPH>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna(),
                config->getPphFlat(),
                config->getPphPersentase()
            );
            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Petak_Mulai"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakGo>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna(),
                config->getGoSalary()
            );
            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Penjara"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakPenjara>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna(),
                config->getJailFine()
            );
            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Bebas_Parkir"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakBebasParkir>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna()
            );
            board.setPetak(petakIndex,petak);
        }
        else if (pro.getNama() == "Petak_Pergi_ke_Penjara"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakPergiPenjara>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna()
            );
            board.setPetak(petakIndex,petak);
        }
        else{
            // [CHANGE] into throw
            std::cout<< "[Config Error] Jenis " << pro.getNama() << " itu salah\n";
        }

    }
    return board;
}

std::vector<User> gameBuilder::buildPemain(configBase* config){
    std::string n;
    std::string name;

    std::cout << "=== WAKTUNYA MEMILIH PEMAIN ===" << std::endl;
    std::cout << "Masukkan Jumlah Pemain : ";
    std::cin >> n;
    std::cout << "\n";
    while (n != "2" && n != "3" && n != "4"){
        std::cout << "Masukkan Jumlah Pemain : ";
        std::cin >> n;
        std::cout << "\n";
    }
    std::vector<User> pemain;
    for (int i=0;i<std::stoi(n);i++){
        std::cout << "Masukkan Nama Pemain " << i << ": ";
        std::cin >> name;
        std::cout << "\n";
        User user(name,config->getSaldoAwal());
        pemain.push_back(user);
    }
    std::random_device rd;
    std::mt19937 generator(rd());
    std::shuffle(pemain.begin(), pemain.end(), generator);
    return pemain;
}

std::map<std::string, PetakProperti*> gameBuilder::buildMapKodeToPetak(const Board& board){
    std::map<std::string, PetakProperti*> result;
    for (int i = 0; i < board.getSize(); ++i) {
        Petak* petak = board.getPetakAt(i);
        if (petak == nullptr) {
            continue;
        }

        PetakProperti* petakProperti = dynamic_cast<PetakProperti*>(petak);
        if (petakProperti == nullptr) {
            continue;
        }

        const std::string kode = petakProperti->getKodePetak();
        if (!kode.empty()) {
            result[kode] = petakProperti;
        }
    }
    return result;
}

std::map<std::string, std::vector<PetakProperti*>> gameBuilder::buildMapWarnaGroup(const Board& board){
    std::map<std::string, std::vector<PetakProperti*>> result;
    for (int i = 0; i < board.getSize(); ++i) {
        Petak* petak = board.getPetakAt(i);
        if (petak == nullptr) {
            continue;
        }

        PetakProperti* petakProperti = dynamic_cast<PetakProperti*>(petak);
        if (petakProperti == nullptr) {
            continue;
        }

        const std::string warna = petakProperti->getWarna();
        if (!warna.empty()) {
            result[warna].push_back(petakProperti);
        }
    }
    return result;
}


// === Game Builder ===
Game gameBuilder::buildNewGame (configBase* config){
    // Inisialisasi Config
    std::vector<std::unique_ptr<Properti>> daftarProperti = buildProperti(config);
    Board board = buildBoard(config,daftarProperti);
    this->lokasiKode = buildMapKodeToPetak(board);
    this->lokasiColorGroup = buildMapWarnaGroup(board);
    std::vector<User> pemain = buildPemain(config);

    // Atribut game
    Dadu dadu;
    int MAX_TURN = config->getMaxTurn();
    int SALDO_AWAL = config->getSaldoAwal();

    // Mapping
    std::map<std::string, PetakProperti*> lokasiKode = buildMapKodeToPetak(board);
    std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup = buildMapWarnaGroup(board);

    Game game(MAX_TURN,0,false,pemain,std::move(daftarProperti),board,dadu,lokasiKode,lokasiColorGroup);
    return game;
}

Game gameBuilder::buildLoadGame(configBase* configB, configLoadSave* configS){
    (void) configB;
    (void) configS;
    return Game();
}
