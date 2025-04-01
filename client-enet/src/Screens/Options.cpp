

#include "../../include/Screens/Options.hpp"
#include <iostream>
#include <map>

OptionsState::OptionsState(sf::RenderWindow& window, std::shared_ptr<SFMLRenderer> &renderer)
    : _window(window), _renderer(renderer)
{
    _selectedButton = 1;
    generateMenu();
}

OptionsState::~OptionsState() {}

std::string OptionsState::keyToString(sf::Keyboard::Key key) {
    static const std::map<sf::Keyboard::Key, std::string> keyMap = {
        {sf::Keyboard::A, "A"}, {sf::Keyboard::B, "B"}, {sf::Keyboard::C, "C"},
        {sf::Keyboard::D, "D"}, {sf::Keyboard::E, "E"}, {sf::Keyboard::F, "F"},
        {sf::Keyboard::G, "G"}, {sf::Keyboard::H, "H"}, {sf::Keyboard::I, "I"},
        {sf::Keyboard::J, "J"}, {sf::Keyboard::K, "K"}, {sf::Keyboard::L, "L"},
        {sf::Keyboard::M, "M"}, {sf::Keyboard::N, "N"}, {sf::Keyboard::O, "O"},
        {sf::Keyboard::P, "P"}, {sf::Keyboard::Q, "Q"}, {sf::Keyboard::R, "R"},
        {sf::Keyboard::S, "S"}, {sf::Keyboard::T, "T"}, {sf::Keyboard::U, "U"},
        {sf::Keyboard::V, "V"}, {sf::Keyboard::W, "W"}, {sf::Keyboard::X, "X"},
        {sf::Keyboard::Y, "Y"}, {sf::Keyboard::Z, "Z"}, {sf::Keyboard::Num0, "Num0"},
        {sf::Keyboard::Num1, "Num1"}, {sf::Keyboard::Num2, "Num2"}, {sf::Keyboard::Num3, "Num3"},
        {sf::Keyboard::Num4, "Num4"}, {sf::Keyboard::Num5, "Num5"}, {sf::Keyboard::Num6, "Num6"},
        {sf::Keyboard::Num7, "Num7"}, {sf::Keyboard::Num8, "Num8"}, {sf::Keyboard::Num9, "Num9"},
        {sf::Keyboard::Escape, "Escape"}, {sf::Keyboard::LControl, "LControl"}, {sf::Keyboard::LShift, "LShift"},
        {sf::Keyboard::LAlt, "LAlt"}, {sf::Keyboard::LSystem, "LSystem"}, {sf::Keyboard::RControl, "RControl"},
        {sf::Keyboard::RShift, "RShift"}, {sf::Keyboard::RAlt, "RAlt"}, {sf::Keyboard::RSystem, "RSystem"},
        {sf::Keyboard::SemiColon, "SemiColon"}, {sf::Keyboard::Comma, "Comma"}, {sf::Keyboard::Period, "Period"},
        {sf::Keyboard::Quote, "Quote"}, {sf::Keyboard::Slash, "Slash"}, {sf::Keyboard::BackSlash, "BackSlash"},
        {sf::Keyboard::Tilde, "Tilde"}, {sf::Keyboard::Equal, "Equal"}, {sf::Keyboard::Dash, "Dash"},
        {sf::Keyboard::Space, "Space"}, {sf::Keyboard::Return, "Return"}, {sf::Keyboard::BackSpace, "BackSpace"},
        {sf::Keyboard::Tab, "Tab"}, {sf::Keyboard::PageUp, "PageUp"}, {sf::Keyboard::PageDown, "PageDown"},
        {sf::Keyboard::End, "End"}, {sf::Keyboard::Home, "Home"}, {sf::Keyboard::Insert, "Insert"},
        {sf::Keyboard::Delete, "Delete"}, {sf::Keyboard::Add, "Add"}, {sf::Keyboard::Subtract, "Subtract"},
        {sf::Keyboard::Multiply, "Multiply"}, {sf::Keyboard::Divide, "Divide"}, {sf::Keyboard::Left, "Left"},
        {sf::Keyboard::Right, "Right"}, {sf::Keyboard::Up, "Up"}, {sf::Keyboard::Down, "Down"},
        {sf::Keyboard::Numpad0, "Numpad0"}, {sf::Keyboard::Numpad1, "Numpad1"}, {sf::Keyboard::Numpad2, "Numpad2"},
        {sf::Keyboard::Numpad3, "Numpad3"}, {sf::Keyboard::Numpad4, "Numpad4"}, {sf::Keyboard::Numpad5, "Numpad5"},
        {sf::Keyboard::Numpad6, "Numpad6"}, {sf::Keyboard::Numpad7, "Numpad7"}, {sf::Keyboard::Numpad8, "Numpad8"},
        {sf::Keyboard::Numpad9, "Numpad9"}, {sf::Keyboard::F1, "F1"}, {sf::Keyboard::F2, "F2"},
        {sf::Keyboard::F3, "F3"}, {sf::Keyboard::F4, "F4"}, {sf::Keyboard::F5, "F5"},
        {sf::Keyboard::F6, "F6"}, {sf::Keyboard::F7, "F7"}, {sf::Keyboard::F8, "F8"},
        {sf::Keyboard::F9, "F9"}, {sf::Keyboard::F10, "F10"}, {sf::Keyboard::F11, "F11"},
        {sf::Keyboard::F12, "F12"}, {sf::Keyboard::F13, "F13"}, {sf::Keyboard::F14, "F14"},
        {sf::Keyboard::F15, "F15"}, {sf::Keyboard::Pause, "Pause"}
    };

    auto it = keyMap.find(key);
    if (it != keyMap.end()) {
        return it->second;
    }
    return "Unknown";
}

std::string OptionsState::controllerButtonToString(unsigned int button) {
    static const std::map<unsigned int, std::string> buttonMap = {
        {0, "A"},
        {1, "B"},
        {2, "X"},
        {3, "Y"},
        {4, "LB"},
        {5, "RB"},
        {6, "Back"},
        {7, "Start"},
        {8, "LStick"},
        {9, "RStick"},
        {10, "DPadUp"},
        {11, "DPadDown"},
        {12, "DPadLeft"},
        {13, "DPadRight"}
    };

    auto it = buttonMap.find(button);
    if (it != buttonMap.end()) {
        return it->second;
    }
    return "Unknown Button";
}

void OptionsState::generateMenu()
{
    _optionsButtons.clear();

    // Params ideas
    // std::string accessMode = _accessMd ? "Access Mode: Keyboard" : "Access Mode: Controller";
    // std:string colorBlind = _colorBlind ? "Color Blind Mode: ON" : "Color Blind Mode: OFF";
    // std::string graphics = _graphicsHigh ? "Graphics: High" : "Graphics: Low"; (change the nbr of components of the parallax)
    // (Change)

    _optionsButtons.push_back(Utility::createText("Options", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f -350.0f, 55));
    _optionsButtons.push_back(Utility::createText(_soundOn ? "Sound ON" : "Sound OFF", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f - 100, 30));
    _optionsButtons.push_back(Utility::createText("Key Bindings", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f - 25, 55));

    int yOffset = 50;
    for (const auto &[action, key] : _renderer->getKeyMappings()) {
        _optionsButtons.push_back(Utility::createText(
            action + " " + keyToString(key),
            _window.getSize().x / 2.0f,
            _window.getSize().y / 2.0f + yOffset,
            30
        ));
        yOffset += 50;
    }

    _optionsButtons.push_back(Utility::createText("Controller Shoot Button " + controllerButtonToString(_renderer->getControllerShootButton()),
                                                _window.getSize().x / 2.0f, _window.getSize().y / 2.0f + yOffset, 30));
    _optionsButtons.push_back(Utility::createText("Controller Force Button " + controllerButtonToString(_renderer->getControllerForceButton()),
                                                _window.getSize().x / 2.0f, _window.getSize().y / 2.0f + yOffset + 50, 30));
    _optionsButtons.push_back(Utility::createText("Return to Main Menu", _window.getSize().x / 2.0f, _window.getSize().y / 2.0f + 450, 30));
}

void OptionsState::run()
{
    while (_window.isOpen() && !_returnToMenu) {
        processEvents();
        render();
    }
}

void OptionsState::processEvents() {
    sf::Event event;
    // while (_window.pollEvent(event)) {
    _window.pollEvent(event);
        if (_awaitingRemap) {
            if (event.type == sf::Event::KeyPressed) {
                sf::Keyboard::Key newKey = event.key.code;
                _renderer->setKeyMappings(_currentRemapKey, newKey);
                _optionsButtons[_selectedButton].setString(_currentRemapKey + " " + keyToString(newKey));
                _awaitingRemap = false;
                std::cout << "Key remapped: " << _currentRemapKey << " -> " << keyToString(newKey) << std::endl;
            }
        } else {
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    _returnToMenu = true;
                }
                if (event.key.code == sf::Keyboard::Enter) {
                    activateSelectedButton();
                }
                if (event.key.code == sf::Keyboard::Up) {
                    moveUp();
                } else if (event.key.code == sf::Keyboard::Down) {
                    moveDown();
                }
            }else if (event.type == sf::Event::JoystickButtonPressed) {
                if (_waitingForControllerInput) {
                    if (_selectedButton == 9) {
                        _renderer->setControllerShootButton(event.joystickButton.button);
                        _optionsButtons[9].setString("Controller Shoot Button " + controllerButtonToString(event.joystickButton.button));
                    } else if (_selectedButton == 10) {
                        _renderer->setControllerForceButton(event.joystickButton.button);
                        _optionsButtons[10].setString("Controller Force Button " + controllerButtonToString(event.joystickButton.button));
                    }
                    _waitingForControllerInput = false;
                } else {
                    switch (event.joystickButton.button) {
                        case 11:
                            moveUp();
                            break;
                        case 12:
                            moveDown();
                            break;
                        case 1:
                            _returnToMenu = true;
                            break;
                        case 0:
                            activateSelectedButton();
                            break;
                        default:
                            break;
                    }
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

        if (event.type == sf::Event::Closed) {
            _window.close();
        }
    // }
}

void OptionsState::update()
{
    for (std::size_t i = 1; i < _optionsButtons.size(); i++) {
        sf::FloatRect textBounds = _optionsButtons[i].getLocalBounds();
        _optionsButtons[i].setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
        _optionsButtons[i].setPosition(_window.getSize().x / 2.0f, _optionsButtons[i].getPosition().y);

        if (static_cast<int>(i) == _selectedButton) {
            _optionsButtons[i].setFillColor(sf::Color(17, 173, 160));
        } else {
            _optionsButtons[i].setFillColor(sf::Color::White);
        }
    }
}

void OptionsState::render()
{
    _window.clear();
    update();
    for (const auto& button : _optionsButtons) {
        _window.draw(button);
    }
    _window.display();
}

void OptionsState::moveUp()
{
    if (_selectedButton - 1 >= 1)
        _selectedButton -= 1;
    else
        _selectedButton = _optionsButtons.size() - 1;
}

void OptionsState::moveDown()
{
    if (static_cast<size_t>(_selectedButton + 1) < _optionsButtons.size())
        _selectedButton += 1;
    else
        _selectedButton = 1;
}

void OptionsState::activateSelectedButton()
{
    switch (_selectedButton) {
        case 1:
            _soundOn = !_soundOn;
            _optionsButtons[1].setString(_soundOn ? "Sound ON" : "Sound OFF");
            break;
        case 3:
            _optionsButtons[3].setString("MoveDown [");
            remapKey("MoveDown");
            break;
        case 4:
            _optionsButtons[4].setString("Shoot [");
            remapKey("Shoot");
            break;
        case 5:
            _optionsButtons[5].setString("MoveUp [");
            remapKey("MoveUp");
            break;
        case 6:
            _optionsButtons[6].setString("MoveRight [");
            remapKey("MoveRight");
            break;
        case 7:
            _optionsButtons[7].setString("Force [");
            remapKey("Force");
            break;
        case 8:
            _optionsButtons[8].setString("MoveLeft [");
            remapKey("MoveLeft");
            break;
        case 9:
            _optionsButtons[9].setString("Controller Shoot Button [");
            _waitingForControllerInput = true;
            break;
        case 10:
            _optionsButtons[10].setString("Controller Force Button [");
            _waitingForControllerInput = true;
            break;
        case 11:
            _returnToMenu = true;
            break;
        default:
            break;
    }
}

void OptionsState::remapKey(const std::string &action) {
    _awaitingRemap = true;
    _currentRemapKey = action;
    std::cout << "Press a key to remap '" << action << "'." << std::endl;
}
