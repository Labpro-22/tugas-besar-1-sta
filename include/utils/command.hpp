#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "core/game.hpp"
#include "views/boardView.hpp"
// Kelas Command untuk menjalankan perintah yang diberikan user.

class Command {
public:
    Command();
    ~Command();

    bool execute(User& user, const std::string& input, Game& game, int consecutiveDadu);
};


#endif