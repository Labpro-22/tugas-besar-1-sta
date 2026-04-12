#ifndef KARTU_HPP
#define KARTU_HPP

#include <iostream>
#include "game.hpp"
#include "user.hpp"

class Kartu{
private:
    std::string nama;
    std::string deskripsi;
public:
    Kartu();
    ~Kartu();
    virtual void apply(Game* game, User& user) = 0;
};

class KartuAksi : public Kartu{
private:
    
public:
    KartuAksi();
    ~KartuAksi();
    void apply(Game* g, User& user) override;
};

class KartuSpesial : public Kartu{
private:
    int jenisKartu;
public:
    KartuSpesial();
    ~KartuSpesial();
    void apply(Game* g, User& user) override;
};

// Deck Kartu
template <class T>
class CardDeck{
private:
    std::vector<T> Kartu;
    std::string JenisDeck;
public:
    cardDeck();
    ~CardDeck();
};

#endif