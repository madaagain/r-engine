

#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <csignal>

#include "ServerInterface.hpp"
#include "Ecs.hpp"
class Server : public net::serverInterface<MsgTypes>
{
    public:
        Server(int nPort, ECS::Registry registry);
        ~Server();

        void sendMessageToClient(uint32_t clientID, const net::message<MsgTypes>& msg);

        bool start();

    protected:
        ECS::Registry _registry;

        virtual void onMessage(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg) override;

        void handleClientConnect(std::shared_ptr<net::Connection<MsgTypes>> client);
        void handleClientDisconnect(std::shared_ptr<net::Connection<MsgTypes>> client);
        void handleServerPing(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg);
        void handleAKeyPressed(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg);
        void handleMove(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg);

        void broadcastGameState();
        void startBroadcasting();


    private:

        GameState gameState;
        uint32_t clientIDCounter = 1000;
        std::thread broadcasterThread;
        std::atomic<bool> running { true };
        std::mutex clientMutex;
};

#endif // SERVER_HPP_
