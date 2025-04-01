#pragma once

#include "IGameState.hpp"

class MainMenu : public IGameState
{
    public:
        MainMenu();
        void handleInput(sf::RenderWindow &win) override;
        void update(sf::Time deltaTime) override;
        void render(sf::RenderWindow &win) override;
    
    private:
        sf::Font _font;
        sf::Text _title;
};