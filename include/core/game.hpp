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

    void setMAXTURN(int max);
    void turn();
    int getTurn();
};



class Board{
private:
    int size;
    std::vector<Petak*> kotak;
public: 
    Board(int size);
    ~Board();
};

#endif