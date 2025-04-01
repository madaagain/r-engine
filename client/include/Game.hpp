
#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <string>
// #include "Client.hpp"

class Game {
    public:
        Game(const std::string& server_ip, unsigned short port);
        ~Game();

        void run();

    private:
        void processEvents();
        void update();
        void render();

        // Client client_;
        sf::RenderWindow window_;
};

#endif // GAME_HPP
