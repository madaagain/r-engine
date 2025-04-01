#pragma once

#include "Registry.hpp"
#include "Components.hpp"
#include "IndexedZipper.hpp"

class Utils 
{
    public:
        Utils(ECS::Registry &registry) : _registry(registry){}
        ~Utils() {}

        void deleteMissile()
        {
            auto &missiles = _registry.getComponents<Missile>();
            auto &transformables = _registry.getComponents<Transformable>();
            auto &renderables = _registry.getComponents<Renderable>();
            auto &collidable = _registry.getComponents<Collidable>();
            for (auto &&[idx, miss, tran, rend, col] : indexedZipper(missiles, transformables, renderables, collidable)) {
                if (miss.has_value() && tran.has_value() && rend.has_value() && col.has_value()) {
                    if (!rend->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                    }
                    if (tran->positionX < 0.0f || col->isColliding) {
                        rend->visible = false;
                    }
                }
            }
        }

    private:
        ECS::Registry &_registry;

};
