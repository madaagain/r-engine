

#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <thread>
#include <ctime>
#include <queue>
#include <mutex>
#include <chrono>
#include <cstring>

#include "enet/enet.h"
#include "SafeQueue.hpp"
#include "../../server-interface/IServer.hpp"
#include "../../shared/include/IGameState.hpp"
#include "../../ecs/include/ProtocolSerialization.hpp"

struct GameState;
struct PlayerInput;

class NetworkServer : public IServer
{
  public:
    using MessageHandler = std::function<void(uint32_t clientId, const std::vector<uint8_t>& data)>;

    NetworkServer(uint16_t port = 1234, size_t maxConnections = 1024);
    ~NetworkServer();
    NetworkServer(const NetworkServer&) = delete;
    NetworkServer& operator=(const NetworkServer&) = delete;

    void registerHandler(uint8_t messageType, MessageHandler handler) override;
    void update(ECS::Registry &registry) override;
    void broadcast(const std::vector<uint8_t> &data) override;

    size_t getClientCount() const;

  private:
    void initPlayer(ECS::Registry &registry, uint32_t clientId, uint32_t &playerEntityId);
    void HandleClientConnection(ENetPeer* peer, ECS::Registry &registry);
    void HandleClientDisconnection(ENetPeer* peer, ECS::Registry &registry);
    void handleClientMessage(ENetPeer* peer, ENetPacket* packet, ECS::Registry &registry);
    ENetPeer* FindPeersPerIds(uint32_t Cli_Id);

    void networkLoop();

    ENetHost *server;
    std::unordered_map<ENetPeer*, uint32_t> clientIds;
    std::unordered_map<uint32_t, size_t> _clientToEntity;
    std::unordered_map<uint8_t, MessageHandler> messageHandlers;
    uint32_t nextClientId;
    ENetEvent event;
    std::thread networkThread;
    SafeQueue<ENetEvent> messageQueue;
    bool isRunning;

};
