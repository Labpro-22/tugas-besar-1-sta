#include "../../include/utils/io.hpp"
#include <cctype>
#include <fstream>
#include <sstream>

namespace {
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
}

configBase::configBase(std::string path) : path(path) {
    std::string line;
    // 1. Property.txt
    std::ifstream propFile(this->path + "/property.txt");
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
    propFile.close();

    // 2. Aksi.txt
    std::ifstream aksiFile(this->path + "/aksi.txt");
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
    aksiFile.close();

    // 3. Railroad.txt
    std::ifstream railFile(this->path+"/railroad.txt");
    while (std::getline(railFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        confRailroad.insert({std::stoi(tokens[0]), std::stoi(tokens[1])});
    }
    railFile.close();

    // 4. Utility.txt
    std::ifstream utilFile(this->path+"/utility.txt");
    while (std::getline(utilFile,line)){
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (ss>>token) {tokens.push_back(token);}
        if (tokens.size() < 2 || !isIntegerToken(tokens[0]) || !isIntegerToken(tokens[1])) continue;
        confUtility.insert({std::stoi(tokens[0]), std::stoi(tokens[1])});
    }
    utilFile.close();

    // 5. tax.txt
    std::ifstream taxFile(this->path+"/tax.txt");
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
    taxFile.close();

    // 6. special.txt
    std::ifstream specFile(this->path+"/special.txt");
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
    specFile.close();

    // 7. Misc.txt
    std::ifstream miscFile(this->path+"/misc.txt");
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
    miscFile.close();
}



void configBase::load(const std::string& pathLoad){
}
