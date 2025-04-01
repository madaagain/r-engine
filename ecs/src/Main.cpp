#include <SFML/Graphics.hpp>
#include <iostream>

#include <memory>
#include <thread>
#include <chrono>

#include "Components.hpp"
#include "InputSystem.hpp"
#include "CollisionSystem.hpp"
#include "Renderer.hpp"
#include "RenderingSystem.hpp"
#include "ClockSystem.hpp"
#include "AnimationSystem.hpp"
#include "TransformSystem.hpp"
#include "BasicMissileSystem.hpp"
#include "PipeSystem.hpp"
#include "BirdSystem.hpp"

int main()
{
    ECS::Registry registry;
    std::srand(std::time(nullptr));

    sf::Clock clock;
    float dt = 0.0f;

    registry.registerComponent<Velocity>();
    registry.registerComponent<Controllable>();
    registry.registerComponent<Transformable>();
    registry.registerComponent<Animatable>();
    registry.registerComponent<Renderable>();
    registry.registerComponent<ParallaxScroller>();
    registry.registerComponent<ClockComponent>();
    registry.registerComponent<Player>();
    registry.registerComponent<Audio>();
    registry.registerComponent<Shape>();
    registry.registerComponent<Color>();
    registry.registerComponent<TextRenderable>();


    Systems::InputState inputState{ static_cast<int>(0), false, false, false, false, false, false, false};
    auto inputSystem = std::make_shared<Systems::InputSystem>(
        registry,
        registry.getComponents<Controllable>(),
        registry.getComponents<Velocity>(),
        registry.getComponents<Animatable>(),
        inputState,
        dt
    );

    auto pipeSystem = std::make_shared<Systems::PipeSystem>(
        registry,
        dt  
    );
    auto birdSystem = std::make_shared<Systems::BirdSystem>(
        registry,
        dt  
    );
   registry.addSystem([birdSystem, &dt]() mutable {
        birdSystem->update(dt);
    });
   registry.addSystem([pipeSystem, &dt]() mutable {
        pipeSystem->update(dt);
    });
    
    
    registry.addSystem([inputSystem, &dt]() mutable {
        inputSystem->update(dt);
    });

    std::shared_ptr<IRenderer> renderer = std::make_shared<SFMLRenderer>(800, 600, "Flappy bird");
//     renderer->addParallaxLayer("../../client/assets/sprites/parallax-space-backgound.png",400.0f, 0.0f, 1);
//     renderer->addParallaxLayer("../../client/assets/sprites/parallax-space-big-planet.png", 500.0f, 0.0f, 1);

//     ECS::Entity parallax = registry.spawnEntity();
//     registry.addComponent<ParallaxScroller>(parallax, {
//         "../../client/assets/sprites/r-typesheet3.gif", 0.0f, 0.0f, 2, 0.0f, 0.0f
//     });

//     /*ship*/
//     ECS::Entity ship = registry.spawnEntity();
//     registry.addComponent<Velocity>(ship, {0.0f, 0.0f});
//     registry.addComponent<Controllable>(ship, Controllable());
//     registry.addComponent<Player>(ship, Player(1));
//     registry.addComponent<Renderable>(ship,
//     {"../../client/assets/sprites/r-typesheet42.gif", 166, 17, 0.0f, 0.0f, 1, true, true,  0});

//     Animatable animatable({ {0, 5, 1, 0.1f}, {0, 5, 1, 100.0f}});
//     animatable.animationMap["Idle"] = 1;
//     animatable.animationMap["Up"] = 0;

//     registry.addComponent<Animatable>(ship, animatable);
//     registry.addComponent<Transformable>(ship, {50.0f, 150.0f, 0.0f, 2.2f, 2.2f});
//     registry.addComponent<ClockComponent>(ship, {dt});

//     // @brief collision tester:
//     registry.registerComponent<Collidable>();

//     auto player = registry.spawnEntity();
//     registry.addComponent<Collidable>(player, Collidable(
//         20.0f,
//         CollisionCategory::Player,
//         static_cast<uint8_t>(CollisionCategory::Enemy) | 
//         static_cast<uint8_t>(CollisionCategory::EnemyMissile) |
//         static_cast<uint8_t>(CollisionCategory::Environment)
//     ));

//     auto missile = registry.spawnEntity();
//     registry.addComponent<Collidable>(missile, Collidable(
//         5.0f,
//         CollisionCategory::PlayerMissile,
//         static_cast<uint8_t>(CollisionCategory::Enemy) |
//         static_cast<uint8_t>(CollisionCategory::Environment)
//     ));

//     // Add collision system
//     // auto collisionSystem = std::make_shared<Systems::CollisionSystem>(registry);
//     // registry.addSystem([collisionSystem, &dt]() mutable {
//     //     collisionSystem->update(dt);
//     // });

//     registry.registerComponent<Collidable>();


//     /*ship2*/
//     ECS::Entity ship2 = registry.spawnEntity();
//     registry.addComponent<Velocity>(ship2, {0.0f, 0.0f});
//     registry.addComponent<Controllable>(ship2, Controllable());
//     registry.addComponent<Player>(ship2, Player(2));
//     registry.addComponent<Renderable>(ship2,
//     {"../../client/assets/sprites/r-typesheet42.gif", 166, 17, 0.0f, 0.0f, 1, true, true,  0});

//     Animatable animatable2({ {0, 5, 1, 0.1f}, {0, 5, 1, 100.0f}});
//     animatable2.animationMap["Idle"] = 1;
//     animatable2.animationMap["Up"] = 0;

//     registry.addComponent<Animatable>(ship2, animatable2);
//     registry.addComponent<Transformable>(ship2, {150.0f, 250.0f, 0.0f, 2.2f, 2.2f});
//     registry.addComponent<ClockComponent>(ship2, {dt});   
//     /*ship2*/


    auto transformSystem = std::make_shared<Systems::TransformSystem>(
        registry.getComponents<Transformable>(), registry.getComponents<Velocity>(), registry.getComponents<Player>()
    );

    auto clockSystem = std::make_shared<Systems::ClockSystem>(
        registry.getComponents<ClockComponent>()
    );
    auto animationSystem = std::make_shared<Systems::AnimationSystem>(
        registry.getComponents<Animatable>(),
        registry.getComponents<ClockComponent>(),
        registry.getComponents<Renderable>(),
        registry.getComponents<ParallaxScroller>()
    );

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


//     auto missileSystem = std::make_shared<Systems::BasicMissileSystem>(
//         registry,
//         registry.getComponents<Controllable>(),
//         registry.getComponents<Velocity>(),
//         registry.getComponents<Transformable>(),
//         registry.getComponents<Player>(),
//         inputState, dt
//     );

    registry.addSystem([clockSystem, &dt]() mutable {
        clockSystem->update(dt);
    });

    registry.addSystem([transformSystem, &dt]() mutable {
        transformSystem->update(dt);
    });

    registry.addSystem([animationSystem, &dt]() mutable {
        animationSystem->update(dt);
    });

    registry.addSystem([renderingSystem, &dt]() mutable {
        renderingSystem->update(dt);
    });


//     registry.addSystem([missileSystem, &dt]() mutable {
//         missileSystem->update(dt);
//     });

//     auto& players = registry.getComponents<Player>();
//     for (size_t i = 0; i < registry.getEntityCount(); ++i) {
//         if (players.has(i)) {
//             std::cout << "Player Entity ID: " << i << std::endl;
//         }
//     }

    while (std::static_pointer_cast<SFMLRenderer>(renderer)->isOpen()) {
        std::static_pointer_cast<SFMLRenderer>(renderer)->handleEvents();
        std::static_pointer_cast<SFMLRenderer>(renderer)->handleInputs(inputState);
        dt = clock.restart().asSeconds();
        // renderer->drawParallaxLayers("../../client/assets/sprites/parallax-space-backgound.png", 0.0f, 2.0f, 2.0f, 2.0f, 0, dt);
        registry.runSystems();
        renderer->present();

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}
