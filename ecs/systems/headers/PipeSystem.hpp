#pragma once

#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"

#include <random>
#include <iostream>

namespace Systems 
{
    class PipeSystem : ISystem
    {
        public:
            PipeSystem(ECS::Registry &registry, float &dt) 
                : _registry(registry), _dt(dt), _spawnTimer(0.0f)
            {
                std::random_device rd;
                _randEngine = std::mt19937(rd());
                _offsetDist = std::uniform_real_distribution<float>(-150.0f, 150.0f);
            }
            
            void update(float dt) override
            {
                _spawnTimer += dt;
                if (_spawnTimer >= 2.0f) {
                    float randomOffset = _offsetDist(_randEngine);
                    spawnPipe(180.0f, 950.0f, 0.0f + randomOffset);
                    spawnPipe(0.0f, 950.0f, 750.0f + randomOffset);
                    _spawnTimer = 0.0f;
                }
            }
        
        private:
            ECS::Registry &_registry;
            float &_dt;
            float _spawnTimer;
            std::mt19937 _randEngine;
            std::uniform_real_distribution<float> _offsetDist;

            void spawnPipe(float rotationAngle, float x, float y)
            {
                ECS::Entity pipe = _registry.spawnEntity();
                _registry.addComponent<Velocity>(pipe, Velocity{-150.0f, 0.0f});
                _registry.addComponent<Renderable>(pipe, Renderable{
                    "../../client/assets/sprites/pipe-red.png",
                    52, 320, 0.0f, 0.0f, 0, true, true, 52, 320, 0
                });
                _registry.addComponent<Transformable>(pipe, Transformable{
                    x, y, rotationAngle, 2.0f, 2.0f
                });
                _registry.addComponent<Color>(pipe, Color{255.0f, 255.0f, 255.0f, 255.0f});
            }
    };
}
