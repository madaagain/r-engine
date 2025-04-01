#pragma once

#include <cstdlib>
#include <unordered_map>
#include <iostream>

#include "Components.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"

namespace Systems
{
    class BasicMissileSystem : public ISystem
    {
        public:
            BasicMissileSystem(ECS::Registry &registry,
                               ECS::SparseArray<Controllable> &controls, 
                               ECS::SparseArray<Velocity> &velocities,
                               ECS::SparseArray<Transformable> &transformables,
                               ECS::SparseArray<Player> &players,
                               float &dt)
                : _registry(registry),
                  _controls(controls),
                  _velocities(velocities),
                  _transformables(transformables),
                  _players(players),
                  _dt(dt)
            {}

            void update(float dt) override
            {
                (void) dt;
                auto &health = _registry.getComponents<Health>();

                for (auto &&[idx, inp, vel, trans, play, hp] : indexedZipper(_controls, _velocities, _transformables, _players, health)) {
                    if (inp.has_value() && vel.has_value() && trans.has_value() && play.has_value()) {
                        bool currentAttack = inp->attack;
                        uint32_t playerId = play->playerId;
                        bool &lastAttackState = _playerAttackState[playerId];
                        if (currentAttack && !lastAttackState && hp->currentHealth > 0) {
                            float spawnX = trans->positionX + 20.0f;
                            float spawnY = trans->positionY + 10.0f;
                            ECS::Entity missile = _registry.spawnEntity();
                            _registry.addComponent<Velocity>(missile, Velocity{650.0f, 0.0f});
                            _registry.addComponent<Missile>(missile, Missile{10.0f});
                            _registry.addComponent<Renderable>(missile, Renderable{"../../client/assets/sprites/player-missile.png", 19, 11, 0.0f, 0.0f, 0, true, true, 19, 11, 0});
                            _registry.addComponent<Color>(missile, {255.f, 255.f, 255.f, 255.f});
                            _registry.addComponent<Transformable>(missile, Transformable{spawnX, spawnY, 0.0f, 2.0f, 2.0f});
                            _registry.addComponent<ClockComponent>(missile, ClockComponent{0});
                            _registry.addComponent<Collidable>(missile, Collidable{
                                17.5f,
                                11.0f,
                                false, 
                                CollisionCategory::PlayerMissile,
                                static_cast<uint8_t>(CollisionCategory::Enemy)
                            });
                            _registry.addComponent<Audio>(missile, {
                                "../../client/assets/sounds/ship-shoot.ogg",
                                true,
                                false,
                                10.0f,
                                0.8f + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (1.1f - 0.8f),
                            });
                            lastAttackState = true;
                        } else if (!currentAttack && lastAttackState) {
                            lastAttackState = false;
                            stopPlayingSoundMissile();// this part needs to be fixed, when keeping the space bar pushed down the sound will keep playing...
                        }                
                    }
                }
            }

        private:
            ECS::Registry &_registry;
            ECS::SparseArray<Controllable> &_controls;
            ECS::SparseArray<Velocity> &_velocities;
            ECS::SparseArray<Transformable> &_transformables;
            ECS::SparseArray<Player> &_players;
            float &_dt;
            std::unordered_map<uint32_t, bool> _playerAttackState;

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
                        if (tran->positionX > 800.0f || col->isColliding) { 
                            rend->visible = false;
                        }
                    }
                }
            }

            void stopPlayingSoundMissile() 
            {
                auto &missiles = _registry.getComponents<Missile>();
                auto &audios = _registry.getComponents<Audio>();
                for (auto &&[idx, miss, sou] : indexedZipper(missiles, audios)) {
                    if (miss.has_value() && sou.has_value()) {
                        sou->isPlaying = false;
                    }
                }
            } 
    };
}