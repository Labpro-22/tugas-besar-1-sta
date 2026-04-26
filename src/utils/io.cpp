#include "../../include/utils/io.hpp"
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace {
class ConfigFileOpenException : public gameException {
public:
    explicit ConfigFileOpenException(const std::string& msg) : gameException(msg) {}
};

class ConfigLoadFormatException : public gameException {
public:
    explicit ConfigLoadFormatException(const std::string& msg) : gameException(msg) {}
};

bool isIntegerToken(const std::string& token) {
    if (token.empty()) {
        return false;
    }

    size_t start = (token[0] == '-' || token[0] == '+') ? 1 : 0;
    if (start >= token.size()) {
        return false;
    }

    for (size_t i = start; i < token.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(token[i]))) {
            return false;
        }
    }
    return true;
}

std::ifstream openFileOrThrow(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw ConfigFileOpenException("Gagal membuka file: " + filePath);
    }
    return file;
}

void ensureParentDirectoryExists(const std::string& filePath) {
    std::filesystem::path path(filePath);
    std::filesystem::path parentPath = path.parent_path();
    if (!parentPath.empty()) {
        std::filesystem::create_directories(parentPath);
    }
}

std::string trim(const std::string& raw) {
    size_t start = 0;
    while (start < raw.size() && std::isspace(static_cast<unsigned char>(raw[start]))) {
        ++start;
    }

    size_t end = raw.size();
    while (end > start && std::isspace(static_cast<unsigned char>(raw[end - 1]))) {
        --end;
    }

    return raw.substr(start, end - start);
}

std::vector<std::string> splitTokens(const std::string& line) {
    std::istringstream ss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

int parseIntStrict(const std::string& token, const std::string& field, const std::string& filePath, size_t lineNo) {
    size_t parsedChar = 0;
    int value = 0;
    try {
        value = std::stoi(token, &parsedChar);
    } catch (const std::exception&) {
        throw ConfigLoadFormatException(
            "Format integer tidak valid untuk field '" + field + "' di " + filePath +
            " baris " + std::to_string(lineNo) + "."
        );
    }

    if (parsedChar != token.size()) {
        throw ConfigLoadFormatException(
            "Format integer tidak valid untuk field '" + field + "' di " + filePath +
            " baris " + std::to_string(lineNo) + "."
        );
    }

    return value;
}

std::string readNextNonEmptyLine(std::ifstream& file, size_t& lineNo, const std::string& filePath, const std::string& expectedField) {
    std::string line;
    while (std::getline(file, line)) {
        ++lineNo;
        if (!trim(line).empty()) {
            return line;
        }
    }

    throw ConfigLoadFormatException(
        "Format file load tidak lengkap. Field '" + expectedField + "' tidak ditemukan di " + filePath + "."
    );
}

int parseSingleIntegerLine(
    std::ifstream& file,
    size_t& lineNo,
    const std::string& filePath,
    const std::string& fieldName
) {
    const std::string line = readNextNonEmptyLine(file, lineNo, filePath, fieldName);
    const std::vector<std::string> tokens = splitTokens(line);
    if (tokens.size() != 1) {
        throw ConfigLoadFormatException(
            "Field '" + fieldName + "' harus berisi tepat 1 integer di " + filePath +
            " baris " + std::to_string(lineNo) + "."
        );
    }
    return parseIntStrict(tokens[0], fieldName, filePath, lineNo);
}

std::string getKartuSaveJenis(const KartuSpesial* kartu) {
    if (dynamic_cast<const MoveCard*>(kartu) != nullptr) {
        return "MoveCard";
    }
    if (dynamic_cast<const DiscountCard*>(kartu) != nullptr) {
        return "DiscountCard";
    }
    if (dynamic_cast<const ShieldCard*>(kartu) != nullptr) {
        return "ShieldCard";
    }
    if (dynamic_cast<const TeleportCard*>(kartu) != nullptr) {
        return "TeleportCard";
    }
    if (dynamic_cast<const LassoCard*>(kartu) != nullptr) {
        return "LassoCard";
    }
    if (dynamic_cast<const DemolitionCard*>(kartu) != nullptr) {
        return "DemolitionCard";
    }
    return "UnknownCard";
}

int getKartuSaveNilai(const KartuSpesial* kartu) {
    if (const MoveCard* moveCard = dynamic_cast<const MoveCard*>(kartu)) {
        return moveCard->getLangkah();
    }
    if (const DiscountCard* discountCard = dynamic_cast<const DiscountCard*>(kartu)) {
        return discountCard->getPersentaseDiskon();
    }
    return 0;
}

void appendDeckState(StateDeck& state, const std::vector<KartuSpesial*>& cards) {
    for (const KartuSpesial* kartu : cards) {
        if (kartu != nullptr) {
            state.jenis.push_back(getKartuSaveJenis(kartu));
        }
    }
}
}

configBase::configBase(std::string path)
    : path(std::move(path)),
      pphFlat(0),
      pphPersentase(0),
      pbmFlat(0),
      goSalary(0),
      jailFine(0),
	      MAX_TURN(0),
	      SALDO_AWAL(0),
	      confLoadSave(nullptr) {
    if (this->path.empty()) {
        return;
    }

    std::string line;
    // 1. Property.txt
    std::ifstream propFile = openFileOrThrow(this->path + "/property.txt");
    while (std::getline(propFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        if (tokens.size() < 7) continue;
        if (!(tokens[0][0] >= '0' && tokens[0][0] <= '9')) continue;

        int id = std::stoi(tokens[0]);
        std::string kode = tokens[1];
        std::string nama = tokens[2];
        std::string jenis = tokens[3];
        std::string warna = tokens[4];
        int hargaLahan = std::stoi(tokens[5]);
        int nilaiGadai = std::stoi(tokens[6]);
        int upgradeRumah = (tokens.size() > 7 ? std::stoi(tokens[7]) : 0);
        int upgradeHotel = (tokens.size() > 8 ? std::stoi(tokens[8]) : 0);
        std::vector<int> rentLevel;
        for (size_t i = 9; i < tokens.size(); ++i) rentLevel.push_back(std::stoi(tokens[i]));

        PropertyConfig property(id, kode, nama, jenis, warna, hargaLahan, nilaiGadai, upgradeRumah, upgradeHotel, rentLevel);
        confProperty.push_back(property);
    }

    // 2. Aksi.txt
    std::ifstream aksiFile = openFileOrThrow(this->path + "/aksi.txt");
    while (std::getline(aksiFile, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss >> token) {
            tokens.push_back(token);
        }
        if (tokens.size() < 5) continue;
        if (!(tokens[0][0] >= '0' && tokens[0][0] <= '9')) continue;

        int id = std::stoi(tokens[0]);
        std::string kode = tokens[1];
        std::string nama = tokens[2];
        std::string jenis = tokens[3];
        std::string warna = tokens[4];

        AksiConfig aksi(id, kode, nama, jenis, warna);
        confAksi.push_back(aksi);
    }

    // 3. Railroad.txt
    std::ifstream railFile = openFileOrThrow(this->path + "/railroad.txt");
    while (std::getline(railFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        confRailroad.insert({std::stoi(tokens[0]), std::stoi(tokens[1])});
    }

    // 4. Utility.txt
    std::ifstream utilFile = openFileOrThrow(this->path + "/utility.txt");
    while (std::getline(utilFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        confUtility.insert({std::stoi(tokens[0]), std::stoi(tokens[1])});
    }

    // 5. tax.txt
    std::ifstream taxFile = openFileOrThrow(this->path + "/tax.txt");
    while (std::getline(taxFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 3 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1]) || !isIntegerToken(tokens[2])) continue;
        pphFlat = std::stoi(tokens[0]);
        pphPersentase = std::stoi(tokens[1]);
        pbmFlat = std::stoi(tokens[2]);
    }

    // 6. special.txt
    std::ifstream specFile = openFileOrThrow(this->path + "/special.txt");
    while (std::getline(specFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        goSalary = std::stoi(tokens[0]);
        jailFine = std::stoi(tokens[1]);
    }

    // 7. Misc.txt
    std::ifstream miscFile = openFileOrThrow(this->path + "/misc.txt");
    while (std::getline(miscFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        MAX_TURN = std::stoi(tokens[0]);
        SALDO_AWAL = std::stoi(tokens[1]);
    }
}

// === Implementasi Load Existing Game === 
void configBase::setLoadSaveConfig(const configLoadSave& config) {
    confLoadSave = std::make_unique<configLoadSave>(config);
}

void configBase::load(const std::string& pathLoad) {
    std::ifstream loadFile = openFileOrThrow(pathLoad);
    size_t lineNo = 0;

    configLoadSave loadedConfig;

    loadedConfig.setCurrentTurn(parseSingleIntegerLine(loadFile, lineNo, pathLoad, "currentTurn"));
    loadedConfig.setMaxTurn(parseSingleIntegerLine(loadFile, lineNo, pathLoad, "MAX_TURN"));

    const int countPemain = parseSingleIntegerLine(loadFile, lineNo, pathLoad, "countPemain");
    if (countPemain < 0) {
        throw ConfigLoadFormatException("countPemain tidak boleh negatif.");
    }
    loadedConfig.setCountPemain(countPemain);

    std::vector<StatePemain> pemain;
    pemain.reserve(static_cast<size_t>(countPemain));
    for (int i = 0; i < countPemain; ++i) {
        const std::string pemainLine = readNextNonEmptyLine(
            loadFile,
            lineNo,
            pathLoad,
            "state pemain ke-" + std::to_string(i + 1)
        );
        const std::vector<std::string> pemainTokens = splitTokens(pemainLine);
        if (pemainTokens.size() != 5) {
            throw ConfigLoadFormatException(
                "State pemain harus berformat '<username> <uang> <koordinat> <status> <jumlahKartu>' di " +
                pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        StatePemain state;
        state.username = pemainTokens[0];
        state.uang = parseIntStrict(pemainTokens[1], "pemain.uang", pathLoad, lineNo);
        state.koordinat = parseIntStrict(pemainTokens[2], "pemain.koordinat", pathLoad, lineNo);
        state.status = parseIntStrict(pemainTokens[3], "pemain.status", pathLoad, lineNo);
        const int jumlahKartu = parseIntStrict(pemainTokens[4], "pemain.jumlahKartu", pathLoad, lineNo);
        if (jumlahKartu < 0) {
            throw ConfigLoadFormatException("Jumlah kartu pemain tidak boleh negatif.");
        }

        state.kartu.reserve(static_cast<size_t>(jumlahKartu));
        for (int j = 0; j < jumlahKartu; ++j) {
            const std::string kartuLine = readNextNonEmptyLine(
                loadFile,
                lineNo,
                pathLoad,
                "kartu pemain ke-" + std::to_string(i + 1)
            );
            const std::vector<std::string> kartuTokens = splitTokens(kartuLine);
            if (kartuTokens.size() != 3) {
                throw ConfigLoadFormatException(
                    "Kartu pemain harus berformat '<jenis> <nilai> <sisa_durasi>' di " +
                    pathLoad + " baris " + std::to_string(lineNo) + "."
                );
            }

            confKartu kartu;
            kartu.jenis = kartuTokens[0];
            kartu.nilai = parseIntStrict(kartuTokens[1], "kartu.nilai", pathLoad, lineNo);
            kartu.sisa_durasi = parseIntStrict(kartuTokens[2], "kartu.sisa_durasi", pathLoad, lineNo);
            state.kartu.push_back(kartu);
        }

        pemain.push_back(state);
    }
    loadedConfig.setPemain(pemain);

    std::vector<StateProperti> properti;
    properti.reserve(static_cast<size_t>(countPemain));
    for (int i = 0; i < countPemain; ++i) {
        const std::string propertiLine = readNextNonEmptyLine(
            loadFile,
            lineNo,
            pathLoad,
            "state properti pemain ke-" + std::to_string(i + 1)
        );
        const std::vector<std::string> propertiTokens = splitTokens(propertiLine);
        if (propertiTokens.empty()) {
            throw ConfigLoadFormatException(
                "State properti pemain kosong di " + pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        StateProperti state;
        state.jumlahProperti = parseIntStrict(propertiTokens[0], "properti.jumlahProperti", pathLoad, lineNo);
        if (state.jumlahProperti < 0) {
            throw ConfigLoadFormatException("Jumlah properti tidak boleh negatif.");
        }
        if (propertiTokens.size() != static_cast<size_t>(state.jumlahProperti + 1)) {
            throw ConfigLoadFormatException(
                "State properti harus berformat '<jumlahProperti> <kode1> ... <kodeN>' di " +
                pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        for (size_t j = 1; j < propertiTokens.size(); ++j) {
            state.kodeProperti.push_back(propertiTokens[j]);
        }
        properti.push_back(state);
    }
    loadedConfig.setProperti(properti);

    const std::string deckLine = readNextNonEmptyLine(loadFile, lineNo, pathLoad, "deckKartuSpesial");
    const std::vector<std::string> deckTokens = splitTokens(deckLine);
    if (deckTokens.empty()) {
        throw ConfigLoadFormatException(
            "State deck kosong di " + pathLoad + " baris " + std::to_string(lineNo) + "."
        );
    }

    StateDeck deck;
    deck.jumlahKartuDeck = parseIntStrict(deckTokens[0], "deck.jumlahKartuDeck", pathLoad, lineNo);
    if (deck.jumlahKartuDeck < 0) {
        throw ConfigLoadFormatException("Jumlah kartu deck tidak boleh negatif.");
    }
    if (deckTokens.size() != static_cast<size_t>(deck.jumlahKartuDeck + 1)) {
        throw ConfigLoadFormatException(
            "State deck harus berformat '<jumlahKartuDeck> <jenis1> ... <jenisN>' di " +
            pathLoad + " baris " + std::to_string(lineNo) + "."
        );
    }
    for (size_t i = 1; i < deckTokens.size(); ++i) {
        deck.jenis.push_back(deckTokens[i]);
    }
    loadedConfig.setDeckKartuSpesial(deck);

    const int jumlahLog = parseSingleIntegerLine(loadFile, lineNo, pathLoad, "log.jumlahLog");
    if (jumlahLog < 0) {
        throw ConfigLoadFormatException("Jumlah log tidak boleh negatif.");
    }

    StateLog logState;
    logState.jumlahLog = jumlahLog;
    logState.log.reserve(static_cast<size_t>(jumlahLog));
    for (int i = 0; i < jumlahLog; ++i) {
        const std::string logLine = readNextNonEmptyLine(
            loadFile,
            lineNo,
            pathLoad,
            "log entry ke-" + std::to_string(i + 1)
        );

        std::istringstream logStream(logLine);
        confLog entry;
        if (!(logStream >> entry.turn >> entry.username >> entry.jenisAksi)) {
            throw ConfigLoadFormatException(
                "Log harus berformat '<turn> <username> <jenisAksi> <detail>' di " +
                pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        std::getline(logStream, entry.detail);
        entry.detail = trim(entry.detail);
        logState.log.push_back(entry);
    }
    loadedConfig.setLog(logState);

    setLoadSaveConfig(loadedConfig);

    // MAX_TURN ada di configLoadSave, jadi harus overwrite config base.
    MAX_TURN = loadedConfig.getMaxTurn();
}
    
void configBase::save(const std::string &pathSave , const Game& game){
    configLoadSave savedConfig;

    savedConfig.setCurrentTurn(game.getTurn());
    savedConfig.setMaxTurn(game.getMaxTurn());

    const std::vector<User>& pemainGame = game.getPemain();
    savedConfig.setCountPemain(static_cast<int>(pemainGame.size()));

    std::vector<StatePemain> pemain;
    pemain.reserve(pemainGame.size());
    for (const User& user : pemainGame) {
        StatePemain state;
        state.username = user.getUsername();
        state.uang = user.getUang();
        state.koordinat = user.getKoordinat();
        state.status = user.getStatus();

        for (const KartuSpesial* kartu : user.getKartuSpesial()) {
            if (kartu == nullptr) {
                continue;
            }

            confKartu kartuState;
            kartuState.jenis = getKartuSaveJenis(kartu);
            kartuState.nilai = getKartuSaveNilai(kartu);
            kartuState.sisa_durasi = 0;
            state.kartu.push_back(kartuState);
        }

        pemain.push_back(state);
    }
    savedConfig.setPemain(pemain);

    std::vector<StateProperti> properti;
    properti.reserve(pemainGame.size());
    for (const User& user : pemainGame) {
        StateProperti state;
        for (const Properti* prop : user.getListProperti()) {
            if (prop != nullptr) {
                state.kodeProperti.push_back(prop->getKode());
            }
        }
        state.jumlahProperti = static_cast<int>(state.kodeProperti.size());
        properti.push_back(state);
    }
    savedConfig.setProperti(properti);

    StateDeck deck;
    const CardDeck<KartuSpesial>& deckGame = game.getDeckKartuSpesial();
    appendDeckState(deck, deckGame.getDrawPile());
    appendDeckState(deck, deckGame.getDiscardPile());
    deck.jumlahKartuDeck = static_cast<int>(deck.jenis.size());
    savedConfig.setDeckKartuSpesial(deck);

    StateLog logState;
    for (const LogEntry& entry : game.getLogger().getLogs()) {
            confLog logEntry;
            logEntry.turn = entry.getTurn();
            logEntry.username = entry.getUsername();
            logEntry.jenisAksi = entry.getJenisAksi();
            logEntry.detail = entry.getDetail();
            logState.log.push_back(logEntry);
    }
    logState.jumlahLog = static_cast<int>(logState.log.size());
    savedConfig.setLog(logState);

    setLoadSaveConfig(savedConfig);
    
    ensureParentDirectoryExists(pathSave);
    std::ofstream saveFile(pathSave);
    if (!saveFile.is_open()) {
        throw ConfigFileOpenException("Gagal membuka file: " + pathSave);
    }

    const configLoadSave& output = *confLoadSave;
    saveFile << output.getCurrentTurn() << '\n';
    saveFile << output.getMaxTurn() << '\n';
    saveFile << output.getCountPemain() << '\n';

    for (const StatePemain& state : output.getPemain()) {
        saveFile << state.username << ' '
                 << state.uang << ' '
                 << state.koordinat << ' '
                 << state.status << ' '
                 << state.kartu.size() << '\n';

        for (const confKartu& kartu : state.kartu) {
            saveFile << kartu.jenis << ' '
                     << kartu.nilai << ' '
                     << kartu.sisa_durasi << '\n';
        }
    }

    for (const StateProperti& state : output.getProperti()) {
        saveFile << state.jumlahProperti;
        for (const std::string& kode : state.kodeProperti) {
            saveFile << ' ' << kode;
        }
        saveFile << '\n';
    }

    const StateDeck& deckOutput = output.getDeckKartuSpesial();
    saveFile << deckOutput.jumlahKartuDeck;
    for (const std::string& jenis : deckOutput.jenis) {
        saveFile << ' ' << jenis;
    }
    saveFile << '\n';

    const StateLog& logOutput = output.getLog();
    saveFile << logOutput.jumlahLog << '\n';
    for (const confLog& log : logOutput.log) {
        saveFile << log.turn << ' '
                 << log.username << ' '
                 << log.jenisAksi << ' '
                 << log.detail << '\n';
    }
}
