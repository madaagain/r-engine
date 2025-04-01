

#ifndef INITGAME_HPP
#define INITGAME_HPP

#include <SFML/Graphics.hpp>
// #include "Client.hpp"
#include "IGame.hpp"

class InitGame : public IState
{
    public:
        InitGame(sf::RenderWindow& window, const std::string& server_ip, unsigned short port);
        ~InitGame() override;

        void run() override;
        void processEvents() override;
        void render() override;
        void update();

    private:
        // Client _client;
        sf::RenderWindow& _window;
};

#endif // INITGAME_HPP
