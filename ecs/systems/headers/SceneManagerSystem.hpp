#pragma once

#include "ISystems.hpp"
#include "../include/Registry.hpp"
#include "../../server-interface/IServer.hpp"
#include "../scene-interface/SceneManager.hpp"
#include <string>
#include <iostream>

namespace Systems
{
    class SceneManagerSystem : public ISystem
    {
    public:
        SceneManagerSystem(ECS::Registry &registry, float &dt, SceneManager &sceneManager)
            : _registry(registry),
              _dt(dt),
              _sceneManager(sceneManager),
              _lastSceneName(""),
              _wasPaused(false)
        {
        }

        void update(float dt) override
        {
            (void)dt;
            auto &scenes = _registry.getComponents<Scene>();
            if (scenes.size() == 0 || !scenes[0].has_value())
                return;
            Scene &sceneComp = scenes[0].value();
            if (sceneComp.currentScene != "SceneTransition")
                sceneComp.isResetting = false;
            if (sceneComp.currentScene != _lastSceneName) {
                sceneComp.isResetting = true;
                if( sceneComp.isResetting) {
                    std::cout << "IS RESETTING\n\n\n\n" << sceneComp.isResetting << std::endl;
                }
                handleSceneChange(_lastSceneName, sceneComp.currentScene);
                _lastSceneName = sceneComp.currentScene;
            }
            if (sceneComp.isPaused != _wasPaused) {
                handlePauseToggle(_wasPaused, sceneComp.isPaused);
                _wasPaused = sceneComp.isPaused;
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        SceneManager &_sceneManager;
        std::string _lastSceneName;
        bool _wasPaused;

        void handleSceneChange(const std::string &oldScene, const std::string &newScene)
        {
            std::cout << "[SceneManagerSystem] Scene changed from '" 
                      << oldScene << "' to '" << newScene << "'\n";

            if (!oldScene.empty()) {
                // _sceneManager.popScene();
            }
            if (!newScene.empty()) {
                _sceneManager.pushScene(newScene);
            }
        }

        void handlePauseToggle(bool wasPaused, bool isPaused)
        {
            std::cout << "[SceneManagerSystem] Pause changed from "
                      << wasPaused << " to " << isPaused << "\n"; //this is probably a shit way to handle pause calling a private method but IDK
        }
    };
}

