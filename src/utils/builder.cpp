#include "../../include/utils/builder.hpp"

std::vector<std::unique_ptr<Properti>> gameBuilder::buildProperti(configBase* config){
    std::vector<std::unique_ptr<Properti>> daftarProperti;
    std::vector<int> railroadRent;
    for (const auto& entry : config->getRailroadConfig()) {
        railroadRent.push_back(entry.second);
    }

    std::vector<int> utilityFactor;
    for (const auto& entry : config->getUtilityConfig()) {
        utilityFactor.push_back(entry.second);
    }

    for (const PropertyConfig& pro : config->getPropertyConfig()){
        if (pro.getJenis() == "STREET"){
            auto prop = std::make_unique<Street>(pro.getId(), pro.getKode(), pro.getNama(), pro.getWarna(), pro.getHargaLahan(), pro.getNilaiGadai(),
                pro.getUpgradeRumah(), pro.getUpgradeHotel(),pro.getRentLevel());
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
            auto petak = std::make_shared<PetakLahan>(
                street->getId(),
                kode,
                street->getNama(),
                "Lahan",
                street,
                street->getWarna()
            );
            board.setPetak(street->getId()-1,petak);
        } else if (RailRoad* rail = dynamic_cast<RailRoad*>(pro.get())) {
            const std::string kode = rail->getKode().empty() ? "STA" : rail->getKode();
            auto petak = std::make_shared<PetakStasiun>(
                rail->getId(),
                kode,
                rail->getNama(),
                "Stasiun",
                rail,
                rail->getWarna()
            );
            board.setPetak(rail->getId()-1,petak);
        } else if (Utility* utility = dynamic_cast<Utility*>(pro.get())) {
            const std::string kode = utility->getKode().empty() ? "UTL" : utility->getKode();
            auto petak = std::make_shared<PetakUtilitas>(
                utility->getId(),
                kode,
                utility->getNama(),
                "Utilitas",
                utility,
                utility->getWarna()
            );
            board.setPetak(utility->getId()-1,petak);
        }
    }

    // Petak Aksi
    for (const auto& pro: config->getAksiConfig()){
        if (pro.getJenis() == "Festival"){
            auto petak = std::make_shared<PetakFestival>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Festival",
                pro.getWarna()
            );

            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Dana_Umum"){
            
        }
        else if (pro.getJenis() == "Kesempatan"){

        }
        else if (pro.getJenis() == "Pajak_Barang_Mewah"){
            auto petak = std::make_shared<PetakPBM>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Pajak_Barang_Mewah",
                pro.getWarna(),
                config->getPbmFlat()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Pajak_Penghasilan"){
            auto petak = std::make_shared<PetakPPH>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Pajak_Penghasilan",
                pro.getWarna(),
                config->getPphFlat(),
                config->getPphPersentase()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Petak_Mulai"){
            auto petak = std::make_shared<PetakGo>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Petak_Mulai",
                pro.getWarna(),
                config->getGoSalary()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Penjara"){
            auto petak = std::make_shared<PetakPenjara>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Penjara",
                pro.getWarna(),
                config->getJailFine()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Bebas_Parkir"){
            auto petak = std::make_shared<PetakBebasParkir>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Bebas_Parkir",
                pro.getWarna()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else if (pro.getJenis() == "Petak_Pergi_ke_Penjara"){
            auto petak = std::make_shared<PetakPergiPenjara>(
                pro.getId(),
                pro.getKode(),
                pro.getNama(),
                "Petak_Pergi_ke_Penjara",
                pro.getWarna()
            );
            board.setPetak(pro.getId()-1,petak);
        }
        else{
            // [CHANGE] into throw
            std::cout<< "[Config Error] Jenis " << pro.getJenis() << " itu salah\n";
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

Game buildLoadGame(configLoadSave* config);
