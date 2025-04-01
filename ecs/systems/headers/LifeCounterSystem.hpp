#pragma once


#include "ISystems.hpp"
#include "Components.hpp"


namespace Systems
{
    class LifeCounterSystem : public ISystem
    {
        public:
            LifeCounterSystem(ECS::Registry &registry)
                : _registry(registry) {}

            void update(float dt) override
            {
                (void) dt;
                auto &text = _registry.getComponents<TextRenderable>();
                auto &healthpoint = _registry.getComponents<Health>();
                auto &player = _registry.getComponents<Player>();
                int playerCount = 0;

                for (auto &&[txt, hp, playr] : Zipper(text, healthpoint, player)) {
                    if (txt.has_value() && hp.has_value() && playr.has_value()) {
                        txt->text = "P" + std::to_string(playerCount + 1) + 
                                    " HP " + std::to_string(hp->currentHealth);

                        txt->x = 5;
                        txt->y = 10 + (playerCount * 30);
                        playerCount++;
                    }
                }
            }

        private:
            ECS::Registry &_registry;
    };
}
