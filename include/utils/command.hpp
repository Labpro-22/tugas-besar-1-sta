#ifndef COMMAND_HPP
#define COMMAND_HPP
#include "core/game.hpp"
// Kelas Command untuk menjalankan perintah yang diberikan user.

class Command {
public:
    Command();
    ~Command();

    bool execute(const std::string& input, Game& game);
};


#endif