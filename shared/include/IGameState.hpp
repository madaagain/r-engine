

#pragma once
#include <stdint.h>
#include <unordered_map>
#include <ctime>

#include "../../ecs/include/Registry.hpp"
#include "../../ecs/include/ZipperIterator.hpp"
#include "../../ecs/include/IndexedZipper.hpp"
#include "../../ecs/components/Components.hpp"

class IGameState
{
public:

    struct StateOfPlayer {
        float x;
        float y;
        float vx;
        float vy;
        uint32_t entityId;
        bool isActive;
    };

    struct GameSnap {
        std::unordered_map<uint32_t, StateOfPlayer> players;
        uint32_t timeStamp;
    };

    virtual ~IGameState() = default;
    virtual GameSnap captureState() = 0;
    virtual void applyState(const GameSnap& snapshot) = 0;
};
