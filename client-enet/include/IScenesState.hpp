

#ifndef ISCENESSTATE_HPP
#define ISCENESSTATE_HPP

#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include <iostream>
#include <vector>

/**
 * @brief Interface for game scenes (Menu, Options, ...).
 *
 * We do *not* store an sf::RenderWindow in this interface.
 * Each derived scene can store its own reference to the window.
**/

class IScenesState
{
    public:
        virtual ~IScenesState() = default;

        virtual void run() = 0;
        virtual void processEvents() = 0;
        virtual void render() = 0;
        virtual void update() = 0;
};

#endif // ISCENESSTATE_HPP
