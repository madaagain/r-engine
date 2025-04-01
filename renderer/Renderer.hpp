#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <deque>

#include "Components.hpp"
#include "SFML/Audio/Sound.hpp"
#include <SFML/Audio.hpp>
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Graphics.hpp"
#include "IRenderer.hpp"
#include "../systems/headers/InputSystem.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Window/Keyboard.hpp"

class SFMLRenderer : public IRenderer
{
public:
    SFMLRenderer(int screenWidth, int screenHeight, const std::string &title)
        : _window(sf::VideoMode(screenWidth, screenHeight), title),
          _drawBoundingBoxes(false)
    {
        _window.setFramerateLimit(60);
        keyMappings = {
            {"MoveLeft", sf::Keyboard::Left},
            {"MoveRight", sf::Keyboard::Right},
            {"MoveUp", sf::Keyboard::Up},
            {"MoveDown", sf::Keyboard::Down},
            {"Shoot", sf::Keyboard::Space},
            {"Force", sf::Keyboard::F}
        };

        controllerShootButton = 0;
        controllerForceButton = 2;
    } 

    void clear() override
    {
        _window.clear(sf::Color::Black);
    }

    void present() override
    {
        _window.display();
    }

    void drawShape(const std::string &shapeType, float x, float y, float rotation, bool isVisible, float r, float g, float b, float a, float sizeX, float sizeY, float radius) override
    {
        if (!isVisible)
            return; 
        if (shapeType == "Rectangle") {
            sf::RectangleShape rectangle(sf::Vector2f(x, y));
            rectangle.setSize(sf::Vector2f(sizeX, sizeY));
            rectangle.setRotation(rotation);
            rectangle.setFillColor(sf::Color(r, g, b, a));
            _window.draw(rectangle);
        }
        if (shapeType == "Circle") {
            sf::CircleShape circle(radius);
            circle.setFillColor(sf::Color(r, g, b, a));
            _window.draw(circle);
        }
    }
    
    void drawTexture(
        const std::string &textureId, float x, float y,
        float rotation, float scaleX, float scaleY,
        int zOrder, int frame, bool isVisible,
        int width, int height, bool animated,
        float rectOriginX, float rectOriginY,
        float r, float g, float b, float a
    ) override
    {
        (void) zOrder;
        (void) frame;
        (void) animated;


        if (!isVisible) {
            return;
        }

        if (animated) {
            sf::Texture &texture = getTexture(textureId);
            sf::Sprite sprite;
            sprite.setTexture(texture);
            sprite.setTextureRect(sf::IntRect(
                static_cast<int>(rectOriginX),
                static_cast<int>(rectOriginY),
                width, height
            ));
            sprite.setColor(sf::Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a)));

            sprite.setOrigin(static_cast<float>(width) / 2.0f, static_cast<float>(height) / 2.0f);
            sprite.setPosition(x, y);
            sprite.setRotation(rotation);
            sprite.setScale(scaleX, scaleY);
            _window.draw(sprite);

            if (_drawBoundingBoxes) {
                sf::RectangleShape boundingBox(sf::Vector2f(static_cast<float>(width) * scaleX, static_cast<float>(height) * scaleY));
                boundingBox.setOrigin(static_cast<float>(width), static_cast<float>(height));
                boundingBox.setPosition(x, y);
                boundingBox.setRotation(rotation);
                boundingBox.setFillColor(sf::Color::Transparent);
                boundingBox.setOutlineColor(sf::Color::Red);
                boundingBox.setOutlineThickness(1.0f);
                _window.draw(boundingBox);
            }
        }
    }

    void toggleBoundingBoxes(bool enable) override
    {
        _drawBoundingBoxes = enable;
    }

    void addParallaxLayer(
        const std::string &textureId, 
        float speedX,
        float speedY,
        int zLayer
        ) override
    {
            try {
                _parallaxLayers.emplace_back(textureId, speedX, speedY, zLayer);
                std::sort(_parallaxLayers.begin(), _parallaxLayers.end(),
                [](const ParallaxScroller &a, const ParallaxScroller &b){
                    return a.zLayer < b.zLayer;
                });
            } catch (const std::exception &e) {
                std::cerr << "Error adding parllax layer -> " << e.what() << std::endl;
            }
    }

    void initShaders(){}

    void drawParallaxLayers(
        const std::string &textureId,
        float &offsetX,
        float &offsetY,
        float speedX,
        float speedY,
        int zLayer,
        float &rotation,
        float scale,
        float dt,
        bool isVisible,
        bool isAnimated,
        int currentFrame
    ) override
    {
        (void) dt;

        if (!isVisible)
            return;
        ParallaxScroller data{ textureId, speedX, speedY, zLayer, offsetX, offsetY, rotation, scale, isVisible, isAnimated, currentFrame };
        _parallaxLayers.push_back(data);
        std::sort(_parallaxLayers.begin(), _parallaxLayers.end(),
                [](const ParallaxScroller &a, const ParallaxScroller &b) {
                    return a.scrollSpeedX < b.scrollSpeedX;
                });

        for (auto &layer : _parallaxLayers) {
            sf::Texture &texture = getTexture(layer.textureId);
            texture.setRepeated(true);
            sf::Sprite sprite;
            sprite.setTexture(texture);
            sprite.setPosition(layer.offsetX, layer.offsetY);
            sprite.setScale(scale, scale);
            sprite.setColor(sf::Color(255, 255, 255, 255));
            sprite.setRotation(static_cast<float>(layer.rotation));

            if (layer.isAnimated) {
                sprite.setTextureRect(sf::IntRect(0 + (100 * layer.currentFrame), 0, 100, 100));
            }

            _window.draw(sprite);
        }
        _parallaxLayers.clear();
    }

    void drawText(
        const std::string &text,
        const std::string &fontPath,
        int fontSize,
        int x,
        int y,
        int zOrder,
        bool visible,
        float a,
        float r,
        float g,
        float b
    ) override
    {
        (void) zOrder;

        if (!visible)
            return;

        sf::Font &font = getFont(fontPath);

        sf::Text sfText;
        sfText.setFont(font);
        sfText.setString(text);
        sfText.setCharacterSize(static_cast<unsigned int>(fontSize));
        sfText.setFillColor(sf::Color(static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), static_cast<uint8_t>(a)));
        sfText.setPosition(static_cast<float>(x), static_cast<float>(y));

        _window.draw(sfText);
    }

    bool isOpen() const 
    {
        return _window.isOpen();
    }

    void handleEvents()
    {
        sf::Event event;
        while (_window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                _window.close();
            else if (event.type == sf::Event::LostFocus)
                focus = false;
            else if (event.type == sf::Event::GainedFocus)
                focus = true;
        }
    }

    void playSounds(const std::string &soundFile, float volume, float pitch, bool isPlaying, bool isLoop) override
    {
        if (!isPlaying) {
            return; 
        }
        sf::SoundBuffer &buffer = getSounds(soundFile);
        auto sound = std::make_unique<sf::Sound>();
        sound->setBuffer(buffer);
        sound->setVolume(volume);
        sound->setPitch(pitch);
        sound->setLoop(isLoop);
        if (isPlaying) {
            sound->play();
            _activeSounds.push_back(std::move(sound));
        }
    }

    void playMusic(const std::string &musicFile, float volume, bool isPlaying, bool isLoop) override
    {
        for (auto &music : _activeMusic) {
            if (!isPlaying) {
                music->stop();
                return;
            }
            if (music && music->getStatus() == sf::Music::Playing && music->getLoop() == isLoop) {
                return;
            }
        }
        auto music = std::make_unique<sf::Music>();
        if (!music->openFromFile(musicFile)) {
            std::cerr << "Error loading file:" << musicFile << std::endl;
            return;
        }
            music->setVolume(volume);
            music->setLoop(isLoop);
            music->play();
            music->setPosition(0, 1, 10);
            _activeMusic.push_back(std::move(music));
    }


    void cleanupMusic() override
    {
        _activeMusic.erase(
            std::remove_if(
                _activeMusic.begin(),
                _activeMusic.end(),
                [](const std::unique_ptr<sf::Music>& music) {
                    return music->getStatus() == sf::Music::Stopped;
                }
            ),
            _activeMusic.end()
        );
    }

    // void cleanupMusic() override
    // {
    //     if (_activeMusic.empty()) {
    //         return;
    //     }

    //     _activeMusic.erase(
    //         std::remove_if(
    //             _activeMusic.begin(),
    //             _activeMusic.end(),
    //             [](const std::unique_ptr<sf::Music>& music) {
    //                 return !music || music->getStatus() != sf::Music::Playing;
    //             }
    //         ),
    //         _activeMusic.end()
    //     );
    // }

    void handleInputs(Systems::InputState &inputState)
    {
        inputState.attack = false;

        if (focus) {
            if (sf::Keyboard::isKeyPressed(keyMappings["MoveLeft"]) ||
                sf::Keyboard::isKeyPressed(keyMappings["MoveRight"]) ||
                sf::Keyboard::isKeyPressed(keyMappings["MoveUp"]) ||
                sf::Keyboard::isKeyPressed(keyMappings["MoveDown"]) ||
                sf::Keyboard::isKeyPressed(keyMappings["Shoot"]) ||
                sf::Keyboard::isKeyPressed(keyMappings["Force"])) {
                inputState.isKeyboard = true;
            }

            inputState.moveLeft = sf::Keyboard::isKeyPressed(keyMappings["MoveLeft"]);
            inputState.moveRight = sf::Keyboard::isKeyPressed(keyMappings["MoveRight"]);
            inputState.moveUp = sf::Keyboard::isKeyPressed(keyMappings["MoveUp"]);
            inputState.moveDown = sf::Keyboard::isKeyPressed(keyMappings["MoveDown"]);
            inputState.attack = sf::Keyboard::isKeyPressed(keyMappings["Shoot"]);
            inputState.fKey = sf::Keyboard::isKeyPressed(keyMappings["Force"]);

            for (unsigned int i = 0; i < sf::Joystick::Count; ++i) {
                if (sf::Joystick::isConnected(i)) {

                    for (unsigned int button = 0; inputState.isKeyboard && button < sf::Joystick::getButtonCount(i); ++button) {
                        if (sf::Joystick::isButtonPressed(i, button) ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::X) < -50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::X) > 50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::Y) < -50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::Y) > 50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::PovX) < -50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::PovX) > 50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::PovY) < -50 ||
                            sf::Joystick::getAxisPosition(i, sf::Joystick::PovY) > 50) {
                            inputState.isKeyboard = false;
                            break;
                        }
                    }

                    if (!inputState.isKeyboard) {
                        float xAxis = sf::Joystick::getAxisPosition(i, sf::Joystick::X);
                        float yAxis = sf::Joystick::getAxisPosition(i, sf::Joystick::Y);

                        inputState.moveLeft = (xAxis < -50);
                        inputState.moveRight = (xAxis > 50);
                        inputState.moveUp = (yAxis < -50);
                        inputState.moveDown = (yAxis > 50);

                        float dpadX = sf::Joystick::getAxisPosition(i, sf::Joystick::PovX);
                        float dpadY = sf::Joystick::getAxisPosition(i, sf::Joystick::PovY);

                        inputState.moveLeft |= (dpadX < -50);
                        inputState.moveRight |= (dpadX > 50);
                        inputState.moveUp |= (dpadY < -50);
                        inputState.moveDown |= (dpadY > 50);

                        inputState.attack = sf::Joystick::isButtonPressed(i, controllerShootButton);
                        inputState.fKey = sf::Joystick::isButtonPressed(i, controllerForceButton);
                    }
                }
            }
        }
    }

    void cleanupSounds() override
    {
        _activeSounds.erase(
            std::remove_if(
                _activeSounds.begin(),
                _activeSounds.end(),
                [](const std::unique_ptr<sf::Sound>& sound) {
                    return sound->getStatus() == sf::Sound::Stopped;
                }
            ),
            _activeSounds.end()
        );
    }

    std::unordered_map<std::string, sf::Keyboard::Key> getKeyMappings() const
    {
        return keyMappings;
    }

    void setKeyMappings(std::string key, sf::Keyboard::Key newKey)
    {
        keyMappings[key] = newKey;
    }

    int getControllerShootButton() const
    {
        return controllerShootButton;
    }

    int getControllerForceButton() const
    {
        return controllerForceButton;
    }

    void setControllerShootButton(int newButton)
    {
        controllerShootButton = newButton;
    }

    void setControllerForceButton(int newButton)
    {
        controllerForceButton = newButton;
    }

private:
    sf::RenderWindow _window;
    std::unordered_map<std::string, sf::Texture> _textures;
    std::unordered_map<std::string, sf::Font> _fonts;
    std::vector<ParallaxScroller> _parallaxLayers;
    std::unordered_map<std::string, sf::SoundBuffer> _soundBuffers;
    std::vector<std::unique_ptr<sf::Sound>> _activeSounds;
    std::deque<std::unique_ptr<sf::Music>> _activeMusic;
    bool focus = true;
    bool _drawBoundingBoxes;

    std::unordered_map<std::string, sf::Keyboard::Key> keyMappings;
    int controllerShootButton;
    int controllerForceButton;

    sf::Texture &getTexture(const std::string &textureId) 
    {
        auto it = _textures.find(textureId);
        if(it == _textures.end()){
            sf::Texture texture;
            if (!texture.loadFromFile(textureId)) {
                std::cerr << "Failed to load texture -> " << textureId << std::endl;
                throw std::runtime_error("Texture loading failed for: " + textureId);
            }
            auto res = _textures.emplace(textureId, std::move(texture));
            return res.first->second;
        }
        return it->second;
    }

    sf::SoundBuffer &getSounds(const std::string &soundId) 
    {
        auto it = _soundBuffers.find(soundId);
        if(it == _soundBuffers.end()){
            sf::SoundBuffer buffer;
            if (!buffer.loadFromFile(soundId)) {
                std::cerr << "Failed to load sound -> " << soundId << std::endl;
                throw std::runtime_error("Sound loading failed for: " + soundId);
            }
            std::cout << buffer.getSampleCount() << std::endl;
            auto res = _soundBuffers.emplace(soundId, std::move(buffer));
            return res.first->second;
        }
        return it->second;
    }

    sf::Font &getFont(const std::string &fontPath)
    {
        auto it = _fonts.find(fontPath);
        if (it == _fonts.end()) {
            sf::Font font;
            if (!font.loadFromFile(fontPath)) {
                std::cerr << "Failed to load font -> " << fontPath << std::endl;
                throw std::runtime_error("Font loading failed for: " + fontPath);
            }
            auto res = _fonts.emplace(fontPath, std::move(font));
            return res.first->second;
        }
        return it->second;
    }
};
