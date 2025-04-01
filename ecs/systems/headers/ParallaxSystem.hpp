#pragma once

#include "ISystems.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <filesystem>

namespace Systems
{
    struct ParallaxLevelConfig
    {
        std::vector<ParallaxScroller> layers;
    };

    class ParallaxSystem : public ISystem
    {
    public:
        ParallaxSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt), _currentScene("")
        {
            initLevelConfigs();
        }

        void update(float dt) override
        {
            (void)dt;
            auto &scenes = _registry.getComponents<Scene>();

            for (auto &&[scene] : Zipper(scenes))
            {
                if (!scene.has_value())
                    continue;

                if (_currentScene != scene->currentScene) {
                    _currentScene = scene->currentScene;
                    setLevel(_currentScene);
                }
                if (scene->nextParallax.size() == 1) {
                    std::cout << "[ParallaxSystem] Switching background to: "
                              << scene->nextParallax << std::endl;
                    setLevel(scene->nextParallax);
                }
            }
            auto &parallaxes = _registry.getComponents<ParallaxScroller>();
            for (auto &&[par] : Zipper(parallaxes)) {
                if (!par.has_value())
                    continue;
                par->offsetX -= par->scrollSpeedX;
                par->offsetY -= par->scrollSpeedY;

                if (par->offsetX < -400) { // change rand spawn
                    if (par->zLayer == 2)
                        par->offsetX = 800;
                    else {
                        par->offsetX = static_cast<float>(600 + std::rand() % 801); // rand change spawn
                        par->offsetY = static_cast<float>(std::rand() % 601);
                        par->rotation = std::rand() % 360;
                        par->scrollSpeedX = static_cast<float>(std::rand() % 200 + 3) / 1000.0f;
                        par->scale = static_cast<float>(std::rand() % 500 + 200) / 100;
                    }
                }
                if (par->offsetY < -600) {
                    par->offsetY = 600;
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float         &_dt;
        std::string    _currentScene;
        std::unordered_map<std::string, ParallaxLevelConfig> _levelConfigs;

        void initLevelConfigs()
        {
            const std::string BASE_PATH = "../../client/assets/sprites/";

            _levelConfigs["LevelOne"] = {
                {
                    ParallaxScroller{BASE_PATH + "parallax-space-backgound.png", 0.0f, 0.0f, 1, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "parallax-space-stars.png", 0.03f, 0.0f, 2, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "parallax-space-stars.png", 0.03f, 0.0f, 2, 800.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "parallax-space-far-planets.png",
                        static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 3,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 2) / 100, true},
                    ParallaxScroller{BASE_PATH + "parallax-space-ring-planet.png",
                        static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 4,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 2) / 100, true},
                    ParallaxScroller{BASE_PATH + "parallax-space-big-planet.png",
                        static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 5,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360), 
                        static_cast<float>(std::rand() % 500 + 2) / 100, true}
                }
            };

            _levelConfigs["LevelTwo"] = {
                {
                    ParallaxScroller{BASE_PATH + "lvl2/nebula_blue.png", 0.0f, 0.0f, 1, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/space_back2.png", 0.03f, 0.0f, 2,  0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/space_back2.png", 0.03f, 0.0f, 2,  800.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/planet_blue.png",
                        static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 3, 
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true},
                    ParallaxScroller{BASE_PATH + "lvl2/animatedPlanet.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 4, 
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/blackHole.png", 0.07f, 0.0f, 5, 
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true}
                }
            };

            _levelConfigs["LevelThree"] = {
                {
                    ParallaxScroller{BASE_PATH + "lvl2/nebula_red.png", 0.0f, 0.0f, 1, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/space_back3.png", 0.03f, 0.0f, 2, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/space_back3.png", 0.03f, 0.0f, 2, 0.0f, 0.0f, 0, 4, true},
                    ParallaxScroller{BASE_PATH + "lvl2/galaxy.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 3, 
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/red_planet3.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 4,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/gaz.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 5,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/blackHole3.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 5,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/blackHole3.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 5,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true},
                    ParallaxScroller{BASE_PATH + "lvl2/bh32_2.png", static_cast<float>(std::rand() % 200 + 3) / 1000.0f, 0.0f, 5,
                        static_cast<float>(std::rand() % 2401 - 800),
                        static_cast<float>(std::rand() % 600),
                        static_cast<float>(std::rand() % 360),
                        static_cast<float>(std::rand() % 500 + 200) / 100, true, true}
                }
            };

            for (const auto &[name, config] : _levelConfigs) {
                std::cout << "[ParallaxSystem] Loaded " << name
                          << " (" << config.layers.size() << " layers)\n";
            }
        }

        void setLevel(const std::string &levelName)
        {
            auto it = _levelConfigs.find(levelName);
            if (it == _levelConfigs.end())
                return;
            cleanupCurrentParallax();
            createParallaxForLevel(it->second);
        }

        void cleanupCurrentParallax()
        {
            auto &parallaxes = _registry.getComponents<ParallaxScroller>();
            std::cout << "[ParallaxSystem] Cleaning up parallax layers: " << _currentScene << std::endl;

            for (auto &&[i, parallax] : indexedZipper(parallaxes)) {
                if (parallax.has_value()) {
                    parallax->isVisible = false;
                    _registry.killEntity(_registry.entityFromIndex(i));
                }
            }
        }

        void createParallaxForLevel(const ParallaxLevelConfig &config)
        {
            for (const auto &layer : config.layers)
            {
                ECS::Entity entity = _registry.spawnEntity();

                if (layer.isAnimated) {
                    Animatable animatable({{0, 49, true, false, 50, 0, 0.1f}});
                    animatable.animationMap["Idle"] = 1;
                    ClockComponent clockComponent({0.0f});
                    _registry.addComponent<Animatable>(entity, animatable);
                    _registry.addComponent<ClockComponent>(entity, clockComponent);
                }

                _registry.addComponent<ParallaxScroller>(entity, layer);
            }
        }
    };
}