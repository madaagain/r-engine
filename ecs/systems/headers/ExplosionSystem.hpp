#pragma once

#include "ISystems.hpp"
#include "Components.hpp"
#include <iostream>

namespace Systems
{
    class ExplosionSystem : public ISystem 
    {
    public:
        ExplosionSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt) {}

        void update(float dt) override
        {
            (void)dt;
            auto &explosions = _registry.getComponents<Explosion>();
            auto &renderables = _registry.getComponents<Renderable>();
            for (auto &&[idx, expl, rend] : indexedZipper(explosions, renderables)) {
                if (expl.has_value() && rend.has_value()) {
                    if (!rend->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                        continue;
                    }
                    expl->duration -= _dt;
                    if (expl->duration <= 0.0f) {
                        rend->visible = false;
                    }
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
    };
}