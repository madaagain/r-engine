#pragma once

#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"

namespace Systems
{
    class BirdSystem : public ISystem
    {
        public:
            enum BirdState
            {
                Still,
                Falling,
                Flying
            };

            BirdSystem(ECS::Registry &registry, float &dt)
                : _registry(registry)
                , _dt(dt)
                , _isBirdInit(false)
                , _birdState(BirdState::Still)
                , _prevAttackState(false)
            {}

            void update(float dt) override
            {
                auto &controllables = _registry.getComponents<Controllable>();
                auto &velocities   = _registry.getComponents<Velocity>();
                if (!_isBirdInit) {
                    initBird();
                    _isBirdInit = true;
                }
                const float gravity = 600.0f;
                const float flyImpulse = -200.0f;
                for (auto &&[con, vel] : Zipper(controllables, velocities)) {
                    if (con.has_value() && vel.has_value()) {
                        if (con->attack && !_prevAttackState) {
                            vel->vy = flyImpulse;
                            _birdState = BirdState::Flying;
                        }
                        else {
                            vel->vy += gravity * dt;
                            _birdState = BirdState::Falling;
                        }
                        _prevAttackState = con->attack;
                    }
                }
            }

        private:
            ECS::Registry &_registry;
            float &_dt;
            bool _isBirdInit;
            BirdState _birdState;
            bool _prevAttackState;

            void initBird()
            {
                ECS::Entity bird = _registry.spawnEntity();
                Controllable controllable(false, false, false, false, false, static_cast<uint32_t>(bird));
                Player player(static_cast<uint32_t>(bird));
                _registry.addComponent<Velocity>(bird, Velocity{0.0f, 0.0f});
                _registry.addComponent<Player>(bird, player);
                _registry.addComponent<Controllable>(bird, controllable);
                _registry.addComponent<Renderable>(bird, Renderable{
                    "../../client/assets/sprites/flappybird.png",
                    34,
                    24,
                    0.0f,
                    0.0f,
                    0, 
                    true, 
                    true, 
                    34, 
                    24, 
                    0
                });
                _registry.addComponent<Color>(bird, Color{255.f, 255.f, 255.f, 255.f});
                _registry.addComponent<Transformable>(bird, Transformable{200.0f, 300.0f, 0.0f, 1.5f, 1.5f});
            }
    };
}
