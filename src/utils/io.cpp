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

std::string getPropertiSaveJenis(const Properti* properti) {
    if (dynamic_cast<const Street*>(properti) != nullptr) {
        return "STREET";
    }
    if (dynamic_cast<const RailRoad*>(properti) != nullptr) {
        return "RAILROAD";
    }
    if (dynamic_cast<const Utility*>(properti) != nullptr) {
        return "UTILITY";
    }
    return "UNKNOWN";
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

template <class T>
void appendDeckNames(std::vector<std::string>& output, const std::vector<T*>& cards, std::string (*getJenis)(const T*)) {
    for (const T* kartu : cards) {
        if (kartu != nullptr) {
            output.push_back(getJenis(kartu));
        }
    }
}

void setDeckPileFromTokens(StateDeck& deck, const std::vector<std::string>& tokens, const std::string& filePath, size_t lineNo) {
    if (tokens.empty()) {
        throw ConfigLoadFormatException(
            "Baris deck kartu spesial kosong di " + filePath +
            " baris " + std::to_string(lineNo) + "."
        );
    }

    if (tokens[0] == "DRAW") {
        deck.drawJenis.assign(tokens.begin() + 1, tokens.end());
    } else if (tokens[0] == "DISCARD") {
        deck.discardJenis.assign(tokens.begin() + 1, tokens.end());
    } else {
        throw ConfigLoadFormatException(
            "Baris deck kartu spesial harus diawali DRAW atau DISCARD di " +
            filePath + " baris " + std::to_string(lineNo) + "."
        );
    }
}

StateDeck buildLegacyDeckState(const std::vector<std::string>& tokens, const std::string& filePath, size_t lineNo) {
    StateDeck deck;
    const int jumlahDraw = parseIntStrict(tokens[0], "deckKartuSpesial.jumlahDraw", filePath, lineNo);
    if (jumlahDraw < 0) {
        throw ConfigLoadFormatException("Jumlah kartu deck tidak boleh negatif.");
    }

    size_t posisi = 1;
    while (posisi < tokens.size() && tokens[posisi] != "DISCARD") {
        deck.drawJenis.push_back(tokens[posisi]);
        ++posisi;
    }

    if (posisi < tokens.size() && tokens[posisi] == "DISCARD") {
        ++posisi;
        if (posisi >= tokens.size()) {
            throw ConfigLoadFormatException(
                "Marker DISCARD harus diikuti jumlah kartu discard di " +
                filePath + " baris " + std::to_string(lineNo) + "."
            );
        }

        const int jumlahDiscard = parseIntStrict(tokens[posisi], "deckKartuSpesial.jumlahDiscard", filePath, lineNo);
        if (jumlahDiscard < 0) {
            throw ConfigLoadFormatException("Jumlah kartu discard pile tidak boleh negatif.");
        }

        ++posisi;
        while (posisi < tokens.size()) {
            deck.discardJenis.push_back(tokens[posisi]);
            ++posisi;
        }

        if (deck.discardJenis.size() != static_cast<size_t>(jumlahDiscard)) {
            throw ConfigLoadFormatException(
                "Jumlah kartu discard pile tidak sesuai di " +
                filePath + " baris " + std::to_string(lineNo) + "."
            );
        }
    }

    if (deck.drawJenis.size() != static_cast<size_t>(jumlahDraw)) {
        throw ConfigLoadFormatException(
            "Jumlah kartu draw pile tidak sesuai di " +
            filePath + " baris " + std::to_string(lineNo) + "."
        );
    }

    deck.jenis = deck.drawJenis;
    deck.jenis.insert(deck.jenis.end(), deck.discardJenis.begin(), deck.discardJenis.end());
    deck.jumlahKartuDeck = static_cast<int>(deck.jenis.size());
    return deck;
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
        if (pemainTokens.size() != 5 && pemainTokens.size() != 8) {
            throw ConfigLoadFormatException(
                "State pemain harus berformat '<username> <uang> <koordinat> <status> <jumlahKartu>' atau '<username> <uang> <koordinat> <status> <jailTurns> <activeDiscount> <shieldActive> <jumlahKartu>' di " +
                pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        StatePemain state;
        state.username = pemainTokens[0];
        state.uang = parseIntStrict(pemainTokens[1], "pemain.uang", pathLoad, lineNo);
        state.koordinat = parseIntStrict(pemainTokens[2], "pemain.koordinat", pathLoad, lineNo);
        state.status = parseIntStrict(pemainTokens[3], "pemain.status", pathLoad, lineNo);
        int jumlahKartu = 0;
        if (pemainTokens.size() == 5) {
            jumlahKartu = parseIntStrict(pemainTokens[4], "pemain.jumlahKartu", pathLoad, lineNo);
        } else {
            state.jailTurns = parseIntStrict(pemainTokens[4], "pemain.jailTurns", pathLoad, lineNo);
            state.activeDiscount = parseIntStrict(pemainTokens[5], "pemain.activeDiscount", pathLoad, lineNo);
            state.shieldActive = parseIntStrict(pemainTokens[6], "pemain.shieldActive", pathLoad, lineNo);
            jumlahKartu = parseIntStrict(pemainTokens[7], "pemain.jumlahKartu", pathLoad, lineNo);
        }
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

    std::string propertiHeaderLine = readNextNonEmptyLine(
        loadFile,
        lineNo,
        pathLoad,
        "state properti"
    );
    std::vector<std::string> propertiHeaderTokens = splitTokens(propertiHeaderLine);
    if (!propertiHeaderTokens.empty() && propertiHeaderTokens[0] == "PROPERTY_STATE") {
        if (propertiHeaderTokens.size() != 2) {
            throw ConfigLoadFormatException(
                "PROPERTY_STATE harus berformat 'PROPERTY_STATE <jumlahProperti>' di " +
                pathLoad + " baris " + std::to_string(lineNo) + "."
            );
        }

        const int jumlahPropertiLengkap = parseIntStrict(propertiHeaderTokens[1], "PROPERTY_STATE.jumlahProperti", pathLoad, lineNo);
        if (jumlahPropertiLengkap < 0) {
            throw ConfigLoadFormatException("Jumlah properti lengkap tidak boleh negatif.");
        }

        std::vector<confProperti> propertiLengkap;
        propertiLengkap.reserve(static_cast<size_t>(jumlahPropertiLengkap));
        for (int i = 0; i < jumlahPropertiLengkap; ++i) {
            const std::string propertiLine = readNextNonEmptyLine(
                loadFile,
                lineNo,
                pathLoad,
                "state properti lengkap ke-" + std::to_string(i + 1)
            );
            const std::vector<std::string> tokens = splitTokens(propertiLine);
            if (tokens.size() != 8) {
                throw ConfigLoadFormatException(
                    "State properti lengkap harus berformat '<kode> <jenis> <owner> <status> <fmult> <fdur> <jumlahRumah> <hasHotel>' di " +
                    pathLoad + " baris " + std::to_string(lineNo) + "."
                );
            }

            confProperti state;
            state.kode = tokens[0];
            state.jenis = tokens[1];
            state.owner = tokens[2];
            state.status = parseIntStrict(tokens[3], "properti.status", pathLoad, lineNo);
            state.fmult = parseIntStrict(tokens[4], "properti.fmult", pathLoad, lineNo);
            state.fdur = parseIntStrict(tokens[5], "properti.fdur", pathLoad, lineNo);
            state.jumlahBangunan = parseIntStrict(tokens[6], "properti.jumlahBangunan", pathLoad, lineNo);
            state.hasHotel = parseIntStrict(tokens[7], "properti.hasHotel", pathLoad, lineNo);
            propertiLengkap.push_back(state);
        }
        loadedConfig.setPropertiLengkap(propertiLengkap);
    } else {
        std::vector<StateProperti> properti;
        properti.reserve(static_cast<size_t>(countPemain));
        for (int i = 0; i < countPemain; ++i) {
            std::vector<std::string> propertiTokens;
            if (i == 0) {
                propertiTokens = propertiHeaderTokens;
            } else {
                const std::string propertiLine = readNextNonEmptyLine(
                    loadFile,
                    lineNo,
                    pathLoad,
                    "state properti pemain ke-" + std::to_string(i + 1)
                );
                propertiTokens = splitTokens(propertiLine);
            }

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
    }

    const std::string deckLine = readNextNonEmptyLine(loadFile, lineNo, pathLoad, "deckKartuSpesial");
    const std::vector<std::string> deckTokens = splitTokens(deckLine);
    StateDeck deck;
    if (!deckTokens.empty() && (deckTokens[0] == "DRAW" || deckTokens[0] == "DISCARD")) {
        setDeckPileFromTokens(deck, deckTokens, pathLoad, lineNo);

        const std::string secondDeckLine = readNextNonEmptyLine(loadFile, lineNo, pathLoad, "deckKartuSpesial pile kedua");
        const std::vector<std::string> secondDeckTokens = splitTokens(secondDeckLine);
        setDeckPileFromTokens(deck, secondDeckTokens, pathLoad, lineNo);

        deck.jenis = deck.drawJenis;
        deck.jenis.insert(deck.jenis.end(), deck.discardJenis.begin(), deck.discardJenis.end());
        deck.jumlahKartuDeck = static_cast<int>(deck.jenis.size());
    } else {
        deck = buildLegacyDeckState(deckTokens, pathLoad, lineNo);
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

    std::string extraLine;
    if (std::getline(loadFile, extraLine)) {
        ++lineNo;
        extraLine = trim(extraLine);
        if (!extraLine.empty()) {
            const std::vector<std::string> tokens = splitTokens(extraLine);
            if (tokens.size() != 1) {
                throw ConfigLoadFormatException(
                    "currentPemainIndex harus berisi tepat 1 integer di " +
                    pathLoad + " baris " + std::to_string(lineNo) + "."
                );
            }
            loadedConfig.setCurrentPemainIndex(parseIntStrict(tokens[0], "currentPemainIndex", pathLoad, lineNo));
        }
    }

    if (std::getline(loadFile, extraLine)) {
        ++lineNo;
        extraLine = trim(extraLine);
        if (!extraLine.empty()) {
            const std::vector<std::string> tokens = splitTokens(extraLine);
            if (tokens.size() != 1) {
                throw ConfigLoadFormatException(
                    "kartuSpesialSudahDibagikan harus berisi tepat 1 integer di " +
                    pathLoad + " baris " + std::to_string(lineNo) + "."
                );
            }
            loadedConfig.setKartuSpesialSudahDibagikan(
                parseIntStrict(tokens[0], "kartuSpesialSudahDibagikan", pathLoad, lineNo) != 0
            );
        }
    }

    if (std::getline(loadFile, extraLine)) {
        ++lineNo;
        extraLine = trim(extraLine);
        if (!extraLine.empty()) {
            const std::vector<std::string> tokens = splitTokens(extraLine);
            if (tokens.size() != 1) {
                throw ConfigLoadFormatException(
                    "sudahPakaiKartuKemampuan harus berisi tepat 1 integer di " +
                    pathLoad + " baris " + std::to_string(lineNo) + "."
                );
            }
            loadedConfig.setSudahPakaiKartuKemampuan(
                parseIntStrict(tokens[0], "sudahPakaiKartuKemampuan", pathLoad, lineNo) != 0
            );
        }
    }

    if (std::getline(loadFile, extraLine)) {
        extraLine.erase(0, extraLine.find_first_not_of(" \t\r\n"));
        extraLine.erase(extraLine.find_last_not_of(" \t\r\n") + 1);
        if (!extraLine.empty() && extraLine != "-") {
            loadedConfig.setDynamicMapFile(extraLine);
        }
    }

    setLoadSaveConfig(loadedConfig);

    // MAX_TURN ada di configLoadSave, jadi harus overwrite config base.
    MAX_TURN = loadedConfig.getMaxTurn();
}
    
void configBase::save(const std::string &pathSave , const Game& game){
    configLoadSave savedConfig;

    savedConfig.setCurrentTurn(game.getTurn());
    savedConfig.setMaxTurn(game.getMaxTurn());
    savedConfig.setCurrentPemainIndex(game.getCurrentPemainIndex());
    savedConfig.setKartuSpesialSudahDibagikan(game.isKartuSpesialSudahDibagikanGiliranIni());
    savedConfig.setSudahPakaiKartuKemampuan(game.isKartuKemampuanSudahDipakaiGiliranIni());

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
        state.jailTurns = user.getJailTurns();
        state.activeDiscount = user.getActiveDiscount();
        state.shieldActive = user.isShieldActive() ? 1 : 0;

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

    std::vector<confProperti> propertiLengkap;
    for (const std::unique_ptr<Properti>& propPtr : game.getDaftarProperti()) {
        const Properti* prop = propPtr.get();
        if (prop == nullptr) {
            continue;
        }

        confProperti state;
        state.kode = prop->getKode();
        state.jenis = getPropertiSaveJenis(prop);
        state.owner = prop->getOwner() == nullptr ? "-" : prop->getOwner()->getUsername();
        state.status = static_cast<int>(prop->getStatus());
        state.fmult = prop->getFestivalMultiplier();
        state.fdur = prop->getFestivalDuration();

        const Street* street = dynamic_cast<const Street*>(prop);
        if (street != nullptr) {
            state.jumlahBangunan = street->getJumlahBangunan();
            state.hasHotel = street->isHotel() ? 1 : 0;
        }

        propertiLengkap.push_back(state);
    }
    savedConfig.setPropertiLengkap(propertiLengkap);

    StateDeck deck;
    const CardDeck<KartuSpesial>& deckGame = game.getDeckKartuSpesial();
    appendDeckNames<KartuSpesial>(deck.drawJenis, deckGame.getDrawPile(), getKartuSaveJenis);
    appendDeckNames<KartuSpesial>(deck.discardJenis, deckGame.getDiscardPile(), getKartuSaveJenis);
    deck.jenis = deck.drawJenis;
    deck.jenis.insert(deck.jenis.end(), deck.discardJenis.begin(), deck.discardJenis.end());
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
                 << state.jailTurns << ' '
                 << state.activeDiscount << ' '
                 << state.shieldActive << ' '
                 << state.kartu.size() << '\n';

        for (const confKartu& kartu : state.kartu) {
            saveFile << kartu.jenis << ' '
                     << kartu.nilai << ' '
                     << kartu.sisa_durasi << '\n';
        }
    }

    saveFile << "PROPERTY_STATE " << output.getPropertiLengkap().size() << '\n';
    for (const confProperti& state : output.getPropertiLengkap()) {
        saveFile << state.kode << ' '
                 << state.jenis << ' '
                 << state.owner << ' '
                 << state.status << ' '
                 << state.fmult << ' '
                 << state.fdur << ' '
                 << state.jumlahBangunan << ' '
                 << state.hasHotel << '\n';
    }

    const StateDeck& deckOutput = output.getDeckKartuSpesial();
    saveFile << "DRAW";
    for (const std::string& jenis : deckOutput.drawJenis) {
        saveFile << ' ' << jenis;
    }
    saveFile << '\n';
    saveFile << "DISCARD";
    for (const std::string& jenis : deckOutput.discardJenis) {
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
    saveFile << output.getCurrentPemainIndex() << '\n';
    saveFile << (output.getKartuSpesialSudahDibagikan() ? 1 : 0) << '\n';
    saveFile << (output.getSudahPakaiKartuKemampuan() ? 1 : 0) << '\n';
    saveFile << (game.getDynamicFile().empty() ? "-" : game.getDynamicFile()) << '\n';
}
