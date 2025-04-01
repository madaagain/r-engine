#pragma once

#include <stdexcept>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>
#include "IScene.hpp"
#include "../include/Registry.hpp"
#include "../../server-interface/IServer.hpp"

class SceneManager 
{
    public:
        using SceneFactory = std::function<std::unique_ptr<IScene>(ECS::Registry&, float&, std::shared_ptr<IServer>)>;

        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;

        SceneManager(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server)
            : _registry(registry), _dt(dt), _server(std::move(server))
        {
        }

        void registerScene(const std::string &sceneName, SceneFactory factory) 
        {
            _sceneFactories[sceneName] = std::move(factory);
        }

        void pushScene(const std::string &sceneName)
        {
            auto it = _sceneFactories.find(sceneName);
            if (it == _sceneFactories.end()) {
                throw std::runtime_error("No scene factory registered for: " + sceneName);
            }
            std::unique_ptr<IScene> newScene = it->second(_registry, _dt, _server);
            newScene->initRegistry();
            _scenesStack.push_back(std::move(newScene));
            _activeSceneName = sceneName;
            std::cout << "[SceneManager] Pushed scene: " << sceneName << std::endl;
        }

        void popScene()
        {
            if (!_scenesStack.empty()) {
                //we might need to kill entities here !!!
                _scenesStack.pop_back();
            }

            if (!_scenesStack.empty()) {
                /*
                    if there is still any scenes we set the top scene name (it's a demonstration here it doesnt work yet obviously...)
                */
                _activeSceneName = "[Top Scene Name]";
            } else {
                _activeSceneName.clear();
            }
        }

        void updateCurrentScene()
        {
            // just in case we want to for example add a boss or other event to the scene but it might actually be already handled
        }

        bool isEmpty() const { return _scenesStack.empty(); }

        const std::string& getActiveSceneName() const { return _activeSceneName; }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;

        std::vector<std::unique_ptr<IScene>> _scenesStack;
        std::unordered_map<std::string, SceneFactory> _sceneFactories;
        std::string _activeSceneName;
};
