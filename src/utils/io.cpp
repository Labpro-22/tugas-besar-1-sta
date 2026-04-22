#include "io.hpp"
#include <fstream>
#include <sstream>

IO::IO(std::string path) : path(path) {}

Game IO::makeGame(){
    // 1. Propert.txt
    std::ifstream propFile(this->path);
    std::string line;
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
    // 2. Railroad.txt
    std::ifstream railFile(this->path);
    std::string line2;
    while (std::getline(railFile, line2)) {
        
    }
    
    railFile.close();
    return Game();
}






void IO::load(const std::string& pathLoad){
    // 1. Build Board
    Board *b = new Board();
    b->buildBoard(pathLoad);

    
}
