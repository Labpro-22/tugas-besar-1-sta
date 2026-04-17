#pragma once

#include <vector>
#include "user.hpp"
#include "properti.hpp"

class Lelang{
private:
    Properti* targetProperti;
    std::vector<User*> peserta;
    User* highestBidder;
    int currentHighestBid;
public:
    Lelang(Properti* target, std::vector<User*> semuaPemain);
    ~Lelang() = default;
    void pass(User* user);
    void bid(User* user, int nominal);
    bool isEnd();
    User* getWinner();
    int getFinalPrice();
    Properti* getTargetProperti() const;
    std::vector<User*> getPesertaAktif();
};