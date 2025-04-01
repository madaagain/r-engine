
#include "Game.hpp"

Game::Game(const std::string& server_ip, unsigned short port)
    :  window_(sf::VideoMode(800, 600), "R-type")
{
    (void) server_ip;
    (void) port;
}

Game::~Game()
{
}

void Game::run()
{
    while (window_.isOpen()) {
        processEvents();
        update();
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                window_.close();
            }
            if (event.key.code == sf::Keyboard::Space) {
            }
            if (event.key.code == sf::Keyboard::Left) {
            }
            if (event.key.code == sf::Keyboard::Right) {
            }
        }
    }
}

void Game::update()
{

}

void Game::render()
{
    window_.clear();
    window_.display();
}
