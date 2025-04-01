#pragma once

#include "Components.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"

namespace Systems
{
    class SceneSystem : public ISystem
    {
        public:
            SceneSystem(ECS::Registry &registry, float &dt)
                : _registry(registry), _dt(dt) {}
        

            void update(float dt) override
            {
                (void)dt;
                /*
                
                    SceneManager ?
                    if (scene == "Menu")
                        logic(initMenu)
                    if (scene == "LvlOne")
                        logic(initLvlOne)

                    que contient une scène ?
                     -Le registry avec les entités
                     - faire attention à garder les joueurs actuels si on cleanup le registry
                     - On cleanup les IA, et le décor mais on garde le score et les joueurs

                     une scene = un lvl

                     Une queue/stack
                     1) on add le lvl 1 qui init le registry du lvl 1

                    Il faut faire une factory de scenes, les scenes sont des classes à part entière ?
                    
                */
                auto &sce = _registry.getComponents<Scene>();
                for (auto &&[scene] : Zipper(sce)) {

                }
            }

        private:
            ECS::Registry &_registry;
            float &_dt;
    };
}