#pragma once

#include "ISystems.hpp"
#include <iostream>

namespace Systems
{
    struct InputState
    {
        size_t playerId;
        bool isKeyboard;
        bool moveLeft;
        bool moveRight;
        bool moveUp;
        bool moveDown;
        bool attack;
        bool fKey;
        InputState(
            int playerId,
            bool isKeyboard,
            bool moveLeft,
            bool moveRight,
            bool moveUp,
            bool moveDown,
            bool attack,
            bool fKey
        ) : playerId(playerId), isKeyboard(isKeyboard), moveLeft(moveLeft), moveRight(moveRight), moveUp(moveUp), moveDown(moveDown), attack(attack), fKey(fKey) {}
    };

    class InputSystem : public ISystem
    {
        public:
            InputSystem(ECS::Registry &registry,
                        ECS::SparseArray<Controllable> &controls,
                        ECS::SparseArray<Velocity> &velocities,
                        ECS::SparseArray<Animatable> &animatables,
                        const InputState &inputState, float &dt)
                : _registry(registry),
                  _controls(controls),
                  _velocities(velocities),
                  _animatables(animatables),
                  _inputState(inputState),
                  _dt(dt) {}

            void update(float dt) override
            {
                (void) dt;
                auto &players = _registry.getComponents<Player>();

                for (auto &&[i, inp, vel, pla] : indexedZipper(_controls, _velocities, players)) {
                    if (inp.has_value() && vel.has_value() && pla.has_value()) {
                        if (pla->playerId == _inputState.playerId) {
                            inp->moveLeft = _inputState.moveLeft;
                            inp->moveRight = _inputState.moveRight;
                            inp->moveDown = _inputState.moveDown;
                            inp->moveUp = _inputState.moveUp;
                            inp->attack = _inputState.attack;
                            inp->fKey = _inputState.fKey; // <-- add this line
                            inp->clientId = static_cast<uint32_t>(_inputState.playerId);
                            
                        }
                    }
                }
            }

        private:
            ECS::Registry &_registry;
            ECS::SparseArray<Controllable> &_controls;
            ECS::SparseArray<Velocity> &_velocities;
            ECS::SparseArray<Animatable> &_animatables;
            const InputState &_inputState;
            float &_dt;
    };
}
