
#include "Game.hpp"
#include <iostream>

int main()
{
    try {
        Game game("127.0.0.1", 8080);
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
