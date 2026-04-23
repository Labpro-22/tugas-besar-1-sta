#ifndef BOARDVIEW_HPP
#define BOARDVIEW_HPP

#include "core/board.hpp"
#include "core/user.hpp"
#include <vector>
#include <string>

class BoardView {
public:
    static std::string getColorCode(const std::string& warna);

    static void cetakPapan(Board& board, const std::vector<User>& players);
};

#endif
