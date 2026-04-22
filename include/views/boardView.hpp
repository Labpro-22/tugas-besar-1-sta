#ifndef BOARDVIEW_HPP
#define BOARDVIEW_HPP

#include "board.hpp"
#include "user.hpp"
#include <vector>
#include <string>

class BoardView {
public:
    static std::string getColorCode(const std::string& warna);

    static void cetakPapan(Board& board, const std::vector<User>& players, int turn, int maxTurn);
};

#endif