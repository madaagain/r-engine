#pragma once

#include <cmath>
#include <random>
#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "../../renderer/Renderer.hpp"
#include "SparseArray.hpp"
#include "ZipperIterator.hpp"

namespace Systems
{
    constexpr float SPAWN_TIMING = 63.0f;

    class GorgiaAI : public ISystem
    {
       public:
           GorgiaAI(ECS::Registry &registry, float &dt)
               : _registry(registry), 
                 _dt(dt),
                 _shootingTime(0.0f),
                 _directionTimer(0.0f),
                 _spawnTimer(0.0f),
                 _bossSpawned(false),
                 rng(std::random_device{}()),
                 angleDistribution(0.0f, 2.0f * 3.14159f),
                 speedDistribution(50.0f, 250.0f),
                 _musicStarted(false) {}

           void update(float dt) override
           { 
                auto &transformables = _registry.getComponents<Transformable>();
                auto &ais = _registry.getComponents<BossAIState>();
                auto &velocities = _registry.getComponents<Velocity>();
                auto &gorgiaAI = _registry.getComponents<BossAIComponent>();
                auto &collidables = _registry.getComponents<Collidable>();
                auto &health = _registry.getComponents<Health>();
                auto &renderables = _registry.getComponents<Renderable>();
                auto &scenes = _registry.getComponents<Scene>();

               _directionTimer += dt;
               _spawnTimer += dt;
                this->playLevelMusic(_spawnTimer, SPAWN_TIMING);
                if (_spawnTimer >= SPAWN_TIMING && !_bossSpawned) {
                    spawnBoss(_registry);
                    _bossSpawned = true;
                }

               for (auto &&[idx, tran, ai, vel, gorgia, coll, hp, rend] : 
                   indexedZipper(transformables, ais, velocities, gorgiaAI, collidables, health, renderables)) {
                   if (!tran.has_value() || !ai.has_value() || !vel.has_value() || !coll.has_value())
                       continue;
                   if (!rend->visible) {
                       _registry.killEntity(_registry.entityFromIndex(idx));
                       continue;
                   }
                   _shootingTime += dt;
                   ai->isAttacking = _shootingTime >= 1.5f;
                   if (ai->isAttacking) {
                       _shootingTime = 0.0f;
                   }
                   if (_directionTimer >= 2.0f) {
                       float angle = angleDistribution(rng);
                       float speed = speedDistribution(rng);
                       vel->vx = std::cos(angle) * speed;
                       vel->vy = std::sin(angle) * speed;
                       _directionTimer = 0.0f;
                   }
                   if (tran->positionX <= 100.0f || tran->positionX >= 781.0f) {
                       vel->vx *= -1.25f;
                   }
                   if (tran->positionY <= 100.0f || tran->positionY >= 581.0f) {
                       vel->vy *= -1.25f;
                   }
                   if (coll->isColliding) {
                       hp->currentHealth--;
                   }
                   if (hp->currentHealth <= 0) {
                       if (!ai->isDead) {
                            ai->isDead = true;
                       }
                       auto &scores = _registry.getComponents<Score>();

                       for (auto &&[idx, sc] : indexedZipper(scores)) {
                           if (sc.has_value() && !ai->isDead) {
                               sc->score = 50;
                               sc->increase = true;
                           }
                       }
                        bossExplosion(tran->positionX, tran->positionY);

                       for (auto &&[sce] : Zipper(scenes)) {
                           if (sce.has_value()) {
                                stopBossMusic(); 
                               sce->currentScene = "SceneTransition";
                           }
                       }
                       rend->visible = false;
                   }
                   gorgia->time += dt;
               }
           }

       private:
           ECS::Registry &_registry;
           float &_dt;
           float _shootingTime;
           float _directionTimer;
           float _spawnTimer;
           bool _bossSpawned;
           std::mt19937 rng;
           std::uniform_real_distribution<float> angleDistribution;
           std::uniform_real_distribution<float> speedDistribution;
           bool _musicStarted;
           bool _levelMusicStarted = false;

            void playLevelMusic(float spawnTimer, float spawnTiming)
            {
                if (!_levelMusicStarted) {
                    ECS::Entity musicLvl1 = _registry.spawnEntity();
                    _registry.addComponent<Audio>(musicLvl1, {
                        "../../client/assets/musics/Level1.wav",
                        true, 
                        false,
                        20.0f,
                        1.0f,
                        true,
                    });
                    _levelMusicStarted = true;
                }
                auto &scenes = _registry.getComponents<Scene>();
                auto &audios = _registry.getComponents<Audio>();
                for (auto &&[scene] : Zipper(scenes)) {
                    if (scene.has_value()) {
                        if (scene->currentScene == "LevelOne" && !_musicStarted) {
                            for (auto &&[audio] : Zipper(audios)) {
                                if (audio.has_value()) {
                                    if (audio->audioPath == "../../client/assets/musics/Level1.wav") {
                                        audio->isPlaying = true;
                                    }
                                }
                            }
                        } else if (scene->currentScene == "LevelOne" && spawnTimer >= spawnTiming) {
                            for (auto &&[audio] : Zipper(audios)) {
                                if (audio.has_value()) {
                                    if (audio->audioPath == "../../client/assets/musics/Level1.wav") {
                                        audio->isPlaying = false;
                                    }
                                }
                            }
                        }
                    }
                }
            }

            void stopBossMusic()
            {
                auto &audios = _registry.getComponents<Audio>();
                for (auto &&[audio] : Zipper(audios)) {
                    if (audio.has_value()) {
                        if (audio->audioPath == "../../client/assets/musics/Boss-level1.ogg") {
                            audio->volume = 0.0f;
                            std:: cout << "Audio stopped playing\n";
                        }
                    }
                }
                ECS::Entity musicWin = _registry.spawnEntity();
                _registry.addComponent<Audio>(musicWin, {
                    "../../client/assets/musics/win-level.ogg",
                    true, 
                    false,
                    15.0f,
                    1.0f,
                    true,
                });
            }

            ECS::Entity spawnBoss(ECS::Registry &registry)
            {
                auto bossEnemy = registry.spawnEntity();
                registry.addComponent<Velocity>(bossEnemy, Velocity{0.0f, 0.0f});
                Animatable animatable({{0, 2, false, false, 6, 0, 0.08f}});
                animatable.animationMap["Idle"] = 1;
                registry.addComponent<BossAIComponent>(bossEnemy, {10, 0.0f, 1.0f + static_cast<float>(std::rand() % 100) / 100.0f, 30.0f + static_cast<float>(std::rand() % 50)});
                registry.addComponent<Animatable>(bossEnemy, animatable);
                registry.addComponent<Renderable>(bossEnemy, Renderable{"../../client/assets/sprites/r-typesheet9.gif", 332, 58, 0.0f, 0.0f, 8, true, true, 0});
                registry.addComponent<Color>(bossEnemy, {255.f, 255.f, 255.f, 255.f});
                registry.addComponent<Transformable>(bossEnemy, Transformable{780.0f, 580.0f, 0.0f, 2.0f, 2.0f});
                registry.addComponent<ClockComponent>(bossEnemy, ClockComponent{0.0f});
                registry.addComponent<Health>(bossEnemy, Health{300, 300, false});
                registry.addComponent<BossAIState>(bossEnemy, BossAIState{false, false, true});
                registry.addComponent<Collidable>(bossEnemy, Collidable{66.0f, 58.0f, CollisionCategory::Enemy, static_cast<uint8_t>(CollisionCategory::PlayerMissile)});
                if (!_musicStarted) {
                    ECS::Entity musicBoss = _registry.spawnEntity();
                    registry.addComponent<Audio>(musicBoss, {
                        "../../client/assets/musics/Boss-level1.ogg",
                        true,
                        false,
                        25.0f,
                        1.0f,
                        true,
                    });
                    _musicStarted = true;
                }
                return bossEnemy;
            }

        void bossExplosion(float x, float y)
        {
            auto explosionEntity = _registry.spawnEntity();
            _registry.addComponent<Transformable>(explosionEntity, Transformable{x, y, 0.0f, 1.0f, 1.0f});
            _registry.addComponent<Animatable>(explosionEntity, Animatable({{0, 5, false, false, 5, 0, 0.08f}}));
            _registry.addComponent<Renderable>(explosionEntity, Renderable{"../../client/assets/sprites/r-typesheet44-big-explosion.gif", 654, 136, 0.0f, 0.0f, 1, true, true, 0});
            _registry.addComponent<Color>(explosionEntity, {255.f, 255.f, 255.f, 255.f});
            _registry.addComponent<ClockComponent>(explosionEntity, ClockComponent{0.0f});
            _registry.addComponent<Explosion>(explosionEntity, {0.4f});
        }

    };
}