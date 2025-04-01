#pragma once

#include <iostream>

#include "Components.hpp"
#include "ISystems.hpp"
#include "SparseArray.hpp"

namespace Systems
{
    class EnemyMissile : public ISystem 
    {
        public:
            EnemyMissile(ECS::Registry &registry, float &dt)
                : _registry(registry),
                  _dt(dt)
            {}

        void update(float dt) override
        {
            (void) dt;
            auto &BydosAIS = _registry.getComponents<BydosAIComponent>();
            auto &transformables = _registry.getComponents<Transformable>();
            auto &velocities = _registry.getComponents<Velocity>();
            auto &AIstates = _registry.getComponents<AIState>();

            for (auto &&[idx, ai, tran, vel, state] : indexedZipper(BydosAIS, transformables, velocities, AIstates)) {
                if (vel.has_value() && tran.has_value() && ai.has_value() && state.has_value()) {
                    if (state->isAttacking) {
                        float spawnX = tran->positionX - 10.0f;
                        float spawnY = tran->positionY;
                        ECS::Entity missile = _registry.spawnEntity();
                        _registry.addComponent<Velocity>(missile, Velocity{-450.0f, 0.0f});
                        _registry.addComponent<Missile>(missile, Missile{10.0f});
                        _registry.addComponent<Renderable>(missile, Renderable{"../../client/assets/sprites/r-typesheet1.gif", 2, 2, 300.0f, 275.0f, 0, true, true, 5, 5, 0});
                        _registry.addComponent<Color>(missile, {255.f, 255.f, 255.f, 255.f});
                        _registry.addComponent<Transformable>(missile, Transformable{spawnX, spawnY, 0.0f, 2.2f, 2.2f});
                        _registry.addComponent<ClockComponent>(missile, ClockComponent{0});
                        _registry.addComponent<Collidable>(missile, Collidable{
                            17.5f,
                            11.0f,
                            false, 
                            CollisionCategory::EnemyMissile,
                            static_cast<uint8_t>(CollisionCategory::Player)
                        });
                    }                        
                }                            
            }

            auto &scenes = _registry.getComponents<Scene>();
                    for (auto &&[sce] : Zipper(scenes)) {
                        if (sce.has_value()) {
                            if (sce->currentScene != "LevelOne")
                                return;
                            else
                                gorgiaMissile(transformables, velocities);
                        }
                    }
            // deleteMissile();
        }

        private:
            ECS::Registry &_registry;
            float &_dt;

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

            void gorgiaMissile(ECS::SparseArray<Transformable> &transformables, ECS::SparseArray<Velocity> &velocities)
            {
            auto &bossAI = _registry.getComponents<BossAIComponent>();
            auto &bossStates = _registry.getComponents<BossAIState>();
            for (auto &&[idx, boss, tran, vel, state] : indexedZipper(bossAI, transformables, velocities, bossStates)) {
                if (vel.has_value() && tran.has_value() && boss.has_value() && state.has_value()) {
                    if (state->isAttacking) {
                        float spawnX = tran->positionX - 10.0f;
                        float spawnY = tran->positionY;
                        const float angles[] = {-30.0f, 0.0f, 30.0f};
                        for (float angle : angles) {
                            float rad = angle * M_PI / 180.0f;
                            float vx = -300.0f * std::cos(rad);
                            float vy = -300.0f *std::sin(rad);
                            ECS::Entity missile = _registry.spawnEntity();
                            _registry.addComponent<Velocity>(missile, Velocity{vx, vy});
                            _registry.addComponent<Missile>(missile, Missile{15.0f});
                            _registry.addComponent<Renderable>(missile, Renderable{
                            "../../client/assets/sprites/r-typesheet1.gif",
                            20,
                            5,
                            335.0f,
                            255.0f,
                            0,
                            true,
                            true, 30, 10, 0});
                            _registry.addComponent<Color>(missile, {255.f, 255.f, 255.f, 255.f});
                            _registry.addComponent<Transformable>(missile, Transformable{spawnX, spawnY, 180.0f, 2.2f, 2.2f});
                            _registry.addComponent<ClockComponent>(missile, ClockComponent{0});
                            _registry.addComponent<Collidable>(missile, Collidable{
                                14.5f,
                                11.0f,
                                false, 
                                CollisionCategory::EnemyMissile,
                                static_cast<uint8_t>(CollisionCategory::Player)
                            });
                        }
                    }
                }
            }
        }
    };
}