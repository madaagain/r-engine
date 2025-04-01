#pragma once

#include "Registry.hpp"
#include "Entity.hpp"
#include "ZipperIterator.hpp"
#include "IndexedZipper.hpp"
#include "Components.hpp"
#include <cstdlib>

namespace Systems
{
    class ISystem
    {
        public:
            virtual ~ISystem() = default;
            virtual void update(float dt) = 0;
    };
}
