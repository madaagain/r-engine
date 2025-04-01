

#pragma once

#include "ISystems.hpp"
#include "Components.hpp"
#include "Registry.hpp"
#include "IndexedZipper.hpp"

#include <iostream>
#include <random>

namespace Systems
{
    class HealingSystem : public ISystem
    {
        public:
            HealingSystem(ECS::Registry &registry, float &dt)
                : _registry(registry),
                  _spawnTimer(0.0f),
                  _nextSpawnInterval(0.0f),
                  _dt(dt),
                  _healNb(0),
                  _MaxHeal(0)
            {}

            void update(float dt) override
            {
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[sce] : Zipper(scenes)) {
                    if (sce->currentScene == "LevelOne") {
                        _MaxHeal = 5;
                        _nextSpawnInterval = 10.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(15.0f - 10.0f)));
                        break;
                    }
                }
                for (auto &&[sce] : Zipper(scenes)) {
                    if (sce->currentScene == "LevelTwo") {
                        _MaxHeal = 5;
                        _nextSpawnInterval = 10.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(30.0f - 20.0f)));
                        break;
                    }
                }
                for (auto &&[sce] : Zipper(scenes)) {
                    if (sce->currentScene == "LevelTwo") {
                        _MaxHeal = 7;
                        _nextSpawnInterval = 10.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(30.0f - 20.0f)));
                        break;
                    }
                }

                _spawnTimer += dt;
                if (_spawnTimer >= _nextSpawnInterval && _healNb < _MaxHeal) {
                    spawnHeal();
                    _spawnTimer = 0.0f;
                    _healNb++;
                    _nextSpawnInterval = 10.0f + static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX/(15.0f - 10.0f)));
                }
                deleteHeal();
            }

        private:
            ECS::Registry &_registry;
            float _spawnTimer;
            float _nextSpawnInterval;
            float &_dt;
            int _healNb;
            int _MaxHeal;

            void spawnHeal()
            {
                float spawnY = static_cast<float>(std::rand() % (570 - 30 + 1) + 30);
                ECS::Entity heal = _registry.spawnEntity();
                _registry.addComponent<Velocity>(heal, Velocity{-100.0f, 0.0f});
                _registry.addComponent<Health>(heal, Health{1});
                _registry.addComponent<Renderable>(heal, Renderable{
                    "../../client/assets/sprites/heal.png",
                    30, 30, 0.0f, 0.0f, 0, true, true, 30, 30, 0
                });
                _registry.addComponent<Transformable>(heal, Transformable{
                    800.0f, spawnY, 0.0f, 1.0f, 1.0f
                });
                _registry.addComponent<Collidable>(heal, Collidable{
                    75.0f, 75.0f,
                    false,
                    CollisionCategory::Heal,
                    static_cast<uint8_t>(CollisionCategory::Player)
                });
            }

            void deleteHeal()
            {
                auto &transformables = _registry.getComponents<Transformable>();
                auto &renderables = _registry.getComponents<Renderable>();
                auto &collidables = _registry.getComponents<Collidable>();

                for (auto &&[idx, tran, rend, col] : indexedZipper(transformables, renderables, collidables)) {
                    if (tran.has_value() && rend.has_value() && col.has_value()) {
                        if (col->category == CollisionCategory::Heal) {
                            if (!rend->visible) {
                                _registry.killEntity(_registry.entityFromIndex(idx));
                                continue;
                            }
                            if (tran->positionX <= 0.0f || col->isColliding) {
                                rend->visible = false;
                            }
                        }
                    }
                }
            }
    };
}
