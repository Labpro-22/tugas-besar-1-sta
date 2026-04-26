#ifndef BOARDVIEW_HPP
#define BOARDVIEW_HPP

#include "core/board.hpp"
#include "core/user.hpp"
#include <string>
#include <vector>

class Game;
class Petak;
class Properti;

class BoardViewRenderer {
private:
    static constexpr int CELL_WIDTH = 10;
    static constexpr int CELL_HEIGHT = 4;
    static constexpr const char* RESET = "\033[0m";

    Board& board;
    const std::vector<User>& players;
    int turn;
    int maxTurn;
    int currentPlayerIndex;
    int rowCount;
    int colCount;
    int middleSpaceWidth;
    std::vector<std::vector<int>> layout;

    int perimeterLength(int rows, int cols) const;
    void buildLayout();
    std::string fit(const std::string& text, int width) const;
    std::string center(const std::string& text, int width) const;
    std::string colorize(const std::string& text, const std::string& color) const;
    std::string getPetakColor(Petak* petak) const;
    std::string playerCode(const User* owner) const;
    std::string buildingCode(Properti* properti) const;
    std::string propertyStatus(Petak* petak) const;
    std::string categoryCode(Petak* petak) const;
    std::string cellContent(Petak* petak, int lineIndex) const;
    std::string cellLine(int petakIndex, int lineIndex) const;
    std::string getCenterLine(int lineIndex) const;
    Properti* getProperti(Petak* petak) const;
    std::vector<std::string> pieceLines(Petak* petak) const;
    std::vector<std::string> centerContent() const;

    void printFullBorder() const;
    void printMiddleBorder(int& centerLineCounter) const;
    void printHorizontalRow(int rowIndex) const;

public:
    BoardViewRenderer(Board& board, const std::vector<User>& players, int turn = -1, int maxTurn = -1, int currentPlayerIndex = -1);
    void print() const;
};

class BoardView {
public:
    static std::string getColorCode(const std::string& warna);

    static void cetakPapan(Game& game);
    static void cetakPapan(Board& board, const std::vector<User>& players);
};

#endif
