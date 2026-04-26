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
            throw gameException("Jenis properti tidak dikenali: " + pro.getJenis() + " untuk kode " + pro.getKode());
        }
    }

    return daftarProperti;
}

Board gameBuilder::buildBoard(configBase* config, const std::vector<std::unique_ptr<Properti>>& sertifikat){
    // Buat board dengan size property config + aksi config
    Board board(config->getPropertyConfig().size() + config->getAksiConfig().size());

    if (board.getSize()<20 || board.getSize()>60) {
        throw gameException("Ukuran papan tidak valid: " + std::to_string(board.getSize()) + ". Ukuran harus berada pada rentang 20-60 petak.");
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
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakKartu<KartuDanaUmum>>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna()
            );
            board.setPetak(petakIndex, petak);
        }
        else if (pro.getNama() == "Kesempatan"){
            int petakIndex = pro.getId() - 1;
            auto petak = std::make_shared<PetakKartu<KartuKesempatan>>(
                petakIndex,
                pro.getKode(),
                pro.getNama(),
                pro.getJenis(),
                pro.getWarna()
            );
            board.setPetak(petakIndex, petak);
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
            throw gameException("Jenis petak aksi tidak dikenali: " + pro.getNama() + " untuk kode " + pro.getKode());
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

KartuSpesial* gameBuilder::buildKartuSpesial(const std::string& jenis, int nilai) {
    if (jenis == "MoveCard") {
        MoveCard* kartu = new MoveCard();
        kartu->setLangkah(nilai);
        return kartu;
    }
    if (jenis == "DiscountCard") {
        DiscountCard* kartu = new DiscountCard();
        kartu->setPersentaseDiskon(nilai);
        return kartu;
    }
    if (jenis == "ShieldCard") {
        return new ShieldCard();
    }
    if (jenis == "TeleportCard") {
        return new TeleportCard();
    }
    if (jenis == "LassoCard") {
        return new LassoCard();
    }
    if (jenis == "DemolitionCard") {
        return new DemolitionCard();
    }
    throw gameException("Jenis kartu spesial tidak dikenali: " + jenis);
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

    // Mapping
    std::map<std::string, PetakProperti*> lokasiKode = buildMapKodeToPetak(board);
    std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup = buildMapWarnaGroup(board);

    Game game(MAX_TURN,0,false,pemain,std::move(daftarProperti),board,dadu,lokasiKode,lokasiColorGroup);
    return game;
}

Game gameBuilder::buildLoadGame(configBase* configB, const configLoadSave* configS){
    if (configB == nullptr || configS == nullptr) {
        throw gameException("Gagal load game: config base atau config load/save kosong.");
    }

    // 1. Konversi Config Base
    std::vector<std::unique_ptr<Properti>> daftarProperti = buildProperti(configB);
    Board board = buildBoard(configB, daftarProperti);
    std::map<std::string, PetakProperti*> lokasiKode = buildMapKodeToPetak(board);
    std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup = buildMapWarnaGroup(board);

    // 2A-B. Current Turn, Count Pemain, dan state pemain
    const std::vector<StatePemain>& statePemain = configS->getPemain();
    if (configS->getCountPemain() != static_cast<int>(statePemain.size())) {
        throw gameException("Gagal load game: countPemain tidak sesuai dengan data pemain.");
    }

    std::vector<User> pemain;
    pemain.reserve(statePemain.size());
    for (const StatePemain& state : statePemain) {
        User user(state.username, state.uang);
        user.setKoordinat(state.koordinat);
        user.setStatus(state.status);
        user.setJailTurns(state.jailTurns);
        user.setActiveDiscount(state.activeDiscount);
        user.setShieldActive(state.shieldActive != 0);

        std::vector<KartuSpesial*> kartuTangan;
        kartuTangan.reserve(state.kartu.size());
        for (const confKartu& kartu : state.kartu) {
            kartuTangan.push_back(buildKartuSpesial(kartu.jenis, kartu.nilai));
        }
        user.setKartuSpesial(kartuTangan);
        pemain.push_back(user);
    }

    Dadu dadu;
    Game game(
        configS->getMaxTurn(),
        configS->getCurrentTurn(),
        false,
        pemain,
        std::move(daftarProperti),
        board,
        dadu,
        lokasiKode,
        lokasiColorGroup
    );
    game.setCurrentPemainIndex(configS->getCurrentPemainIndex());
    game.setKartuSpesialSudahDibagikanGiliranIni(configS->getKartuSpesialSudahDibagikan());
    game.setKartuKemampuanSudahDipakaiGiliranIni(configS->getSudahPakaiKartuKemampuan());

    // 2C. Set Properti dari kode properti di save file
    std::vector<User>& pemainGame = game.getPemain();
    const std::vector<confProperti>& propertiLengkap = configS->getPropertiLengkap();
    if (!propertiLengkap.empty()) {
        for (const confProperti& state : propertiLengkap) {
            auto it = game.getLokasiKode().find(state.kode);
            if (it == game.getLokasiKode().end() || it->second == nullptr) {
                throw gameException("Gagal load game: kode properti tidak ditemukan: " + state.kode);
            }

            Properti* properti = it->second->getSertifikat();
            if (properti == nullptr) {
                throw gameException("Gagal load game: sertifikat properti kosong untuk kode: " + state.kode);
            }

            User* owner = nullptr;
            if (state.owner != "-") {
                auto userIt = std::find_if(pemainGame.begin(), pemainGame.end(), [&state](const User& user) {
                    return user.getUsername() == state.owner;
                });
                if (userIt == pemainGame.end()) {
                    throw gameException("Gagal load game: owner properti tidak ditemukan: " + state.owner);
                }
                owner = &(*userIt);
            }

            if (state.status < 0 || state.status > 2) {
                throw gameException("Gagal load game: status properti tidak valid untuk kode: " + state.kode);
            }

            properti->setOwner(owner);
            properti->setStatus(static_cast<PropStatus>(state.status));
            properti->setFestivalMultiplier(state.fmult);
            properti->setFestivalDuration(state.fdur);

            Street* street = dynamic_cast<Street*>(properti);
            if (street != nullptr) {
                street->setJumlahRumah(state.jumlahBangunan);
                street->setHotel(state.hasHotel != 0);
            }
        }
    } else {
        const std::vector<StateProperti>& stateProperti = configS->getProperti();
        if (stateProperti.size() != statePemain.size()) {
            throw gameException("Gagal load game: jumlah state properti tidak sesuai jumlah pemain.");
        }

        for (size_t i = 0; i < stateProperti.size(); ++i) {
            if (stateProperti[i].jumlahProperti != static_cast<int>(stateProperti[i].kodeProperti.size())) {
                throw gameException("Gagal load game: jumlah properti tidak sesuai daftar kode properti.");
            }

            for (const std::string& kode : stateProperti[i].kodeProperti) {
                auto it = game.getLokasiKode().find(kode);
                if (it == game.getLokasiKode().end() || it->second == nullptr) {
                    throw gameException("Gagal load game: kode properti tidak ditemukan: " + kode);
                }

                Properti* properti = it->second->getSertifikat();
                if (properti == nullptr) {
                    throw gameException("Gagal load game: sertifikat properti kosong untuk kode: " + kode);
                }

                properti->setOwner(&pemainGame[i]);
                properti->setStatus(PropStatus::OWNED);
            }
        }
    }

    // 2D. Set deckKartuSpesial
    std::vector<KartuSpesial*> deckKartu;
    std::vector<KartuSpesial*> discardKartu;
    const StateDeck& stateDeck = configS->getDeckKartuSpesial();
    const bool deckSpesialTerpisah = !stateDeck.drawJenis.empty() || !stateDeck.discardJenis.empty();
    const std::vector<std::string>& drawSpesial = deckSpesialTerpisah ? stateDeck.drawJenis : stateDeck.jenis;
    if (stateDeck.jumlahKartuDeck != static_cast<int>(drawSpesial.size() + stateDeck.discardJenis.size())) {
        throw gameException("Gagal load game: jumlah deck kartu spesial tidak sesuai.");
    }
    deckKartu.reserve(drawSpesial.size());
    for (const std::string& jenis : drawSpesial) {
        deckKartu.push_back(buildKartuSpesial(jenis, 0));
    }
    discardKartu.reserve(stateDeck.discardJenis.size());
    for (const std::string& jenis : stateDeck.discardJenis) {
        discardKartu.push_back(buildKartuSpesial(jenis, 0));
    }
    game.getDeckKartuSpesial().setPiles(deckKartu, discardKartu);

    // 2E. Set Log
    const StateLog& stateLog = configS->getLog();
    if (stateLog.jumlahLog != static_cast<int>(stateLog.log.size())) {
        throw gameException("Gagal load game: jumlah log tidak sesuai.");
    }

    std::vector<Logger> loggers;
    if (!stateLog.log.empty()) {
        Logger logger;
        for (const confLog& entry : stateLog.log) {
            logger.addLog(entry.turn, entry.username, entry.jenisAksi, entry.detail);
        }
        loggers.push_back(logger);
    }
    game.setLog(loggers);

    return game;
}
