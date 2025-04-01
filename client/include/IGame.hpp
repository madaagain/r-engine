
#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <string>
// #include "Client.hpp"

class IState
{
    public:
        virtual ~IState() = default;

        virtual void run() = 0;
        virtual void processEvents() = 0;
        virtual void render() = 0;
        virtual void update() = 0;
};


#endif // GAME_HPP
