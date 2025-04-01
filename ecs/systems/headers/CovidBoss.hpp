/*
** EPITECH PROJECT, 2025
** R-type--Mirror3
** File description:
** CovidBoss
*/

#pragma once

#include <cmath>
#include <random>
#include <algorithm>
#include <iostream>
#include "Components.hpp"
#include "ISystems.hpp"

namespace Systems
{
    constexpr float SPAWN_COVID = 63.0f;

    class CovShieldedBossSystem : public ISystem
    {
        public:
            CovShieldedBossSystem(ECS::Registry& registry, float& dt)
                : _registry(registry),
                _dt(dt),
                _spawnTimer(0.0f),
                _bossSpawned(false),
                _rng(std::random_device{}()),
                _moveInterval(2.0f, 4.0f),
                _radialDist(-1.0f, 1.0f)
            {}

            void update(float dt) override
            {
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[sce] : Zipper(scenes)) {
                    if (sce->currentScene != "LevelThree") {
                        return;
                    }
                }

                _spawnTimer += dt;
                if (_spawnTimer >= SPAWN_COVID && !_bossSpawned) {
                    std::cout << "Spawning boss..." << std::endl;
                    spawnBoss();
                    _bossSpawned = true;
                    return;
                }

                updateBoss(dt);
                updateOrbState(dt);
                updateOrb(dt);
            }

        private:
            ECS::Registry &_registry;
            float &_dt;
            float _spawnTimer;
            bool _bossSpawned;
            std::mt19937 _rng;
            std::uniform_real_distribution<float> _moveInterval;
            float _directionTimer = 0.0f;
            std::uniform_real_distribution<float> _radialDist;

            void getBossPosition(float& x, float& y)
            {
                auto &transforms = _registry.getComponents<Transformable>();
                auto &bosses = _registry.getComponents<ShieldedBossComponent>();

                for (auto&& [idx, transform, boss] : indexedZipper(transforms, bosses)) {
                    if (!transform.has_value()) 
                        continue;

                    x = transform->positionX;
                    y = transform->positionY;
                    break;
                }
            }

            bool getPlayerPosition(float& x, float& y)
            {
                auto& transforms = _registry.getComponents<Transformable>();
                auto& players = _registry.getComponents<Player>();

                for (auto&& [idx, transform, player] : indexedZipper(transforms, players)) {
                    if (!transform.has_value()) 
                        continue;

                    x = transform->positionX;
                    y = transform->positionY;
                    return true;
                }
                return false;
            }

            void spawnBoss()
            {
                auto bossEntity = _registry.spawnEntity();
                _registry.addComponent<Velocity>(bossEntity, Velocity{50.0f, 50.0f});
                Animatable animatable({{0, 1, false, false, 1, 0, 0.08f}});
                animatable.animationMap["Idle"] = 1;
                _registry.addComponent<Animatable>(bossEntity, animatable);
                _registry.addComponent<ShieldedBossComponent>(bossEntity, ShieldedBossComponent{});
                _registry.addComponent<Renderable>(bossEntity, Renderable{"../../client/assets/sprites/r-typesheet39.gif", 134, 67, 0.0f, 0.0f, 1, true, true, 0});
                _registry.addComponent<Color>(bossEntity, {255.f, 255.f, 255.f, 255.f});
                _registry.addComponent<Transformable>(bossEntity, Transformable{940.0f, 400.0f, 0.0f, 2.0f, 2.0f});
                _registry.addComponent<ClockComponent>(bossEntity, ClockComponent{0.0f});
                _registry.addComponent<Health>(bossEntity, Health{12, 12, false});
                _registry.addComponent<Collidable>(bossEntity, Collidable{66.0f, 58.0f, CollisionCategory::Enemy, static_cast<uint8_t>(CollisionCategory::PlayerMissile)});
                spawnBossOrbs();
            }

            void spawnBossOrbs()
            {
                constexpr int ORB_COUNT = 30;
                constexpr float ORB_RADIUS = 125.0f;

                float bossX = 0.0f, bossY = 0.0f;
                getBossPosition(bossX, bossY);

                Animatable orbAnim({{0, 4, true, false, 4, 0, 0.08f}});
                orbAnim.animationMap["Idle"] = 1;

                for (int i = 0; i < ORB_COUNT; ++i) {
                    float angle = 2.0f * M_PI * (static_cast<float>(i) / ORB_COUNT);
                    float radius = ORB_RADIUS * std::sqrt(static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX));

                    ProtectiveOrbState orbState(angle);
                    orbState.currentDistance = radius;

                    float xPos = bossX + std::cos(angle) * radius;
                    float yPos = bossY + std::sin(angle) * radius;

                    auto orbEntity = _registry.spawnEntity();
                    _registry.addComponent<ProtectiveOrbState>(orbEntity, orbState);
                    _registry.addComponent<Animatable>(orbEntity, orbAnim);
                    _registry.addComponent<Transformable>(orbEntity, Transformable{xPos, yPos, 0.0f, 1.5f, 1.5f});
                    _registry.addComponent<Velocity>(orbEntity, Velocity{0.0f, 0.0f});
                    _registry.addComponent<Renderable>(orbEntity, Renderable{"../../client/assets/sprites/r-typesheet39orbs-big.gif", 201, 50, 0.0f, 0.0f, 1, true, true, 0});
                    _registry.addComponent<Color>(orbEntity, {255.f, 255.f, 255.f, 255.f});
                    _registry.addComponent<ClockComponent>(orbEntity, ClockComponent{0.0f});
                    _registry.addComponent<Health>(orbEntity, Health{12, 12, false});
                    _registry.addComponent<Collidable>(
                        orbEntity,
                        Collidable{20.0f, 20.0f, CollisionCategory::Enemy,
                        static_cast<uint8_t>(CollisionCategory::PlayerMissile)}
                    );
                    MultiCollidable multiColl;
                    multiColl.collidables.push_back(Collidable{
                        20.0f, 20.0f, false,
                        CollisionCategory::EnemyMissile,
                        static_cast<uint8_t>(CollisionCategory::Player)
                    });
                    _registry.addComponent<MultiCollidable>(orbEntity, multiColl);
                }
            }

            void updateBoss(float dt)
            {
                auto &transforms = _registry.getComponents<Transformable>();
                auto &velocities = _registry.getComponents<Velocity>();
                auto &bosses = _registry.getComponents<ShieldedBossComponent>();
                auto &collidables = _registry.getComponents<Collidable>();
                auto &healths = _registry.getComponents<Health>();
                auto &renderables = _registry.getComponents<Renderable>();

                for (auto&& [idx, transform, velocity, boss, hp, col, rend] : indexedZipper(transforms, velocities, bosses, healths, collidables, renderables)) {
                    if (!transform.has_value() || !velocity.has_value() || !boss.has_value() || !hp.has_value() || !col.has_value() || !rend.has_value())
                        continue;
                    if (!rend->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                    }

                    if (col->isColliding) {
                        bool shielded = false;
                        auto &orbs = _registry.getComponents<ProtectiveOrbState>();
                        for (auto &&[oIdx, ob, orbrend] : indexedZipper(orbs, renderables)) {
                            if (ob.has_value() && orbrend->visible) {
                                shielded = true;
                            }
                        }
                        if (!shielded) {
                            hp->currentHealth--;
                        }
                    }
                    auto &scenes = _registry.getComponents<Scene>();

                    if (hp->currentHealth <= 0) {
                        auto &scores = _registry.getComponents<Score>();
                        for (auto &&[sIdx, sc] : indexedZipper(scores)) {
                            if (sc.has_value()) {
                                sc->score = 50;
                                sc->increase = true;
                            }
                        }
                        rend->visible = false;
                        bossExplosion(transform->positionX, transform->positionY);
                        for (auto &&[sce] : Zipper(scenes)) {
                            if (sce.has_value()) {
                                sce->currentScene = "SceneTransition";
                            }
                        }
                    }

                    transform->positionX += velocity->vx * dt;
                    transform->positionY += velocity->vy * dt;

                    if (transform->positionY > 400.0f) {
                        velocity->vy = -std::abs(velocity->vy);
                    } else {
                        if (transform->positionY <= 100.0f || transform->positionY >= 400.0f) {
                            velocity->vy *= -1.0f;
                            transform->positionY = std::clamp(transform->positionY, 100.0f, 400.0f);
                        }
                    }

                    if (transform->positionX > 600.0f) {
                        velocity->vx = -std::abs(velocity->vx);
                    } else {
                        if (transform->positionX <= 200.0f || transform->positionX >= 600.0f) {
                            velocity->vx *= -1.0f;
                            transform->positionX = std::clamp(transform->positionX, 200.0f, 600.0f);
                        }
                    }
                }
            }

            void updateOrbState(float dt)
            {
                static float attackTimer = 0.0f;
                attackTimer += dt;

                float playerX = 0.0f, playerY = 0.0f;
                if (!getPlayerPosition(playerX, playerY))
                    return;

                if (attackTimer >= 1.0f) {
                    attackTimer = 0.0f;

                    auto& orbs = _registry.getComponents<ProtectiveOrbState>();
                    auto& transforms = _registry.getComponents<Transformable>();
                    float minDistance = std::numeric_limits<float>::max();
                    int closestOrbIndex = -1;

                    for (auto&& [idx, orb, transform] : indexedZipper(orbs, transforms)) {
                        if (!orb.has_value() || !transform.has_value())
                            continue;
                        if (!orb->isDetached && !orb->isAttacking && !orb->isReturning) {
                            float dx = playerX - transform->positionX;
                            float dy = playerY - transform->positionY;
                            float dist = std::sqrt(dx * dx + dy * dy);
                            if (dist < minDistance) {
                                minDistance = dist;
                                closestOrbIndex = static_cast<int>(idx);
                            }
                        }
                    }
                    if (closestOrbIndex != -1) {
                        orbs[closestOrbIndex]->isDetached = true;
                    }
                }
            }

            void updateOrb(float dt)
            {
                auto &transforms = _registry.getComponents<Transformable>();
                auto &velocities = _registry.getComponents<Velocity>();
                auto &orbs = _registry.getComponents<ProtectiveOrbState>();
                auto &collidables = _registry.getComponents<Collidable>();
                auto &renderables = _registry.getComponents<Renderable>();
                auto &health = _registry.getComponents<Health>();
                float bossX = 0.0f, bossY = 0.0f;
                float ATTACK_SPEED = 200.0f;
                float RETURN_SPEED = 200.0f;

                getBossPosition(bossX, bossY);

                for (auto&& [idx, tran, velocity, orb, col, rend, hp] : indexedZipper(transforms, velocities, orbs, collidables, renderables, health)) {
                    if (!tran.has_value() || !velocity.has_value() || !orb.has_value() || !col.has_value() || !rend.has_value() || !hp.has_value())
                        continue;
                    if (!rend->visible) {
                        _registry.killEntity(_registry.entityFromIndex(idx));
                    }
                    if (col->isColliding) {
                        hp->currentHealth--;
                        orb->isAttacking = false;
                        orb->isReturning = true;
                        col->isColliding = false;
                    }
                    if (hp->currentHealth <= 0) {
                        rend->visible = false;
                        orbExplosion(tran->positionX, tran->positionY);
                    }

                    if (!orb->isDetached && !orb->isAttacking && !orb->isReturning) {
                        tran->positionX = bossX + std::cos(orb->originalAngle) * orb->currentDistance;
                        tran->positionY = bossY + std::sin(orb->originalAngle) * orb->currentDistance;
                        velocity->vx = 0.0f;
                        velocity->vy = 0.0f;
                        continue;
                    }
                    if (orb->isDetached && !orb->isAttacking && !orb->isReturning) {
                        std::vector<std::pair<float, float>> playerPositions;
                        auto &playerTransforms = _registry.getComponents<Transformable>();
                        auto &playerComponents = _registry.getComponents<Player>();
                        for (auto&& [pidx, ptrans, pcomp] : indexedZipper(playerTransforms, playerComponents)) {
                            if (!ptrans.has_value()) continue;
                            playerPositions.emplace_back(ptrans->positionX, ptrans->positionY);
                        }
                        if (!playerPositions.empty()) {
                            static int nextPlayerIndex = 0;
                            auto [targetX, targetY] = playerPositions[nextPlayerIndex % playerPositions.size()];
                            nextPlayerIndex++;
                            orb->targetX = targetX;
                            orb->targetY = targetY;
                        }
                        orb->isAttacking = true;
                    }
                    if (orb->isAttacking) {
                        float dx = orb->targetX - tran->positionX;
                        float dy = orb->targetY - tran->positionY;
                        float dist = std::sqrt(dx * dx + dy * dy);

                        if (dist > 1.0f) {
                            velocity->vx = (dx / dist) * ATTACK_SPEED;
                            velocity->vy = (dy / dist) * ATTACK_SPEED;
                        } else {
                            orb->isAttacking = false;
                            orb->isReturning = true;
                            velocity->vx = 0.0f;
                            velocity->vy = 0.0f;
                        }
                    }
                    if (orb->isReturning) {
                        float finalX = bossX + std::cos(orb->originalAngle) * orb->currentDistance;
                        float finalY = bossY + std::sin(orb->originalAngle) * orb->currentDistance;

                        float dx = finalX - tran->positionX;
                        float dy = finalY - tran->positionY;
                        float dist = std::sqrt(dx * dx + dy * dy);
                        if (dist > 1.0f) {
                            velocity->vx = (dx / dist) * RETURN_SPEED;
                            velocity->vy = (dy / dist) * RETURN_SPEED;
                        } else {
                            orb->isReturning = false;
                            orb->isDetached  = false;
                            velocity->vx = 0.0f;
                            velocity->vy = 0.0f;
                        }
                    }
                    if (orb->isDetached || orb->isAttacking || orb->isReturning) {
                        tran->positionX += velocity->vx * dt;
                        tran->positionY += velocity->vy * dt;
                    }
                }
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

            void orbExplosion(float x, float y)
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