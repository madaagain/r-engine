#pragma once

#include "Components.hpp"
#include "Entity.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"
#include <cstdlib>
#include <iterator>

namespace Systems
{

    class CreditSceneSystem : public ISystem 
    {
        public:
            CreditSceneSystem(ECS::Registry &registry, float &dt) : _registry(registry), _dt(dt), _creditTiming(0.0f), _scrollSpeed(1.0f) {}

            void update(float /*DT*/) override
            {
                const std::string TEXT_CREDIT = "Thanks for playing RTYPE!\n\n\n\nWe hope you enjoyed\n\n\n\nthe game and give us\n\n\n\na decent rating !";
               
                auto &scenes = _registry.getComponents<Scene>();
                for (auto &&[sce] : Zipper(scenes)) {
                    if (sce.has_value()) {
                        if (sce->currentScene == "CreditScene") {
                            if (!_textSpawned) {
                                createCredits(TEXT_CREDIT);
                                _textSpawned = true;
                            }
                            updateCreditsPosition();
                        } else {    
                            _textSpawned = false;
                        }
                    }
                }
            }

        private:
            ECS::Registry &_registry;
            float &_dt;
            bool _textSpawned = false;
            float _creditTiming;
            float _scrollSpeed;

            ECS::Entity createCredits(const std::string &text)
            {
                ECS::Entity creditText = _registry.spawnEntity();
                _registry.addComponent<TextRenderable>(creditText, TextRenderable{text, text, "../../client/assets/fonts/r-type.ttf",
                20, 200, 600, 1, true, 255.0, 255.0, 255.0, 255.0});
                return creditText;
            }

            void updateCreditsPosition()
            {
                auto &texts = _registry.getComponents<TextRenderable>();
                for (auto &&[text] : Zipper(texts)) {
                    if (text.has_value()) {
                        if (text->y > 200) { 
                            text->y -= _scrollSpeed;
                        }
                    }
                }
            }
    };
}
