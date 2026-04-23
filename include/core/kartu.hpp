#ifndef KARTU_HPP
#define KARTU_HPP

#include <iostream>
#include "game.hpp"
#include "user.hpp"

class Kartu{
protected:
    std::string nama;
    std::string deskripsi;
public:
    Kartu(std::string nama, std::string deskripsi);
    virtual ~Kartu();
    std::string getNama() const;
    std::string getDeskripsi() const;
    virtual void apply(Game* game, User& user) = 0;
};

/* enum class TipeAksi{
    Kesempatan,
    Dana_Umum,
}; */

class KartuAksi : public Kartu {
public:
    KartuAksi(std::string nama, std::string deskripsi);
    virtual ~KartuAksi() = default;
};

class KartuKesempatan : public KartuAksi {
public:
    KartuKesempatan(std::string nama, std::string deskripsi);
};

class KartuDanaUmum : public KartuAksi {
public:
    KartuDanaUmum(std::string nama, std::string deskripsi);
};

// Kesempatan
class KartuStasiunTerdekat : public KartuKesempatan {
public:
    KartuStasiunTerdekat();
    void apply(Game* game, User& user) override;
};

class KartuMundurTigaPetak : public KartuKesempatan {
public:
    KartuMundurTigaPetak();
    void apply(Game* game, User& user) override;
};

class KartuMasukPenjara : public KartuKesempatan {
public:
    KartuMasukPenjara();
    void apply(Game* game, User& user) override;
};

// Dana Umum
class KartuHadiahUlangTahun : public KartuDanaUmum {
public:
    KartuHadiahUlangTahun();
    void apply(Game* game, User& user) override;
};

class KartuBiayaDokter : public KartuDanaUmum {
public:
    KartuBiayaDokter();
    void apply(Game* game, User& user) override;
};

class KartuNyaleg : public KartuDanaUmum {
public:
    KartuNyaleg();
    void apply(Game* game, User& user) override;
};

// Spesial
class KartuSpesial : public Kartu {
public:
    KartuSpesial(std::string nama, std::string deskripsi);
    virtual ~KartuSpesial() = default;
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
    DiscountCard();
    void apply(Game* game, User& user) override;
};

class ShieldCard : public KartuSpesial {
public:
    ShieldCard();
    void apply(Game* game, User& user) override;
};

class TeleportCard : public KartuSpesial {
public:
    TeleportCard();
    void apply(Game* game, User& user) override;
};

class LassoCard : public KartuSpesial {
public:
    LassoCard();
    void apply(Game* game, User& user) override;
};

class DemolitionCard : public KartuSpesial {
public:
    DemolitionCard();
    void apply(Game* game, User& user) override;
};

// CardDeck untuk mengelola kartu aksi dan kartu spesial
template <class T>
class CardDeck {
private:
    std::vector<T*> drawPile;
    std::vector<T*> discardPile;
public:
    CardDeck() {}
    ~CardDeck() {
        for(T* card : drawPile) delete card;
        for(T* card : discardPile) delete card;
    }

    void addCard(T* card) {
        drawPile.push_back(card);
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(drawPile.begin(), drawPile.end(), g);
    }

    T* draw() {
        if (drawPile.empty()) {
            if (discardPile.empty()) return nullptr; // Deck benar-benar kosong
            std::cout << "[DECK] Tumpukan habis, mengocok ulang dari kartu buangan...\n";
            drawPile = discardPile;
            discardPile.clear();
            shuffle();
        }
        T* drawnCard = drawPile.back();
        drawPile.pop_back();
        return drawnCard;
    }

    void discard(T* card) {
        if(card != nullptr) {
            discardPile.push_back(card);
        }
    }
};

CardDeck<KartuKesempatan>& getKesempatanDeck();
CardDeck<KartuDanaUmum>& getDanaUmumDeck();
CardDeck<KartuSpesial>& getSpesialDeck();

#endif