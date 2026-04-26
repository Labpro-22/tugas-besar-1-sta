#ifndef BUILDER_HPP
#define BUILDER_HPP
#include "io.hpp"
#include <memory>

class gameBuilder{
private:    
    int MAX_TURN;
    std::vector<User> pemain;
    Board board;
    std::map<std::string, PetakProperti*> lokasiKode; 
    std::map<std::string, std::vector<PetakProperti*>> lokasiColorGroup;

    std::vector<std::unique_ptr<Properti>> buildProperti(configBase* config);
    Board buildBoard(configBase* config, const std::vector<std::unique_ptr<Properti>>& sertifikat);
    std::vector<User> buildPemain(configBase* config);
    KartuSpesial* buildKartuSpesial(const std::string& jenis, int nilai);

    std::map<std::string, PetakProperti*> buildMapKodeToPetak(const Board& board);
    std::map<std::string, std::vector<PetakProperti*>> buildMapWarnaGroup(const Board& board);
public: 
    
    Game buildNewGame(configBase* config);
    Game buildLoadGame(configBase* configB, const configLoadSave* configS);
};


#endif
