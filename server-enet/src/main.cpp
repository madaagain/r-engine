

#include "../../gamefield/include/RTypeScenes.hpp"
#include "Components.hpp"
#include "NetworkServer.hpp"
#include "../../ecs/scene-interface/SceneManager.hpp"
#include "../../ecs/systems/headers/SceneManagerSystem.hpp"
#include <memory>
#include <iostream>
#include <chrono>

int main()
{
    try {
        std::srand(std::time(nullptr));
        auto previousTime = std::chrono::high_resolution_clock::now();
        auto lastUpdateTime = std::chrono::high_resolution_clock::now();
        float dtFloat = 0.0f;

        ECS::Registry registry;
        registry.registerComponent<Scene>();
        auto sceneEntity = registry.spawnEntity();
        registry.addComponent<Scene>(sceneEntity, {"LevelOne", false, false});
        auto server = std::make_shared<NetworkServer>(8080, 1024);
        SceneManager sceneManager(registry, dtFloat, server);
        sceneManager.registerScene("LevelOne",
            [](ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) {
                return std::make_unique<LevelOne>(registry, dt, server);
            }
        );
        sceneManager.registerScene("SceneTransition",
            [](ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) {
                return std::make_unique<LevelTwoTransition>(registry, dt, server);
            }
        );
        sceneManager.registerScene("LevelTwo",
            [](ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) {
                return std::make_unique<LevelTwo>(registry, dt, server);
            }
        );

        sceneManager.registerScene("LevelThree",
            [](ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) {
                return std::make_unique<LevelThree>(registry, dt, server);
            }
        );

        sceneManager.registerScene("CreditScene",
            [](ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) {
                return std::make_unique<CreditScene>(registry, dt, server);
            }
        );

        auto sceneManagerSystem = std::make_shared<Systems::SceneManagerSystem>(registry, dtFloat, sceneManager);
        registry.addSystem([sceneManagerSystem, &dtFloat]() mutable {
            sceneManagerSystem->update(dtFloat);
        });
        const float updateInterval = 0.00f; // -> you guys can change this to a lower val if you want less lag
        bool clientConnected = false;
        while (true) {
            auto currTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> frameTime = currTime - previousTime;
            previousTime = currTime;
            auto elapsed = std::chrono::duration<float>(currTime - lastUpdateTime).count();
            if (elapsed >= updateInterval) {
                if (server->getClientCount() != 0) {
                    clientConnected = true;
                }
                if (clientConnected) {
                    dtFloat = elapsed;
                }
                server->update(registry);
                registry.runSystems();
                lastUpdateTime = currTime;
            }
        }

    } catch (const std::exception &e) {
        std::cerr << "Server error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
