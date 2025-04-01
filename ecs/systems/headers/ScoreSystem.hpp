#pragma once


#include "ISystems.hpp"
#include "Components.hpp"


namespace Systems
{
    class ScoreSystem : public ISystem
    {
        public:
            ScoreSystem(ECS::Registry &registry)
                : _registry(registry) {}

            void update(float dt) override
            {
                
                (void) dt;
                auto &text = _registry.getComponents<TextRenderable>();
                auto &scores = _registry.getComponents<Score>();
                for (auto &&[txt, sc] : Zipper(text, scores)) {
                    if (sc.has_value() && txt.has_value()) {  
                        if (sc->increase) {
                            sc->totalScore += sc->score;
                            sc->score = 0;
                            sc->increase = false;
                            txt->text = "Score " + std::to_string(sc->totalScore);
                        }
                    }
                }
            }
        
        private:
            ECS::Registry &_registry;
    };
}