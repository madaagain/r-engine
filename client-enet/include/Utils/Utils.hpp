

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

class Utility
{
    public:
        static sf::Font& getFont() {
            static sf::Font font;
            if (!font.loadFromFile("../../client/assets/fonts/r-type.ttf")) {
                std::cerr << "Failed to load font." << std::endl;
            }
            return font;
        }

        static sf::Texture& getTexture(const std::string& filename) {
            static std::map<std::string, sf::Texture> textures;
            auto& texture = textures[filename];
            if (texture.getSize().x == 0) {
                if (!texture.loadFromFile(filename)) {
                    std::cerr << "Failed to load texture: " << filename << std::endl;
                }
            }
            return texture;
        }

        static sf::Text createText(const std::string& content, float x, float y, unsigned int size = 30, sf::Color color = sf::Color::White) {
            sf::Text text;
            text.setFont(Utility::getFont());
            text.setString(content);
            text.setCharacterSize(size);
            text.setFillColor(color);
            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
            text.setPosition(x, y);
            return text;
        }
};

#endif // UTILITY_HPP
