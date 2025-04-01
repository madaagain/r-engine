

#ifndef OPTIONSSCENE_HPP
#define OPTIONSSCENE_HPP

#include "IScenesState.hpp"
#include "../Utils/Utils.hpp"
#include "../../renderer/Renderer.hpp"
#include <unordered_map>

class OptionsState : public IScenesState
{
    public:
        OptionsState(sf::RenderWindow &window, std::shared_ptr<SFMLRenderer> &renderer);
        ~OptionsState() override;

        void run() override;
        void processEvents() override;
        void render() override;
        void update() override;

        bool isReturnToMenu() const { return _returnToMenu; }

        void moveUp();
        void moveDown();
        void generateMenu();
        void activateSelectedButton();
        void remapKey();  // New function for key remapping
        void remapControllerButton(); // New function for controller button remapping
        std::string keyToString(sf::Keyboard::Key key);
        sf::Keyboard::Key stringToKey(const std::string &key);
        std::string controllerButtonToString(unsigned int button);
        void remapKey(const std::string &action);

    private:
        sf::RenderWindow &_window;
        std::shared_ptr<SFMLRenderer> &_renderer;
        std::vector<sf::Text> _optionsButtons;
        int _selectedButton = 1;
        bool _returnToMenu = false;
        bool _soundOn = true;
        bool _graphicsHigh = true;

        bool _awaitingRemap = false;
        bool _waitingForControllerInput = false;
        std::string _currentRemapKey;
};

#endif // OPTIONSSCENE_HPP
