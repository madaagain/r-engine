#pragma once

#include "Components.hpp"
#include "ISystems.hpp"
#include "ProtocolSerialization.hpp"
#include "NetworkServer.hpp"
#include "../../client-interface/IClient.hpp"
#include "../../server-interface/IServer.hpp"
#include <memory>

namespace Systems
{
    class ServerNetworkSystem : public ISystem
    {
        public:
            ServerNetworkSystem(ECS::Registry &registry, std::shared_ptr<IServer> server, float &dt)
             : _registry(registry), _server(server), _dt(dt){}
            void update(float dt) override 
            {
                (void) dt;
                sendComponentData<Audio>(DataType::SOU);
                sendComponentData<Transformable>(DataType::TRA);
                sendComponentData<Renderable>(DataType::REN);
                sendComponentData<Player>(DataType::PLA);
                sendComponentData<Controllable>(DataType::CON);
                sendComponentData<Velocity>(DataType::VEL);
                sendComponentData<ClockComponent>(DataType::CLO);
                sendComponentData<Score>(DataType::SCO);
                sendComponentData<TextRenderable>(DataType::TEX);
                sendComponentData<Shape>(DataType::SHA);
                sendComponentData<Color>(DataType::COLO);
                sendComponentData<ParallaxScroller>(DataType::PAR);
            }

        private:
            ECS::Registry &_registry;
            std::shared_ptr<IServer> _server;
            float &_dt;

            template<typename Component>
            void sendComponentData(DataType type)
            {
                auto &components = _registry.getComponents<Component>();
                size_t entityCount = _registry.getEntityCount();
                for (size_t i = 0; i < entityCount; ++i) {
                    if (!components[i].has_value())
                        continue;
                    Component comp = components[i].value();
                    auto data = Protocol::serializeGameState(comp, type);
                    uint8_t entityId = static_cast<uint8_t>(i);
                    data.insert(data.begin(), entityId);
                    _server->broadcast(data);
                }
            }
    };

    class ClientNetworkSystem : public ISystem
    {
        public:
            ClientNetworkSystem(ECS::Registry &registry, std::shared_ptr<IClient> client, float &dt)
                : _registry(registry), _client(client), _dt(dt), lastSendTime(std::chrono::high_resolution_clock::now()) {}
        
            void update(float dt) override
            {
                (void) dt;
                sendControllable(_registry);
                _client->update(_registry);
            }

        private:
            void sendControllable(ECS::Registry &registry)
            {
                auto currentTime = std::chrono::high_resolution_clock::now();
                std::chrono::duration<float> elapsed = currentTime - lastSendTime;
                if (elapsed.count() >= 0.00f) { // -> you guys can change this value to a lower val if you want to send inputs faster
                    auto &controllables = registry.getComponents<Controllable>();
                    auto &players = registry.getComponents<Player>();
                    for (auto &&[i, cont, pla] : indexedZipper(controllables, players)) {
                        if (cont.has_value() && pla.has_value()) {
                            if (pla->playerId == _client->getPlayerEntityId()) {
                                Controllable contt = cont.value();
                                auto data = Protocol::serializeGameState(contt, DataType::CON);
                                uint8_t entityIdByte = static_cast<uint8_t>(i);
                                data.insert(data.begin(), entityIdByte);
                                _client->sendDataToServer(data);
                            }
                        }
                    }
                    lastSendTime = currentTime;
                }
            }

            ECS::Registry &_registry;
            std::shared_ptr<IClient> _client;
            float &_dt;
            std::chrono::high_resolution_clock::time_point lastSendTime;
    };

}