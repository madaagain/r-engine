#pragma once

#include "ISystems.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include <iostream>

namespace Systems
{
    class MissileCleanupSystem : public ISystem
    {
        public:
            MissileCleanupSystem(ECS::Registry &registry, float &dt)
                : _registry(registry), _dt(dt) {}

            void update(float dt) override
            {
                (void) dt;
                auto &missiles = _registry.getComponents<Missile>();
                auto &transformables = _registry.getComponents<Transformable>();
                auto &renderables = _registry.getComponents<Renderable>();
                auto &collidables = _registry.getComponents<Collidable>();

                for (auto &&[idx, mis, tra, ren, col] : indexedZipper(missiles, transformables, renderables, collidables)) {
                    if (!mis.has_value() || !tra.has_value() || !ren.has_value() || !col.has_value())
                        continue;
                    if (!ren->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                        continue;
                    }
                    if (tra->positionX > 800.0f || col->isColliding || tra->positionX < 0.0f) {
                        ren->visible = false;
                    }
                }
            }

        private:
            ECS::Registry &_registry;
            float &_dt;
    };
}
