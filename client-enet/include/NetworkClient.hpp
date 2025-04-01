

#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <cstring>

#include "enet/enet.h"
#include "../../ecs/include/ProtocolSerialization.hpp"
#include "../../client-interface/IClient.hpp"
#include "../../shared/include/IGameState.hpp"

class NetworkClient : public IClient
{
    public:
        NetworkClient();
        ~NetworkClient();
        NetworkClient(const NetworkClient&) = delete;
        NetworkClient& operator=(const NetworkClient&) = delete;

        bool connect(const char* address, uint16_t port, ECS::Registry &registry) override;
        void disconnect() override;
        void registerHandler(DataType messageType, MessageHandler handler) override;
        void update(ECS::Registry &registry) override;
        void sendDataToServer(const std::vector<uint8_t> &data) override;
        uint32_t getClientId() override;
        size_t getPlayerEntityId() override;

    private:
        uint32_t clientId = 0;
        size_t PlayerEntityId = 0;
        void networkLoop();
        void handleServerMessage(ENetPacket* packet, ECS::Registry &registry);
        ENetHost* client;
        ENetPeer* serverPeer;
        std::unordered_map<DataType, MessageHandler> MessageHandlers;
        std::thread networkThread;
        std::queue<ENetEvent> messageQueue;
        std::mutex queueMutex;
        bool isConnected;
        bool isRunning;
};
