

#pragma once

#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"
#include <iostream>
#include <cstdlib>

namespace Systems
{
    class ShieldSystem : public ISystem
    {
    public:
        ShieldSystem(ECS::Registry &registry, float &dt)
            : _registry(registry),
              _dt(dt),
              _spawnTime(0.0f),
              _shieldNb(0),
              _MaxShield(0),
              _spawnTiming(0.0f)
        {}

        void update(float dt) override
        {
            auto &scenes = _registry.getComponents<Scene>();
            for (auto &&[sce] : Zipper(scenes)) {
                if (sce->currentScene == "LevelOne") {
                    _MaxShield = 3;
                    _spawnTiming = 15.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(20.0f - 15.0f)));
                    break;
                }
            }

            for (auto &&[sce] : Zipper(scenes)) {
                if (sce->currentScene == "LevelTwo") {
                    _MaxShield = 5;
                    _spawnTiming = 15.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(20.0f - 15.0f)));
                    break;
                }
            }
            for (auto &&[sce] : Zipper(scenes)) {
                if (sce->currentScene == "LevelThree") {
                    _MaxShield = 5;
                    _spawnTiming = 15.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(20.0f - 15.0f)));
                    break;
                }
            }
            _spawnTime += dt;
            if (_spawnTime >= _spawnTiming && _shieldNb < _MaxShield) {
                spawnShield();
                _spawnTime = 0.0f;
                _shieldNb++;
            }
            auto &collidables = _registry.getComponents<Collidable>();
            auto &renderables = _registry.getComponents<Renderable>();
            auto &transformables = _registry.getComponents<Transformable>();
            for (auto &&[idx, coll, ren, tran] : indexedZipper(collidables, renderables, transformables)) {
                if (coll.has_value() && ren.has_value() && tran.has_value() && coll->category == CollisionCategory::ShieldBonus) {
                    if (!ren->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                    }
                    if (coll->isColliding) {
                        coll->isColliding = false;
                        ren->visible = false;
                    }
                    if (tran->positionX < 0.0f) {
                        ren->visible = false;
                    }
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        float _spawnTime;
        int _shieldNb;
        int _MaxShield;
        float _spawnTiming;

        void spawnShield()
        {
            float spawnY = static_cast<float>(std::rand() % (586 - 30 + 1) + 30);
            ECS::Entity shield = _registry.spawnEntity();
            _registry.addComponent<Velocity>(shield, Velocity{-100.0f, 0.0f});
            _registry.addComponent<Renderable>(shield, Renderable{
                "../../client/assets/sprites/r-typesheet3.gif",
                205, 18, 0.0f, 0.0f, 1, true, true, 17, 18, 0});
            _registry.addComponent<Transformable>(shield, Transformable{
                800, spawnY, 0.0f, 2.0f, 2.0f});
            _registry.addComponent<Collidable>(shield, Collidable{
                14.5f, 11.0f, false,
                CollisionCategory::ShieldBonus,
                static_cast<uint8_t>(CollisionCategory::Player)
            });
            Animatable animatable({{0, 12, true, false, 12, 0, 0.1f}});
            animatable.animationMap["Idle"] = 1;
            _registry.addComponent<Animatable>(shield, animatable);
            _registry.addComponent<ClockComponent>(shield, ClockComponent{0.0f});

        }
    };
}
