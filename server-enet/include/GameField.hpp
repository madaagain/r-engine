#pragma once

#include "NetworkServer.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include "../../ecs/include/IndexedZipper.hpp"
#include "../../ecs/systems/headers/TransformSystem.hpp"
#include "../../ecs/systems/headers/NetworkSystem.hpp"
#include "../../ecs/systems/headers/AnimationSystem.hpp"
#include "../../ecs/systems/headers/ClockSystem.hpp"
#include "../../ecs/systems/headers/VelocitySystem.hpp"
#include "../../ecs/systems/headers/BydosAI.hpp"
#include "../../ecs/systems/headers/BasicMissileSystem.hpp"
#include "../../ecs/systems/headers/EnemyMissile.hpp"
#include "../../ecs/systems/headers/CollisionSystem.hpp"
#include "../../ecs/systems/headers/DamageSystem.hpp"
#include "../../ecs/systems/headers/GorgiaAI.hpp"
#include "../../ecs/systems/headers/CovidBoss.hpp"
#include "../../server-interface/IServer.hpp"
#include "../../ecs/systems/headers/ScoreSystem.hpp"
#include "../../ecs/systems/headers/ShieldSystem.hpp"
#include <memory>

class GameField 
{
    public:
        GameField(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) 
            : _registry(registry), _dt(dt), _server(server) {}

         void initRegistry() 
        {
            _registry.registerComponent<Transformable>();
            _registry.registerComponent<Animatable>();
            _registry.registerComponent<Velocity>();
            _registry.registerComponent<Controllable>();
            _registry.registerComponent<Renderable>();
            _registry.registerComponent<ParallaxScroller>();
            _registry.registerComponent<ClockComponent>();
            _registry.registerComponent<Player>();
            _registry.registerComponent<AIState>();
            _registry.registerComponent<Health>();
            _registry.registerComponent<BydosAIComponent>();
            _registry.registerComponent<Missile>();
            _registry.registerComponent<ProtectiveOrbState>();
            _registry.registerComponent<ShieldedBossComponent>();
            _registry.registerComponent<Collidable>();
            _registry.registerComponent<BossAIComponent>();
            _registry.registerComponent<BossAIState>();
            _registry.registerComponent<Score>();
            _registry.registerComponent<TextRenderable>();
            _registry.registerComponent<Shield>();
            _registry.registerComponent<PlayerShield>();
            initSystems();
        }

        void initSystems() 
        {
            auto covboss = std::make_shared<Systems::CovShieldedBossSystem>(_registry, _dt);
            _registry.addSystem([covboss, this]() mutable { covboss->update(_dt); });

            auto gorgiaAI = std::make_shared<Systems::GorgiaAI>(_registry, _dt);
            _registry.addSystem([gorgiaAI, this]() mutable { gorgiaAI->update(_dt); });

            auto clockSystem = std::make_shared<Systems::ClockSystem>(_registry.getComponents<ClockComponent>());
            _registry.addSystem([clockSystem, this]() mutable { clockSystem->update(_dt); });

            auto networkSystem = std::make_shared<Systems::ServerNetworkSystem>(_registry, _server, _dt);
            _registry.addSystem([networkSystem, this]() mutable { networkSystem->update(_dt); });

            auto velocitySystem = std::make_shared<Systems::VelocitySystem>(
                _registry.getComponents<Velocity>(), _registry.getComponents<Controllable>(), _dt
            );
            _registry.addSystem([velocitySystem, this]() mutable { velocitySystem->update(_dt); });

            auto transformSystem = std::make_shared<Systems::TransformSystem>(
                _registry.getComponents<Transformable>(), _registry.getComponents<Velocity>(), _registry.getComponents<Player>()
            );
            _registry.addSystem([transformSystem, this]() mutable { transformSystem->update(_dt); });

            auto collisionSystem = std::make_shared<Systems::CollisionSystem>(_registry, _dt);
            _registry.addSystem([collisionSystem, this]() mutable { collisionSystem->update(_dt); });

            auto damageSystem = std::make_shared<Systems::DamageSystem>(_registry, _dt);
            _registry.addSystem([damageSystem, this]() mutable { damageSystem->update(_dt); });

            auto missileSystem = std::make_shared<Systems::BasicMissileSystem>(
                _registry,
                _registry.getComponents<Controllable>(),
                _registry.getComponents<Velocity>(),
                _registry.getComponents<Transformable>(),
                _registry.getComponents<Player>(),
                _dt
            );
            _registry.addSystem([missileSystem, this]() mutable { missileSystem->update(_dt); });

            auto enemyMissiles = std::make_shared<Systems::EnemyMissile>(_registry, _dt);
            _registry.addSystem([enemyMissiles, this]() mutable { enemyMissiles->update(_dt); });

            auto bydosAISystem = std::make_shared<Systems::BydosAI>(_registry, _dt);
            _registry.addSystem([bydosAISystem, this]() mutable { bydosAISystem->update(_dt); });

            auto animationSystem = std::make_shared<Systems::AnimationSystem>(
                _registry.getComponents<Animatable>(),
                _registry.getComponents<ClockComponent>(),
                _registry.getComponents<Renderable>(),
                _registry.getComponents<ParallaxScroller>()
            );
            _registry.addSystem([animationSystem, this]() mutable { animationSystem->update(_dt); });  

            ECS::Entity score = _registry.spawnEntity();
            _registry.addComponent<Score>(score, Score{0, 0, false});
            _registry.addComponent<TextRenderable>(score, TextRenderable{"score", "Score 0", "../../client/assets/fonts/r-type.ttf", 24, 0, 0, 1, true, 255.0, 255.0, 255.0, 255.0});

            auto scoreSystem = std::make_shared<Systems::ScoreSystem>(_registry);
            _registry.addSystem([scoreSystem, this]() mutable { scoreSystem->update(_dt); });

            auto shieldSystem = std::make_shared<Systems::ShieldSystem>(_registry, _dt);
            _registry.addSystem([shieldSystem, this]() mutable { shieldSystem->update(_dt); });
        }

        void killEntities()
        {
            auto &renderables = _registry.getComponents<Renderable>();
            auto &players = _registry.getComponents<Player>();


            for (auto &&[i, ren, pla] : indexedZipper(renderables, players)) {
                if (ren.has_value()) {
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;

};