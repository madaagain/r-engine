#pragma once

#include <SFML/Graphics.hpp>

enum State
{
    MENU,
    GAMEPLAY,
    PAUSE
};

class IGameState
{
    public:
        virtual ~IGameState() = default;
        virtual void handleInput(sf::RenderWindow &win) = 0;
        virtual void update(sf::Time deltaTime) = 0;
        virtual void render(sf::RenderWindow &win) = 0;
};


//if enumstate