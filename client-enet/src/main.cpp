

#include <iostream>
#include <cstdlib>
#include <memory>
#include "../include/NetworkClient.hpp"
#include "../include/ClientGameState.hpp"
#include "../include/Screens/Menu.hpp"
#include "../include/Utils/Utils.hpp"
#include "../../client/include/InitGame.hpp"
#include "../../client-interface/IClient.hpp"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Renderer.hpp"
#include "InputSystem.hpp"
#include "ScoreSystem.hpp"
#include "RenderingSystem.hpp"
#include "AnimationSystem.hpp"
#include "TransformSystem.hpp"
#include "Components.hpp"
#include "Registry.hpp"
#include "SparseArray.hpp"
#include "ISystems.hpp"
#include "NetworkSystem.hpp"
#include "ParallaxSystem.hpp"

int main()
{
    std::shared_ptr<SFMLRenderer> renderer = std::make_shared<SFMLRenderer>(800, 600, "Rtype");
    try {
        sf::RenderWindow window(sf::VideoMode(1920, 1080), "R-type");
        OptionsState options(window, renderer);
        MenuState menu(window, options);
        menu.run();

    } catch (std::exception &e) {
        std::cerr << "Menu error occurred: " << e.what() << std::endl;
    }

    try {
        std::shared_ptr<IClient> client = std::make_shared<NetworkClient>();
        ECS::Registry registry;
        sf::Clock clock;
        float dt = 0.0f;

        registry.registerComponent<Controllable>();
        registry.registerComponent<Transformable>();
        registry.registerComponent<Animatable>();
        registry.registerComponent<Renderable>();
        registry.registerComponent<ParallaxScroller>();
        registry.registerComponent<Velocity>();
        registry.registerComponent<Player>();
        registry.registerComponent<Shape>();
        registry.registerComponent<Color>();
        registry.registerComponent<Audio>();
        registry.registerComponent<TextRenderable>();
        registry.registerComponent<Scene>();

        auto renderingSystem = std::make_shared<Systems::RenderingSystem>(
            registry.getComponents<Animatable>(),
            registry.getComponents<Transformable>(),
            registry.getComponents<Renderable>(),
            registry.getComponents<TextRenderable>(),
            registry.getComponents<ParallaxScroller>(),
            registry.getComponents<Shape>(),
            registry.getComponents<Color>(),
            registry.getComponents<Audio>(),
            renderer
        );

        if (!client->connect("127.0.0.1", 8080, registry)) {
            std::cerr << "Failed to connect" << std::endl;
            return 1;
        }

        auto clientNetworkSystem = std::make_shared<Systems::ClientNetworkSystem>(registry, client, dt);

        Systems::InputState inputState{ static_cast<int>(client->getPlayerEntityId()), false, false, false, false, false, false, false};
        auto inputSystem = std::make_shared<Systems::InputSystem>(
            registry,
            registry.getComponents<Controllable>(),
            registry.getComponents<Velocity>(),
            registry.getComponents<Animatable>(),
            inputState,
            dt
        );

        auto transformSystem = std::make_shared<Systems::TransformSystem>(
            registry.getComponents<Transformable>(),
            registry.getComponents<Velocity>(),
            registry.getComponents<Player>()
        );


        registry.addSystem([transformSystem, &dt]() mutable {
            transformSystem->update(dt);
        });

        registry.addSystem([renderingSystem, &dt]() mutable {
            renderingSystem->update(dt);
        });

        registry.addSystem([clientNetworkSystem, &dt]() mutable {
            clientNetworkSystem->update(dt);
        });

        registry.addSystem([inputSystem, &dt]() mutable {
            inputSystem->update(dt);
        });

        while (std::static_pointer_cast<SFMLRenderer>(renderer)->isOpen()) {
            std::static_pointer_cast<SFMLRenderer>(renderer)->handleEvents();
            std::static_pointer_cast<SFMLRenderer>(renderer)->handleInputs(inputState);
            dt = clock.restart().asSeconds();
            renderer->present();
            registry.runSystems();
            renderer->cleanupSounds();
        }

    } catch (const std::exception &e) {
        std::cerr << "Client error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
