#pragma once

#include <vector>
#include "user.hpp"
#include "properti.hpp"

class Game;

class Lelang{
private:
    Properti* targetProperti;
    std::vector<User*> peserta;
    User* highestBidder;
    int currentHighestBid;
    int consecutivePasses;
    bool isFirstBid;
    int currentPlayerIndex;
public:
    Lelang(Properti* target, Game* game, User* pemicu);
    ~Lelang() = default;

    void pass(User* user);
    void bid(User* user, int nominal);
    bool isEnd();

    User* getWinner();
    int getFinalPrice();
    Properti* getTargetProperti() const;
    std::vector<User*> getPesertaAktif();

    User* getCurrentPlayer();
    void nextTurn();

    void mulaiLelang();
};