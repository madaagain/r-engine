

#ifndef MENUSCENE_HPP
#define MENUSCENE_HPP

#include "IScenesState.hpp"
#include "Options.hpp"
#include "../Utils/Utils.hpp"

class MenuState : public IScenesState
{
    public:
        MenuState(sf::RenderWindow &window, OptionsState &options);
        ~MenuState() override;

        void run() override;
        void processEvents() override;
        void render() override;
        void update() override;

        bool isGameStart() const { return _startGame; }
        bool isOptionsScene() const { return _optionsScene; }
        bool isQuitScene() const { return _quitScene; }

        void moveUp();
        void moveDown();
        void generateMenu();
        void selectButton(int index);
        void activateSelectedButton();
    private:
        sf::RenderWindow &_window;
        OptionsState &_options;
        sf::Sprite _logoSprite;
        std::vector<sf::Text> _menuButtons;
        sf::Texture _backgroundTexture;
        sf::Sprite _backgroundSprite;
        sf::Clock _clock;
        bool _startGame = false;
        bool _optionsScene = false;
        bool _quitScene = false;
        int  _selectedButton = 0;
};

#endif // MENUSCENE_HPP
