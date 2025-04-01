#pragma once

#include "ISystems.hpp"

namespace Systems
{
    class ClockSystem : public ISystem
    {
        public:
            ClockSystem(ECS::SparseArray<ClockComponent> &clocks)
                : _clocks(clocks) {}

            void update(float dt) override
            {
                for (auto &&[clock] : Zipper(_clocks)) {
                    if (clock.has_value()) {
                        clock->dt = dt; 
                    }
                }
            }

        private:
            ECS::SparseArray<ClockComponent> &_clocks;
    };
}
