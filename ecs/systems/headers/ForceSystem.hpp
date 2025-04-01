

#pragma once

#include "ISystems.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include <cmath>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <random>
#include <string>

namespace Systems
{
    class ForceSystem : public ISystem
    {
    public:
        ForceSystem(ECS::Registry &registry, float &dt)
            : _registry(registry),
              _dt(dt),
              _forceSpawned(false),
              _driftTimer(0.f),
              _currentDriftX(0.f),
              _currentDriftY(0.f),
              _rng(std::random_device{}()),
              _driftDist(-30.f, 30.f)
        {}

        void update(float dt) override
        {
            (void) dt;
            if (!_forceSpawned && !forceExists()) {
                spawnForce();
                _forceSpawned = true;
            }
            toggleForceAttachment();
            updateForceMovement();
            forceShooting();
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        bool _forceSpawned;
        std::unordered_map<size_t, bool> _forceShotState;
        std::unordered_map<size_t, uint32_t> _forceLastOwner;
        float _driftTimer;
        float _currentDriftX;
        float _currentDriftY;
        std::mt19937 _rng;
        std::uniform_real_distribution<float> _driftDist;

        bool forceExists()
        {
            auto &forces = _registry.getComponents<Force>();
            for (auto &&[force] : Zipper(forces))
                if (force.has_value())
                    return true;
            return false;
        }

        void spawnForce()
        {
            ECS::Entity forceEnt = _registry.spawnEntity();
            _registry.addComponent<Force>(forceEnt, Force{false, false, false});
            _registry.addComponent<Transformable>(forceEnt, Transformable{100.f, 300.f, 0.f, 2.0f, 2.0f});
            Animatable animatable({
                {0, 6, true, false, 6, 0, 0.10f}
                // {6, 11, true, false, 12, 0, 0.10f},
            });
            animatable.animationMap["Backward"] = 1;
            animatable.animationMap["Forward"] = 0;
            _registry.addComponent<Animatable>(forceEnt, animatable);
            _registry.addComponent<Renderable>(forceEnt, Renderable{
                "../../client/assets/sprites/force-back.gif",
                180, 22, 0.f, 0.f, 1, true, true, 30, 22, 0
            });
            _registry.addComponent<ClockComponent>(forceEnt, ClockComponent{0.0f});
            _registry.addComponent<Color>(forceEnt, Color{255.f, 255.f, 255.f, 255.f});
        }

        void toggleForceAttachment()
        {
            auto &forces = _registry.getComponents<Force>();
            auto &controls = _registry.getComponents<Controllable>();
            for (auto &&[fIdx, force] : indexedZipper(forces)) {
                if (!force.has_value())
                    continue;
                bool fKeyPressed = false;
                for (auto &&[control] : Zipper(controls)) {
                    if (!control.has_value())
                        continue;
                    if (control->fKey) {
                        if (_forceLastOwner.find(fIdx) == _forceLastOwner.end() ||
                            _forceLastOwner[fIdx] != control->clientId) {
                            _forceLastOwner[fIdx] = control->clientId;
                        }
                        fKeyPressed = true;
                        break;
                    }
                }
                if (fKeyPressed)
                    force->isAttached = !force->isAttached;
            }
        }

        void updateForceMovement()
        {
            auto &forces = _registry.getComponents<Force>();
            auto &transforms = _registry.getComponents<Transformable>();
            auto &players = _registry.getComponents<Player>();
            auto &renderables = _registry.getComponents<Renderable>();

            for (auto &&[fIdx, force, rend] : indexedZipper(forces, renderables)) {
                if (!force.has_value())
                    continue;

                if (force->isAttached) {
                    size_t nearestPlayerIdx = SIZE_MAX;
                    float minDist = std::numeric_limits<float>::max();
                    for (auto &&[pIdx, control, playerTran] : indexedZipper(players, transforms)) {
                        if (!playerTran.has_value())
                            continue;
                        auto &playerTrans = *playerTran;
                        float dx = getForceTransform(fIdx).positionX - playerTrans.positionX;
                        float dy = getForceTransform(fIdx).positionY - playerTrans.positionY;
                        float dist = std::sqrt(dx * dx + dy * dy);
                        if (dist < minDist) {
                            minDist = dist;
                            nearestPlayerIdx = pIdx;
                        }
                    }
                    if (nearestPlayerIdx != SIZE_MAX && minDist < 150.f) {
                        auto &playerTrans = *transforms[nearestPlayerIdx];
                        if (getForceTransform(fIdx).positionX >= playerTrans.positionX) {
                            force->isAttachedFront = true;
                            transforms[fIdx]->positionX = playerTrans.positionX + 55.f;
                            transforms[fIdx]->rotation = 0.f;
                            auto &anim = _registry.getComponents<Animatable>()[fIdx];
                            if (anim.has_value() && anim->animState == "Forward") {
                                // anim->animState = "Backward";
                                rend->textureId = "../../client/assets/sprites/force-back.gif";


                            }

                        } else {
                            force->isAttachedFront = false;
                            transforms[fIdx]->positionX = playerTrans.positionX - 55.f;
                            transforms[fIdx]->rotation = 180.f;
                            auto &anim = _registry.getComponents<Animatable>()[fIdx];
                            if (anim.has_value()) {
                                // anim->animState = "Forward";
                                rend->textureId = "../../client/assets/sprites/force-front.gif";
                            }
                        }
                        transforms[fIdx]->positionY = playerTrans.positionY;
                    } else {
                        force->isAttached = false;
                    }
                } else {
                    _driftTimer += _dt;
                    if (_driftTimer >= 2.0f) {
                        _currentDriftX = _driftDist(_rng);
                        _currentDriftY = _driftDist(_rng);
                        _driftTimer = 0.f;
                    }
                    auto &t = getForceTransform(fIdx);
                    t.positionX += _currentDriftX * _dt;
                    t.positionY += _currentDriftY * _dt;
                }
            }
        }

        void forceShooting()
        {
            auto &forces = _registry.getComponents<Force>();
            auto &transforms = _registry.getComponents<Transformable>();
            auto &controls = _registry.getComponents<Controllable>();

            for (auto &&[fIdx, force] : indexedZipper(forces)) {
                if (!force.has_value())
                    continue;
                if ( (force->isAttached == false) && (_forceLastOwner.find(fIdx) == _forceLastOwner.end()))
                    continue;
                if (fIdx >= transforms.size() || !transforms[fIdx].has_value())
                    continue;
                bool &lastShootState = _forceShotState[fIdx];
                uint32_t ownerId = _forceLastOwner[fIdx];
                bool ownerAttacking = false;
                for (auto &&[control] : Zipper(controls)) {
                    if (!control.has_value())
                        continue;
                    if (control->clientId == ownerId && control->attack) {
                        ownerAttacking = true;
                        break;
                    }
                }
                if (ownerAttacking && !lastShootState) {
                    spawnForceMissile(getForceTransform(fIdx), *force, fIdx);
                    lastShootState = true;
                } else if (!ownerAttacking && lastShootState) {
                    lastShootState = false;
                }
            }
        }

        void spawnForceMissile(const Transformable &forceTrans, const Force &fComp, size_t fIdx)
        {
            float offsetX = fComp.isAttachedFront ? +20.f : -20.f;
            float missileSpeed = fComp.isAttachedFront ? +650.f : -650.f;
            float spawnX = forceTrans.positionX + offsetX;
            float spawnY = forceTrans.positionY;

            std::string texture = "";
            if (fComp.isAttachedFront)
                texture = "../../client/assets/sprites/force-missiles-front.gif";
            else
                texture = "../../client/assets/sprites/force-missiles-back.gif";

            ECS::Entity missile = _registry.spawnEntity();
            _registry.addComponent<Transformable>(missile, Transformable{spawnX, spawnY, 0.f, 1.5f, 1.5f});
            _registry.addComponent<Velocity>(missile, Velocity{missileSpeed, 0.f});
            _registry.addComponent<Renderable>(missile, Renderable{
                texture, 70, 44, 0.f, 0.f, 0, true, true, 35, 44, 0
            });
            Animatable animatable({{0, 2, true, false, 2, 0, 0.20f}});
            animatable.animationMap["Idle"] = 1;
            _registry.addComponent<Animatable>(missile, animatable);
            uint32_t owner = 0;
            if (_forceLastOwner.find(fIdx) != _forceLastOwner.end())
                owner = _forceLastOwner[fIdx];
            _registry.addComponent<Missile>(missile, Missile{10.f, owner});
            _registry.addComponent<Collidable>(missile, Collidable{
                17.5f, 11.f, false,
                CollisionCategory::PlayerMissile,
                static_cast<uint8_t>(CollisionCategory::Enemy)
            });
        }

        Transformable &getForceTransform(size_t fIdx)
        {
            auto &transforms = _registry.getComponents<Transformable>();
            return *transforms[fIdx];
        }
    };
}
