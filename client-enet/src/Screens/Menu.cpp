

#include "../../include/Screens/Menu.hpp"
#include <iostream>

MenuState::MenuState(sf::RenderWindow& window, OptionsState &options)
    : _window(window), _options(options)
{
    generateMenu();
}

MenuState::~MenuState() {}

void MenuState::generateMenu()
{
    _backgroundTexture.loadFromFile("../../client/assets/menus/bg.png");
    _backgroundSprite.setTexture(_backgroundTexture);
    _backgroundSprite.setPosition(0, 0);
    _logoSprite.setTexture(Utility::getTexture("../../client/assets/menus/r-type-logo-clear.png"));
    sf::FloatRect logoRect = _logoSprite.getLocalBounds();
    _logoSprite.setOrigin( logoRect.left + logoRect.width  / 2.0f, logoRect.top  + logoRect.height / 2.0f );
    _logoSprite.setPosition( _window.getSize().x / 2.0f, 250.0f);

    _menuButtons.push_back(Utility::createText("Play", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f +  60, 60));
    _menuButtons.push_back(Utility::createText("Options", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f + 160, 40));
    _menuButtons.push_back(Utility::createText("Quit", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f + 260, 40));

    selectButton(0);
}

void MenuState::run()
{
    while (_window.isOpen()) {
        processEvents();
        render();
        if (isGameStart())
            _window.close();
        else if (isOptionsScene()) {
            _options.run();
            continue;
        } else if (isQuitScene()) {
            _window.close();
            exit(0);
        }
    }
}

void MenuState::processEvents()
{
    sf::Event event;
    while (_window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            _window.close();
        } else if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    _window.close();
                    break;
                case sf::Keyboard::Up:
                    moveUp();
                    break;
                case sf::Keyboard::Down:
                    moveDown();
                    break;
                case sf::Keyboard::Enter:
                    activateSelectedButton();
                    break;
                default:
                    break;
            }
        } else if (event.type == sf::Event::JoystickButtonPressed) {
            switch (event.joystickButton.button) {
                case 11:
                    moveUp();
                    break;
                case 12:
                    moveDown();
                    break;
                case 0:
                    activateSelectedButton();
                    break;
                default:
                    break;
            }
        } else if (event.type == sf::Event::JoystickMoved) {
            if (event.joystickMove.axis == sf::Joystick::PovY) {
                if (event.joystickMove.position < -50) {
                    moveUp();
                } else if (event.joystickMove.position > 50) {
                    moveDown();
                }
            }
        }
    }
}

void MenuState::update()
{
    // sf::Time elapsed = _clock.getElapsedTime();
    // float alpha = (std::sin(elapsed.asSeconds() * 3.0f) + 1.0f) / 2.0f;
    // sf::Color blinkColor(255, 255, 255, static_cast<sf::Uint8>(alpha * 255));
    // _menuButtons[0].setFillColor(blinkColor);

    for (std::size_t i = 0; i < _menuButtons.size(); i++) {
        if (static_cast<int>(i) == _selectedButton) {
            _menuButtons[i].setFillColor(sf::Color(17, 173, 160));
        } else {
            _menuButtons[i].setFillColor(sf::Color::White);
        }
    }
}

void MenuState::render()
{
    _window.clear();
    _window.draw(_backgroundSprite);    update();
    _window.draw(_logoSprite);
    for (const auto& button : _menuButtons) {
        _window.draw(button);
    }
    _window.display();
}

void MenuState::moveUp()
{
    if (_selectedButton - 1 >= 0) {
        selectButton(_selectedButton - 1);
    }
}

void MenuState::moveDown()
{
    if (static_cast<std::vector<sf::Text>::size_type>(_selectedButton + 1) < _menuButtons.size()) {
        selectButton(_selectedButton + 1);
    }
}

void MenuState::selectButton(int index)
{
    if (index >= 0 && (std::size_t)index < _menuButtons.size()) {
        _selectedButton = index;
    }
}

void MenuState::activateSelectedButton() {
    switch (_selectedButton) {
        case 0: // Play button
            _startGame = true;
            break;
        case 1: // Options button
            _optionsScene = true;
            break;
        case 2: // Quit button
            _quitScene = true;
            _window.close();
            break;
        default:
            break;
    }
}
