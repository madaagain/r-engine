#pragma once

#include "Components.hpp"
#include "ISystems.hpp"
#include "IndexedZipper.hpp"
#include "Registry.hpp"
#include <unordered_map>
#include <iostream>

namespace Systems
{
    class DmgIndicatorSystem : public ISystem
    {
    public:
        DmgIndicatorSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt) {}

        void update(float dt) override
        {
            updateCollisionFlash();

            for (auto it = _flashTimers.begin(); it != _flashTimers.end(); ) {
                size_t entityIndex = it->first;
                float &timeLeft = it->second;
                timeLeft -= dt;
                if (timeLeft > 0.0f) {
                    setColor(entityIndex, 255.f, 0.f, 0.f, 255.f);
                    ++it;
                } else {
                    setColor(entityIndex, 255.f, 255.f, 255.f, 255.f);
                    it = _flashTimers.erase(it);
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::unordered_map<size_t, float> _flashTimers;

        void updateCollisionFlash()
        {
            auto &collidables = _registry.getComponents<Collidable>();
            auto &health = _registry.getComponents<Health>();

            for (auto &&[idx, collidable, health] : indexedZipper(collidables, health)) {
                if (collidable.has_value() && health.has_value()) {
                    if (!collidable->isColliding)
                        continue;
                    _flashTimers[idx] = 0.1f;
                }
            }
        }

        void setColor(size_t entityIndex, float r, float g, float b, float a)
        {
            ECS::Entity entity = _registry.entityFromIndex(entityIndex);
            auto &colors = _registry.getComponents<Color>();
            if (entity < colors.size() && colors[entity].has_value()) {
                auto &col = *colors[entity];
                col.r = r;
                col.g = g;
                col.b = b;
                col.a = a;
            }
        }
    };
}
