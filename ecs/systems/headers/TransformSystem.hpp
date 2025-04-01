#pragma once

#include "ISystems.hpp" 

namespace Systems
{
    class TransformSystem : public ISystem
    {
        public:
            TransformSystem(ECS::SparseArray<Transformable> &transformables,
                            ECS::SparseArray<Velocity> &velocities,
                            ECS::SparseArray<Player> &player)
                : _transformables(transformables), _velocities(velocities), _player(player) {}

            void update(float dt) override
            {
                for (auto &&[i, trans, vel] : indexedZipper(_transformables, _velocities)) {
                    if (trans.has_value() && vel.has_value()) {
                        bool isPlayer = false;
                        for (auto &&[p, player] : indexedZipper(_player)) {
                            if (p == i) {
                                isPlayer = true;
                                break;
                            }
                        }
                        if (!isPlayer || (trans->positionX + vel->vx * dt > 0 && trans->positionX + vel->vx * dt < 800))
                            trans->positionX += vel->vx * dt;
                        if (!isPlayer || (trans->positionY + vel->vy * dt > 0 && trans->positionY + vel->vy * dt < 600))
                            trans->positionY += vel->vy * dt;
                        /*
                        we can add more complex transformations if we want after the basic ones like vel+=
                        */
                    }
                }
            }

        private:
            ECS::SparseArray<Transformable> &_transformables;
            ECS::SparseArray<Velocity> &_velocities;
            ECS::SparseArray<Player> &_player;
    };
}