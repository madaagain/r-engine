

#include "../include/InitGame.hpp"

InitGame::InitGame(sf::RenderWindow& window, const std::string& server_ip, unsigned short port)
    :  _window(window)
{
    (void) server_ip;
    (void) port;

}

InitGame::~InitGame() {
}

void InitGame::run() {
    while (_window.isOpen()) {
        processEvents();
        // update();
        render();
    }
}

void InitGame::processEvents()
{
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                _window.close();
            }
            if (event.key.code == sf::Keyboard::Space) {
            }
        }
    }
}

void InitGame::update()
{

}

void InitGame::render()
{
    _window.clear();
    _window.display();
}
