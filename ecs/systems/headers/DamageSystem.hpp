
#pragma once

#include "ISystems.hpp"
#include "Components.hpp"
#include <iostream>

namespace Systems
{
    class DamageSystem : public ISystem
    {
    public:
        DamageSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt), _invicibilityTimer(0.0f), _auraSpawned(false) {}

        void update(float dt) override
        {
            auto &hps = _registry.getComponents<Health>();
            auto &collidables = _registry.getComponents<MultiCollidable>();
            auto &renderables = _registry.getComponents<Renderable>();
            auto &transformables = _registry.getComponents<Transformable>();
            _invicibilityTimer -= dt;
            _invicibilityTimer -= dt;
            for (auto &&[idx, hp, col, ren, tran] :  indexedZipper(hps, collidables, renderables, transformables)) {
                if (hp.has_value() && col.has_value() && ren.has_value() && tran.has_value()) {
                    if (!ren->visible) {
                        // _registry.killEntity(_registry.entityFromIndex(idx));
                        continue;
                    }
                    if (hp->currentHealth <= 0) {
                        ren->visible = false;
                        explosion(tran->positionX, tran->positionY);
                    }
                    for (auto &coll : col->collidables) {
                        if (coll.isColliding && coll.category == CollisionCategory::Player && canCollide(coll.bitMask, CollisionCategory::Heal) && hp->currentHealth != 0) {
                            hp->currentHealth++;
                            if (hp->currentHealth > hp->maxHealth)
                                hp->currentHealth = hp->maxHealth;
                            coll.isColliding = false;
                        }
                        if (coll.isColliding && coll.category == CollisionCategory::Player && canCollide(coll.bitMask, CollisionCategory::ShieldBonus)) {
                            _invicibilityTimer = 5.0f;
                            if (_auraSpawned == false && !hp->isInvicible && ren->visible) {
                                _auraSpawned = true;
                                spawnAura(idx);
                            } else {
                                _auraSpawned = false;
                            }
                            hp->isInvicible = true;
                        }
                        if (!hp->isInvicible && coll.isColliding &&
                            coll.category == CollisionCategory::Player &&
                            canCollide(coll.bitMask, CollisionCategory::EnemyMissile)) {
                            hp->currentHealth--;
                            coll.isColliding = false;
                        }
                        if (!hp->isInvicible && coll.isColliding &&
                            coll.category == CollisionCategory::EnemyMissile &&
                            canCollide(coll.bitMask, CollisionCategory::Player)) {
                            // hp->currentHealth--;
                            // coll.isColliding = false;
                        }
                        if (_invicibilityTimer <= 0.0f && hp->isInvicible) {
                            std::cout << "Not invicible anymore\n";
                            hp->isInvicible = false;
                        }
                    }
                }
            }
            upadateShieldPos();
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        float _invicibilityTimer;
        bool _auraSpawned;

        void spawnAura(size_t idx)
        {
            auto &transformables = _registry.getComponents<Transformable>();
            auto &player = _registry.getComponents<Player>();
            std::cout << "Spawning Aura\n";
            if (transformables[idx].has_value() && player[idx].has_value()) {
                float spawnX = transformables[idx]->positionX;
                float spawnY = transformables[idx]->positionY;
                ECS::Entity auraShield = _registry.spawnEntity();
                _registry.addComponent<Renderable>(auraShield, Renderable{"../../client/assets/sprites/plshield.png", 40, 40, 0.0f, 0.0f, 1, true, true, 40, 40, 0});
                _registry.addComponent<PlayerShield>(auraShield, player[idx]->playerId);
                _registry.addComponent<Transformable>(auraShield, Transformable{spawnX, spawnY, 0.0f, 2.4f, 2.4f});
            }
        }

        void upadateShieldPos()
        {
            auto &player = _registry.getComponents<Player>();
            auto &playerShields = _registry.getComponents<PlayerShield>();
            auto &transformables = _registry.getComponents<Transformable>();
            auto &renderables = _registry.getComponents<Renderable>();
            for (auto &&[idx, sh, tr, rend] : indexedZipper(playerShields, transformables, renderables)) {
                if (sh.has_value() && tr.has_value()) {
                    for (auto &&[idx2, pl] : indexedZipper(player)) {
                        if (pl.has_value() && pl->playerId == sh->ownerId) {
                            tr->positionX = transformables[idx2]->positionX;
                            tr->positionY = transformables[idx2]->positionY;
                        }
                    }
                    if (_invicibilityTimer <= 0.0f) {
                        rend->visible = false;
                    }
                }
            }
        }

        void explosion(float x, float y)
        {
            auto explosionEntity = _registry.spawnEntity();
            _registry.addComponent<Transformable>(explosionEntity, Transformable{x, y, 0.0f, 1.0f, 1.0f});
            _registry.addComponent<Animatable>(explosionEntity, Animatable({{1, 6, false, false, 6, 0, 0.15f}}));
            _registry.addComponent<Renderable>(explosionEntity, Renderable{"../../client/assets/sprites/r-typesheet43-big-explosion.gif", 400, 68, 0.0f, 0.0f, 1, true, true, 0});
            _registry.addComponent<Color>(explosionEntity, {255.f, 255.f, 255.f, 255.f});
            _registry.addComponent<ClockComponent>(explosionEntity, ClockComponent{0.0f});
            _registry.addComponent<Explosion>(explosionEntity, {0.6f});
        }
    };
}
