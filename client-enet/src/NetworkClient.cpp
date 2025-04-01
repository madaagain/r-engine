

#include "../include/NetworkClient.hpp"
#include <stdexcept>
#include <iostream>
#include <utility>

/**
 * @brief Constructeur de la classe NetworkClient.
 * Initialise le client ENet et démarre le thread réseau.
 * @throws std::runtime_error si l'initialisation d'ENet ou la création du client échoue.
 */
NetworkClient::NetworkClient() : serverPeer(nullptr), isConnected(false), isRunning(true)
{
    if (enet_initialize() != 0) {
        throw std::runtime_error("Failed to initialise ENet client");
    }

    client = enet_host_create(nullptr, 1, 2, 0, 0);
    if (!client) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet client");
    }

    networkThread = std::thread(&NetworkClient::networkLoop, this);
}

/**
 * @brief Destructeur de la classe NetworkClient.
 * Arrête le thread réseau, déconnecte si nécessaire et libère les ressources ENet.
 */

NetworkClient::~NetworkClient()
{
    isRunning = false;

    if (networkThread.joinable()) {
        networkThread.join();
    }

    if (isConnected) {
        disconnect();
    }

    if (client) {
        enet_host_destroy(client);
    }
    enet_deinitialize();
}

/**
 * @brief Connecte le client à un serveur ENet.
 * @param address Adresse IP ou hostname du serveur.
 * @param port Port du serveur.
 * @return true si la connexion est réussie, false sinon.
 * @details Tente de se connecter au serveur et attend une réponse pendant 5 secondes.
 */

bool NetworkClient::connect(const char *address, uint16_t port, ECS::Registry &registry)
{
    ENetAddress serverAddress;
    enet_address_set_host(&serverAddress, address);
    serverAddress.port = port;
    std::cout << "Attempting to connect..." << std::endl;
    serverPeer = enet_host_connect(client, &serverAddress, 2, 0);
    if (!serverPeer) {
        std::cerr << "Failed to create ENet peer for connection." << std::endl;
        return false;
    }
    ENetEvent event;
    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection established with server." << std::endl;
        if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_RECEIVE) {
            handleServerMessage(event.packet,registry);
            enet_packet_destroy(event.packet);
            std::cout << "Client ID received: " << clientId << std::endl;
            isConnected = true;
            return true;
        } else {
            std::cerr << "Failed to receive client ID message from server." << std::endl;
            enet_peer_reset(serverPeer);
            return false;
        }
    }

    std::cerr << "Failed to connect to server." << std::endl;
    enet_peer_reset(serverPeer);
    return false;
}


void NetworkClient::disconnect()
{
    if (isConnected && serverPeer) {
        std::cout << "Client disconnecting" << std::endl;
        enet_peer_disconnect(serverPeer, 0);

        ENetEvent event;

        while (enet_host_service(client, &event, 3000) > 0) {
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                isConnected = false;
                serverPeer = nullptr;
                return;
            }
        }

        // [ATTENTION]: Forced deconnexion if no answer, if the server do not respond you can diagnose this method
        enet_peer_reset(serverPeer);
        isConnected = false;
        serverPeer = nullptr;
    }
}

uint32_t NetworkClient::getClientId()
{
    return this->clientId;
}

void NetworkClient::networkLoop()
{
    while(isRunning) {
        ENetEvent event;
        while(enet_host_service(client, &event, 0) > 0) {
            std::lock_guard<std::mutex> lock(queueMutex);
            messageQueue.push(event);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void NetworkClient::sendDataToServer(const std::vector<uint8_t> &data)
{
    if (!isConnected || !serverPeer){
        return;
    }
    ENetPacket *packet = enet_packet_create(
        data.data(),
        data.size(),
        ENET_PACKET_FLAG_RELIABLE
    );
    enet_peer_send(serverPeer, 0, packet);
}

void NetworkClient::update(ECS::Registry &registry)
{
    std::lock_guard<std::mutex> lock(queueMutex);
    while (!messageQueue.empty()) {
        ENetEvent event = messageQueue.front();
        messageQueue.pop();

        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
            handleServerMessage(event.packet, registry);
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_CONNECT:
            isConnected = true;
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            isConnected = false;
            serverPeer = nullptr;
            break;
        case ENET_EVENT_TYPE_NONE:
            break;
        }
    }
}

void NetworkClient::handleServerMessage(ENetPacket *packet, ECS::Registry &registry)
{
    if (packet->dataLength < 1)
        return;

    uint8_t messageType = packet->data[0];
    std::vector<uint8_t> data(packet->data + 1, packet->data + packet->dataLength);

    switch (messageType) {
    case 0xFF: {
        if (data.size() >= 2 * sizeof(uint32_t)) {
            std::memcpy(&clientId, data.data(), sizeof(uint32_t));
            std::memcpy(&PlayerEntityId, data.data() + sizeof(uint32_t), sizeof(uint32_t));
            std::cout << "Received Client ID: " << clientId
                      << " and Player Entity ID: " << PlayerEntityId << std::endl;
        } else {
            std::cerr << "Invalid client ID message received." << std::endl;
        }
        break;
    }
    default:
        Protocol::handleMessage(packet, registry);
        break;
    }
}


size_t NetworkClient::getPlayerEntityId()
{
    return this->PlayerEntityId;
}

void NetworkClient::registerHandler(DataType messageType, MessageHandler handler)
{
    MessageHandlers[messageType] = std::move(handler);
}
