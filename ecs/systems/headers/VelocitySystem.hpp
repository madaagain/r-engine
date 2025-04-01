#pragma once

#include "ISystems.hpp"

namespace Systems
{
    class VelocitySystem : public ISystem
    {
        public:
            VelocitySystem(ECS::SparseArray<Velocity> &velocities, ECS::SparseArray<Controllable> &controllables, float &dt) 
                : _velocities(velocities), _controllables(controllables), _dt(dt){}

            void update(float dt) override
            {
                (void) dt;
                for (auto &&[con, vel] : Zipper(_controllables, _velocities)) {
                    if (con.has_value() && vel.has_value()){
                        vel->vx = 0.0f;
                        vel->vy = 0.0f;
                        if (con->moveLeft){
                            vel->vx -= 500.0f;
                        }
                        if (con->moveRight){
                            vel->vx += 500.0f;
                        }
                        if (con->moveDown){
                            vel->vy += 500.0f;
                        }
                        if (con->moveUp){
                            vel->vy -= 500.0f;
                        }
                    }
                }
            }

        private:
            ECS::SparseArray<Velocity> &_velocities;
            ECS::SparseArray<Controllable> &_controllables;
            float &_dt;
    };
}