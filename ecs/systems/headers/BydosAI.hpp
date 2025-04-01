#pragma once


#include <cmath>
#include <iostream>
#include "Components.hpp"
#include "ISystems.hpp"

namespace Systems
{
    constexpr float SCENE_TIMING_LVL_ONE = 60.0f;
    constexpr float SCENE_TIMING_LVL_TWO = 50.0f;
    constexpr float SCENE_TIMING_LVL_THREE = 60.0f;
    constexpr float SPAWN_TIMING_LVL_ONE = 2.0f;
    constexpr float SPAWN_TIMING_LVL_TWO = 1.6f;
    constexpr float SPAWN_TIMING_LVL_THREE = 0.8f;

    class BydosAI : public ISystem
    {
        public:
            BydosAI(ECS::Registry &registry, float &dt)
            : _registry(registry), _shootingTime(0.0f), _spawnTime(0.0f), _sceneTime(0.0f), _dt(dt) {}

            void update(float dt) override
            {
                resetScene();
                auto &transformables = _registry.getComponents<Transformable>();
                auto &ais = _registry.getComponents<AIState>();
                auto &velocities = _registry.getComponents<Velocity>();
                auto &bydosAI = _registry.getComponents<BydosAIComponent>();
                auto &collidables = _registry.getComponents<Collidable>();
                auto &health = _registry.getComponents<Health>();
                auto &renderables = _registry.getComponents<Renderable>();
                auto &scores = _registry.getComponents<Score>();
                auto &colors = _registry.getComponents<Color>();
                for (auto &&[idx, tran, ai, vel, bydos, coll , hp, rend, color] : indexedZipper(transformables, ais, velocities, bydosAI, collidables, health, renderables, colors)) {
                    if (tran.has_value() && ai.has_value() && vel.has_value() && coll.has_value() && color.has_value()) {
                        if (!rend->visible) {
                            _registry.killEntity(_registry.entityFromIndex(idx));
                            ai->isDead = true;
                        }
                        vel->vx = -100.0f;
                        bydos->time += dt;
                        _shootingTime += dt;
                        ai->isAttacking = false;
                        if (_shootingTime >= 2.0f) {
                            ai->isAttacking = true;
                            _shootingTime = 0.0f;
                        } else {
                            ai->isAttacking = false;
                        }

                        vel->vy = std::sin(bydos->time * bydos->frequency) * bydos->amplitude;
                        if (coll->isColliding) {
                            hp->currentHealth--;
                        }
                        if (tran->positionX <= 0.0f) {
                            rend->visible = false;
                        }
                        if (hp->currentHealth <= 0 && rend->visible) {
                            rend->visible = false;
                            explosion(tran->positionX, tran->positionY);
                            for (auto &&[idx, sc] : indexedZipper(scores)) {
                                if (sc.has_value() && !ai->isDead) {
                                    sc->score = 10;
                                    sc->increase = true;
                                }
                            }
                        }
                    }
                }
                _spawnTime += dt;
                _sceneTime += dt;
                if (_spawnTime >= this->getSpawnTiming() && _sceneTime <= this->getSceneTiming()) {
                    spawnEnemy();
                    _spawnTime = 0.0f;
                }
            }

        private:
            ECS::Registry &_registry;
            float _shootingTime;
            float _spawnTime;
            float _sceneTime;
            float &_dt;

            void spawnEnemy()
            {
                auto bydosEnemy = _registry.spawnEntity();
                _registry.addComponent<Velocity>(bydosEnemy, Velocity{0.0f, 0.0f});
                Animatable animatable({{0, 7, true, false, 16, 0, 0.08f}});
                animatable.animationMap["Idle"] = 1;
                _registry.addComponent<BydosAIComponent>(bydosEnemy, {10, 0.0f, 1.0f + static_cast<float>(std::rand() % 100) / 100.0f, 30.0f + static_cast<float>(std::rand() % 50)});
                _registry.addComponent<Animatable>(bydosEnemy, animatable);
                _registry.addComponent<Renderable>(bydosEnemy, Renderable{"../../client/assets/sprites/r-typesheet5.gif", 533, 36, 0.0f, 0.0f, 1, true, true, 0});
                _registry.addComponent<Color>(bydosEnemy, {255.f, 255.f, 255.f, 255.f});
                _registry.addComponent<Transformable>(bydosEnemy, Transformable{800.0f, static_cast<float>(std::rand() % (565-0 + 1) + 0), 0.0f, 2.0f, 2.0f});
                _registry.addComponent<ClockComponent>(bydosEnemy, ClockComponent{0.0f});
                _registry.addComponent<Health>(bydosEnemy, Health{3, 3, false});
                _registry.addComponent<AIState>(bydosEnemy, AIState{true, false, true});
                _registry.addComponent<Collidable>(bydosEnemy, Collidable{33.0f, 36.0f, CollisionCategory::Enemy, static_cast<uint8_t>(CollisionCategory::PlayerMissile)});
            }

            float getSceneTiming()
            {
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[scene] : Zipper(scenes)) {
                    if (scene.has_value()) {
                        if (scene->currentScene == "LevelOne") {
                            return SCENE_TIMING_LVL_ONE;
                        }
                        if (scene->currentScene == "LevelTwo") {
                            return SCENE_TIMING_LVL_TWO;
                        }
                        if (scene->currentScene == "LevelThree") {
                            return SCENE_TIMING_LVL_THREE;
                        }
                        if (scene->currentScene == "LevelThree") {
                            return SCENE_TIMING_LVL_THREE;
                        }
                    }
                }
                return 0.0f;
            }

            float getSpawnTiming()
            {
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[scene] : Zipper(scenes)) {
                    if (scene.has_value()) {
                        if (scene->currentScene == "LevelOne") {
                            return SPAWN_TIMING_LVL_ONE;
                        }
                        if (scene->currentScene == "LevelTwo") {
                            return SPAWN_TIMING_LVL_TWO;
                        }
                        if (scene->currentScene == "LevelThree") {
                            return SPAWN_TIMING_LVL_THREE;
                        }
                        if (scene->currentScene == "LevelThree") {
                            return SPAWN_TIMING_LVL_THREE;
                        }
                    }
                }
                return 0.0f;
            }

            void resetScene()
            {
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[scene] : Zipper(scenes)) {
                    if (scene.has_value()) {
                        if (scene->isResetting && scene->currentScene == "SceneTransition"){
                            _sceneTime = 0.0f;
                        }
                    }
                }
            }

            void explosion(float x, float y)
            {
                auto explosionEntity = _registry.spawnEntity();
                _registry.addComponent<Transformable>(explosionEntity, Transformable{x, y, 0.0f, 1.0f, 1.0f});
                _registry.addComponent<Animatable>(explosionEntity, Animatable({{0, 6, false, false, 6, 0, 0.08f}}));
                _registry.addComponent<Renderable>(explosionEntity, Renderable{"../../client/assets/sprites/r-typesheet44-explosion1.gif", 301, 52, 0.0f, 0.0f, 1, true, true, 0});
                _registry.addComponent<Color>(explosionEntity, {255.f, 255.f, 255.f, 255.f});
                _registry.addComponent<ClockComponent>(explosionEntity, ClockComponent{0.0f});
                _registry.addComponent<Explosion>(explosionEntity, {0.3f});
            }
    };
}
