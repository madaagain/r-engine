#pragma once

#include "../../ecs/scene-interface/IScene.hpp"
#include "../../ecs/include/Registry.hpp"
#include "../components/Components.hpp"
#include "../systems/headers/TransformSystem.hpp"
#include "../systems/headers/NetworkSystem.hpp"
#include "../systems/headers/AnimationSystem.hpp"
#include "../systems/headers/ClockSystem.hpp"
#include "../systems/headers/VelocitySystem.hpp"
#include "../systems/headers/BydosAI.hpp"
#include "../systems/headers/BasicMissileSystem.hpp"
#include "../systems/headers/EnemyMissile.hpp"
#include "../systems/headers/CollisionSystem.hpp"
#include "../systems/headers/DamageSystem.hpp"
#include "../systems/headers/GorgiaAI.hpp"
#include "../systems/headers/CovidBoss.hpp"
#include "../systems/headers/RobotAI.hpp"
#include "../systems/headers/FadeTransitionSystem.hpp"
#include "../systems/headers/AudioSystem.hpp"
#include "../systems/headers/ShieldSystem.hpp"
#include "../systems/headers/ShrimpBoss.hpp"
#include "../systems/headers/ScoreSystem.hpp"
#include "../systems/headers/ExplosionSystem.hpp"
#include "../../server-interface/IServer.hpp"
#include "../systems/headers/HealingSystem.hpp"
#include "../systems/headers/ParallaxSystem.hpp"
#include "../systems/headers/DmgIndicatorSystem.hpp"
#include "../systems/headers/ForceSystem.hpp"
#include "../systems/headers/CreditSceneSystem.hpp"
#include "../systems/headers/LifeCounterSystem.hpp"
#include "../systems/headers/MissileCleanupSystem.hpp"
#include "Entity.hpp"
#include <memory>
#include <cassert>

class LevelOne : public IScene 
{
    public:
        LevelOne(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) : _registry(registry), _dt(dt), _server(server) {}
        
        void initRegistry() override 
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
            _registry.registerComponent<Collidable>();
            _registry.registerComponent<MultiCollidable>();
            _registry.registerComponent<BossAIComponent>();
            _registry.registerComponent<BossAIState>();
            _registry.registerComponent<ProtectiveOrbState>();
            _registry.registerComponent<ShieldedBossComponent>();
            _registry.registerComponent<Shape>();
            _registry.registerComponent<Color>();
            _registry.registerComponent<Audio>();
            _registry.registerComponent<TextRenderable>();
            _registry.registerComponent<Score>();
            _registry.registerComponent<PlayerShield>();
            _registry.registerComponent<Explosion>();
            _registry.registerComponent<Force>();
            _registry.registerComponent<RobotAIComponent>();
            initSystems();
        }

        void initSystems() override
        {
            auto shapeEntity = _registry.spawnEntity();
            _registry.addComponent<Color>(shapeEntity, {0.0f, 0.0f, 0.0f, 0.0f});
            _registry.addComponent<Shape>(shapeEntity, {"Rectangle", 900.0f, 900.0f, 0.0f, true});
            _registry.addComponent<Transformable>(shapeEntity, {200.0f, 200.0f, 0.0f, 1.0f, 1.0f});

            auto covboss = std::make_shared<Systems::CovShieldedBossSystem>(_registry, _dt);
            _registry.addSystem([covboss, this]() mutable { covboss->update(_dt); });

            auto shrimpBoss = std::make_shared<Systems::ShrimpBoss>(_registry, _dt);
            _registry.addSystem([shrimpBoss, this]() mutable { shrimpBoss->update(_dt); });

            auto gorgiaAI = std::make_shared<Systems::GorgiaAI>(_registry, _dt);
            _registry.addSystem([gorgiaAI, this]() mutable { gorgiaAI->update(_dt); });

            auto clockSystem = std::make_shared<Systems::ClockSystem>(_registry.getComponents<ClockComponent>());
            _registry.addSystem([clockSystem, this]() mutable { clockSystem->update(_dt); });

            auto velocitySystem = std::make_shared<Systems::VelocitySystem>(
                _registry.getComponents<Velocity>(), _registry.getComponents<Controllable>(), _dt
            );
            _registry.addSystem([velocitySystem, this]() mutable { velocitySystem->update(_dt); });

            auto collisionSystem = std::make_shared<Systems::CollisionSystem>(_registry, _dt);
            _registry.addSystem([collisionSystem, this]() mutable { collisionSystem->update(_dt); });

            auto healingSystem = std::make_shared<Systems::HealingSystem>(_registry, _dt);
            _registry.addSystem([healingSystem, this]() mutable { healingSystem->update(_dt); });

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

            auto shieldSystem = std::make_shared<Systems::ShieldSystem>(_registry, _dt);
            _registry.addSystem([shieldSystem, this] () mutable { shieldSystem->update(_dt); });

            auto enemyMissiles = std::make_shared<Systems::EnemyMissile>(_registry, _dt);
            _registry.addSystem([enemyMissiles, this]() mutable { enemyMissiles->update(_dt); });

            auto animationSystem = std::make_shared<Systems::AnimationSystem>(
                _registry.getComponents<Animatable>(),
                _registry.getComponents<ClockComponent>(),
                _registry.getComponents<Renderable>(),
                _registry.getComponents<ParallaxScroller>()
            );
            _registry.addSystem([animationSystem, this]() mutable { animationSystem->update(_dt); }); 

            auto bydosAISystem = std::make_shared<Systems::BydosAI>(_registry, _dt);
            _registry.addSystem([bydosAISystem, this]() mutable { bydosAISystem->update(_dt); });

            auto forceSystem = std::make_shared<Systems::ForceSystem>(_registry, _dt);
            _registry.addSystem([forceSystem, this]() mutable { forceSystem->update(_dt); });

            auto transformSystem = std::make_shared<Systems::TransformSystem>(
                _registry.getComponents<Transformable>(), _registry.getComponents<Velocity>(), _registry.getComponents<Player>()
            );
            _registry.addSystem([transformSystem, this]() mutable { transformSystem->update(_dt); });

            auto dmgIndicatorSystem = std::make_shared<Systems::DmgIndicatorSystem>(_registry, _dt);
            _registry.addSystem([dmgIndicatorSystem, this]() mutable { dmgIndicatorSystem->update(_dt); });

            auto robotAISystem = std::make_shared<Systems::RobotAI>(_registry, _dt);
            _registry.addSystem([robotAISystem, this]() mutable { robotAISystem->update(_dt); });

            auto creditSceneSystem = std::make_shared<Systems::CreditSceneSystem>(_registry, _dt);
            _registry.addSystem([creditSceneSystem, this]() mutable { creditSceneSystem->update(_dt); });

            auto missileCleanupSystem = std::make_shared<Systems::MissileCleanupSystem>(_registry, _dt);
            _registry.addSystem([missileCleanupSystem, this]() mutable { missileCleanupSystem->update(_dt); });

            auto explosionSystem = std::make_shared<Systems::ExplosionSystem>(_registry, _dt);
            _registry.addSystem([explosionSystem, this]() mutable { explosionSystem->update(_dt); });

            auto parallaxSystem = std::make_shared<Systems::ParallaxSystem>(_registry, _dt);
            assert(parallaxSystem && "Failed to create ParallaxSystem!");
            _registry.addSystem([parallaxSystem, this]() mutable { parallaxSystem->update(_dt); });

            auto fadeSystem = std::make_shared<Systems::FadeTransitionSystem>(_registry, _dt);
            _registry.addSystem([fadeSystem, this]() mutable { fadeSystem->update(_dt); });

            ECS::Entity score = _registry.spawnEntity();
            _registry.addComponent<Score>(score, Score{0, 0, false});
            _registry.addComponent<TextRenderable>(score, TextRenderable{"score", "Score 0", "../../client/assets/fonts/r-type.ttf", 15, 5, 580, 1, true, 255.0, 255.0, 255.0, 255.0});

            auto lifeSystem = std::make_shared<Systems::LifeCounterSystem>(_registry);
            _registry.addSystem([lifeSystem, this]() mutable { lifeSystem->update(_dt); });

            auto scoreSystem = std::make_shared<Systems::ScoreSystem>(_registry);
            _registry.addSystem([scoreSystem, this]() mutable { scoreSystem->update(_dt); });

            auto networkSystem = std::make_shared<Systems::ServerNetworkSystem>(_registry, _server, _dt);
            _registry.addSystem([networkSystem, this]() mutable { networkSystem->update(_dt); }); 
        }

        void killEntities() override 
        {

        }
  
    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;
};

class LevelTwoTransition : public IScene
{
    public:
    LevelTwoTransition(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) : _registry(registry), _dt(dt), _server(server) {}

    void initRegistry() override 
    {
        initSystems();
    }

    void initSystems() override
    {
    }

    void killEntities() override 
    {
    }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server; 
};


class LevelTwo : public IScene 
{
    public:
        LevelTwo(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) 
            : _registry(registry), _dt(dt), _server(server) {}

        void initRegistry() override 
        {
            initSystems();
        }

        void initSystems() override
        {


        }

        void killEntities() override 
        {
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;
};

class LevelThree : public IScene 
{
    public:
        LevelThree(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) : _registry(registry), _dt(dt), _server(server) {}

        void initRegistry() override 
        {
            initSystems();
        }

        void initSystems() override
        {
        }

        void killEntities() override 
        {

        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;
};

class CreditScene : public IScene 
{
    public:
        CreditScene(ECS::Registry &registry, float &dt, std::shared_ptr<IServer> server) : _registry(registry), _dt(dt), _server(server) {}

        void initRegistry() override 
        {
            initSystems();
        }

        void initSystems() override
        {
        }

        void killEntities() override 
        {

        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::shared_ptr<IServer> _server;
};