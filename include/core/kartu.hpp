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

enum class TipeAksi{
    Kesempatan,
    Dana_Umum,
};

class KartuAksi : public Kartu{
private:
    TipeAksi kategori;
public:
    KartuAksi();
    KartuAksi(TipeAksi kategori);
    ~KartuAksi();
    void apply(Game* g, User& user) override;
};

class KartuKesempatan : public KartuAksi {
public:
    void apply(Game* game, User& user) override;
};

class KartuDanaUmum : public KartuAksi {
public:
    void apply(Game* game, User& user) override;
};

class KartuSpesial : public Kartu{
private:
    int jenisKartu;
public:
    KartuSpesial();
    ~KartuSpesial();
    virtual void apply(Game* game, User& user) = 0;
};

class MoveCard : public KartuSpesial {
private:
    int langkah;
public:
    MoveCard();
    void apply(Game* game, User& user) override;
};

class DiscountCard : public KartuSpesial {
private:
    int persentasediskon;
public:
    void apply(Game* game, User& user) override;
};

class ShieldCard : public KartuSpesial {
public:
    void apply(Game* game, User& user) override;
};

class TeleportCard : public KartuSpesial {
public:
    void apply(Game* game, User& user) override;
};

class LassoCard : public KartuSpesial {
public:
    void apply(Game* game, User& user) override;
};

class DemolitionCard : public KartuSpesial {
public:
    void apply(Game* game, User& user) override;
};

// Deck Kartu [P.S. = Penggunaan Template untuk menghindari kartu yang berbeda jenis masuk ke carddeck lain.]
template <class T>
class CardDeck{
private:
    std::vector<T> Kartu;
    std::string JenisDeck;
public:
    CardDeck();
    ~CardDeck();
};

#endif