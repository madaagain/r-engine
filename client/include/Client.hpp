

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <functional>
#include <atomic>
#include <cstdint>
#include <iostream>
#include <cstring>
#include <chrono>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ClientInterface.hpp"

class Client : public net::clientInterface<MsgTypes>
{
    public:
        void pingServer();
        void sendConnectMessage();
        void sendDisconnectMessage();
        void handleClientDisconnect(net::message<MsgTypes>& msg);

        void setConnected(bool connected) { is_connected = connected; }
        bool getConnected() const { return is_connected; }
        void setID(uint32_t id) { client_id = id; }
        uint32_t getID() const { return client_id; }

        void handleIncomingMessages();
        void handleWelcomeMessage(net::message<MsgTypes>& msg);
        void handleServerPing(net::message<MsgTypes>& msg);
        void handleAKeyPressed(net::message<MsgTypes>& msg);
        void handleGameState(net::message<MsgTypes>& msg);

        void sendAKeyPressed();
        void sendMove(Direction direction);

        using GameStateCallback = std::function<void(const GameState&)>;
        void setGameStateCallback(GameStateCallback callback) { onGameState = callback; }

    private:
        std::atomic<bool> is_connected = false;
        uint32_t client_id = 0;
        GameStateCallback onGameState;
};

#endif // CLIENT_HPP
