#ifndef GAME_HPP
#define GAME_HPP


#include "petak.hpp"
#include "user.hpp"
#include <vector>


class Game{
private:
    int MAX_TURN;
    static int turn;
    std::vector<User> pemain;
    Board board;
    std::vector<Logger> Log;
    bool end;
public:
    Game();
    ~Game();

    bool isEnd(); // Kondisi 1 Player atau Config : Bangkrut
    void setMAXTURN(int max);
    void turn();
    int getTurn();

    // Tambahan
    void leave(User& user);
};



class Board{
private:
    int size;
    std::vector<Petak*> kotak;
public: 
    Board(int size);
    ~Board();

    void inisialisasiPapan();
    Petak* getPetak(int index) const;
    void cetakPapan(const std::vector<User>& listPemain) const;
    static void tampilkanInfoPetak(Petak* p);
};

#endif