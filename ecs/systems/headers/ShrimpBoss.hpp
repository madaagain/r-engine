#pragma once

#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"
#include "ZipperIterator.hpp"
#include <cmath>

namespace Systems
{
    constexpr float SCENE_TIMING = 55.0f;
    constexpr float SHRIMP_SPAWN_X  = 1050.0f;
    constexpr float SHRIMP_TARGET_X = 600.0f;
    constexpr float SHRIMP_MOVE_SPEED = 50.0f;

    class ShrimpBoss : public ISystem
    {
    public:
        ShrimpBoss(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt),
              _shrimpSpawned(false), _sceneTime(0.0f),
              _shootTiming(0.0f), _waveOffset(0.0f),
              _timeSinceLastSpawn(0.0f), _projectilesSpawned(0),
              _shrimpIdx(0)
        {
        }

        void update(float dt) override
        {
            auto &scenes = _registry.getComponents<Scene>();
            for (auto &&[sce] : Zipper(scenes)) {
                if (!sce.has_value() || sce->currentScene != "LevelTwo") {
                    return;
                }
            }
            if (killShrimp(_shrimpIdx)) {
                return;
            }
            _sceneTime += dt;
            this->playLevelMusic(_sceneTime, SCENE_TIMING);
            if (!_shrimpSpawned) {
                if (_sceneTime <= SCENE_TIMING)
                    return;
                spawnShrimp();
                _shrimpSpawned = true;
            }
            bool inPosition = moveShrimpToScreen(_shrimpIdx, dt);
            if (inPosition) {
                _shootTiming += dt;
                if (_shootTiming >= 2.5f) {
                    _timeSinceLastSpawn += dt;
                    if (_projectilesSpawned < 8) {
                        if (_timeSinceLastSpawn >= 0.05f) {
                            spawnProjectile(_projectilesSpawned);
                            _timeSinceLastSpawn = 0.0f;
                            _projectilesSpawned++;
                        }
                    } else {
                        _shootTiming = 0.0f;
                        _projectilesSpawned = 0;
                        stopPlayingSoundMissile();
                    }
                }
                _waveOffset += 0.1f;
                if (_waveOffset >= 2.0f * M_PI) {
                    _waveOffset = 0.0f;
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        bool _shrimpSpawned;    
        float _sceneTime;        
        float _shootTiming;
        float _waveOffset;
        float _timeSinceLastSpawn;
        int _projectilesSpawned;
        size_t _shrimpIdx;
        bool _levelMusicStarted = false;
        bool _musicStarted = false;

        void spawnShrimp()
        {
            ECS::Entity shrimp = _registry.spawnEntity();
            _shrimpIdx = shrimp;

            Animatable animatable({
                {5, 7, true, true, 8, 0, 0.15f},
            });
            animatable.animationMap["Idle"] = 1;
            animatable.animationMap["Spawning"] = 2;
            _registry.addComponent<Animatable>(shrimp, animatable);
            _registry.addComponent<Transformable>(shrimp,
                Transformable{SHRIMP_SPAWN_X, 300.0f, 0.0f, 2.0f, 2.0f});
            _registry.addComponent<Velocity>(shrimp, Velocity{0.0f, 0.0f});
            _registry.addComponent<Renderable>(shrimp, {
                "../../client/assets/sprites/boss333.png",
                1280, 204, 160, 0.0f, 0, true, true, 160, 204, 0
            });
            _registry.addComponent<Color>(shrimp, {255.f, 255.f, 255.f, 255.f});
            _registry.addComponent<ClockComponent>(shrimp, ClockComponent{0.0f});
            _registry.addComponent<Health>(shrimp, Health{500, 500, false});
        }

        bool moveShrimpToScreen(size_t entityIdx, float dt)
        {
            auto &transformables = _registry.getComponents<Transformable>();
            if (entityIdx >= transformables.size() || !transformables[entityIdx].has_value())
                return false;

            auto &transform = *transformables[entityIdx];
            if (transform.positionX > SHRIMP_TARGET_X) {
                transform.positionX -= SHRIMP_MOVE_SPEED * dt;
                if (transform.positionX < SHRIMP_TARGET_X) {
                    transform.positionX = SHRIMP_TARGET_X;
                }
                return false;
            }
            ECS::Entity entity = _registry.entityFromIndex(entityIdx);
           _registry.addComponent<Collidable>(entity, Collidable{
            80.0f,
            50.0f,
            false,
            CollisionCategory::Enemy,
            static_cast<uint8_t>(CollisionCategory::PlayerMissile)
            });
            return true;
        }

            void playLevelMusic(float spawnTimer, float spawnTiming)
            {
                if (!_levelMusicStarted) {
                    ECS::Entity musicLvl1 = _registry.spawnEntity();
                    _registry.addComponent<Audio>(musicLvl1, {
                        "../../client/assets/musics/Level2.wav",
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
                        if (scene->currentScene == "LevelTwo" && !_musicStarted) {
                            for (auto &&[audio] : Zipper(audios)) {
                                if (audio.has_value()) {
                                    if (audio->audioPath == "../../client/assets/musics/Level2.wav") {
                                        audio->isPlaying = true;
                                    }
                                }
                            }
                        } else if (scene->currentScene == "LevelTwo" && spawnTimer >= spawnTiming) {
                            for (auto &&[audio] : Zipper(audios)) {
                                if (audio.has_value()) {
                                    if (audio->audioPath == "../../client/assets/musics/Level2.wav") {
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
                        if (audio->audioPath == "../../client/assets/musics/Boss-level2.ogg") {
                            audio->volume = 0.0f;
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

        void spawnProjectile(int index)
        {
            const float horizontalSpacing = 5.0f;
            const float waveAmplitude = 10.0f;
            const float speed = -800.0f;
            const int frameWidth = 24;
            const int frameHeight = 22;
            const int totalFrames = 4;

            float startX = 600.0f;
            float startY = 300.0f;
            ECS::Entity missile = _registry.spawnEntity();
            _registry.addComponent<Audio>(missile, {
                "../../client/assets/sounds/shrimp-shoot.wav",
                true,
                false,
                10.0f,
                0.7f
            });
            float posX = startX + index * horizontalSpacing;
            float posY = startY + waveAmplitude * std::sin(_waveOffset + index * 0.5f);
            _registry.addComponent<Transformable>(missile, {posX, posY, 0.0f, 1.5f, 1.5f});
            _registry.addComponent<Velocity>(missile, {speed, 0.0f});
            _registry.addComponent<Renderable>(missile, {
                "../../client/assets/sprites/projectiles-shrimp.png",
                frameWidth * totalFrames, frameHeight,
                0.0f, 0.0f,
                0, true, true,
                frameWidth, frameHeight,
                0
            });

            Animatable animatable({{0, totalFrames - 1, true, false, totalFrames, 0, 0.1f}});
            animatable.animationMap["Fly"] = 0;
            animatable.animState = "Fly";
            _registry.addComponent<Animatable>(missile, animatable);
            _registry.addComponent<ClockComponent>(missile, ClockComponent{0.0f});
            _registry.addComponent<Color>(missile, {255.f, 255.f, 255.f, 255.f});
            _registry.addComponent<Collidable>(missile, {
                17.5f, 11.0f, false, CollisionCategory::EnemyMissile,
                static_cast<uint8_t>(CollisionCategory::Player)
            });
            _registry.addComponent<Missile>(missile, Missile{1.0f});
        }

        void deleteMissile()
        {
            auto &missiles = _registry.getComponents<Missile>();
            auto &transformables = _registry.getComponents<Transformable>();
            auto &renderables = _registry.getComponents<Renderable>();
            auto &collidables = _registry.getComponents<Collidable>();

            for (auto &&[idx, miss, tran, rend, col] : indexedZipper(missiles, transformables, renderables, collidables)) {
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

        void stopPlayingSoundMissile()
        {
            auto &missiles = _registry.getComponents<Missile>();
            auto &audios   = _registry.getComponents<Audio>();

            for (auto &&[idx, miss, sou] : indexedZipper(missiles, audios)) {
                if (miss.has_value() && sou.has_value()) {
                    if (sou->audioPath == "../../client/assets/sounds/shrimp-shoot.wav") {
                        sou->isPlaying = false;
                    }
                }
            }
        }

        bool killShrimp(size_t entityIdx)
        {
            auto &collidables = _registry.getComponents<Collidable>();
            auto &health = _registry.getComponents<Health>();
            auto &renderables = _registry.getComponents<Renderable>();
            auto &scenes = _registry.getComponents<Scene>();
            auto &transformables = _registry.getComponents<Transformable>();

            if (collidables[entityIdx].has_value() && health[entityIdx].has_value() && renderables[entityIdx].has_value()) {
                if (health[entityIdx]->currentHealth <= 0) {
                    renderables[entityIdx]->visible = false;
                    bossExplosion(transformables[entityIdx]->positionX, transformables[entityIdx]->positionY);
                    for (auto &&[sce] : Zipper(scenes)) {
                        if (sce.has_value()) {
                            stopBossMusic();
                            sce->currentScene = "SceneTransition";
                            }
                    }
                    return true;
                }
                if (collidables[entityIdx]->isColliding) {
                    health[entityIdx]->currentHealth--;
                }
            }
            return false;
        }

        void bossExplosion(float x, float y)
        {
            auto explosionEntity = _registry.spawnEntity();
            _registry.addComponent<Transformable>(explosionEntity, Transformable{x, y, 0.0f, 3.0f, 3.0f});
            _registry.addComponent<Animatable>(explosionEntity, Animatable({{0, 5, false, false, 5, 0, 0.15f}}));
            _registry.addComponent<Renderable>(explosionEntity, Renderable{"../../client/assets/sprites/r-typesheet44-big-explosion.gif", 654, 136, 0.0f, 0.0f, 1, true, true, 0});
            _registry.addComponent<Color>(explosionEntity, {255.f, 255.f, 255.f, 255.f});
            _registry.addComponent<ClockComponent>(explosionEntity, ClockComponent{0.0f});
            _registry.addComponent<Explosion>(explosionEntity, {0.75f});
        }

    };
}
