#pragma once

#include <iostream>
#include "ISystems.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include "SparseArray.hpp"
#include "ZipperIterator.hpp"
#include "IndexedZipper.hpp"

namespace Systems
{
    class AudioSystem : public ISystem
    {
    public:
        AudioSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt) {}

        void update(float dt) override
        {
            (void)dt;
            auto &sounds = _registry.getComponents<Audio>();
            for (auto &&[idx, sound] : indexedZipper(sounds)) {
                if (sound.has_value()) {
                    if (sound->isPlaying) {
                        std::cout << "Hello" << std::endl;
                       if(!sound->loop) {
                        std::cout << "zedzedzedHello" << std::endl;
                        sound->isPlaying = false;
                       } 
                    }
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
    };
}
