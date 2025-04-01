

#include <stdexcept>

#include "../include/NetworkServer.hpp"
#include "../../ecs/include/SparseArray.hpp"
#include "Components.hpp"

/** @brief: Constructor of the networkManager class
*
* This method cobstruct the different variable for initialisation of enet, the port and the load of clients
* the constructor will also statrt the thread
*
* @param uint16_t for the port number, and a size_t variable to laod the number of clients
* @return Error if the number of elements exepected by [server] are not met
*
*/

NetworkServer::NetworkServer(uint16_t port, size_t maxClients) : nextClientId(0)
{
    if (enet_initialize() != 0) {
      throw std::runtime_error("enet_initialize() failed");
    }

    ENetAddress address;
    address.port = port;
    address.host = ENET_HOST_ANY;

    server = enet_host_create(&address, maxClients, 2, 0, 0);
    if (!server) {
      enet_deinitialize();
      throw std::runtime_error("enet_host_create_server() failed");
    }
    networkThread = std::thread(&NetworkServer::networkLoop, this);
}

NetworkServer::~NetworkServer()
{
    isRunning = false;
    if (networkThread.joinable())
        networkThread.join();

    if (server)
        enet_host_destroy(server);

    enet_deinitialize();
}

void NetworkServer::networkLoop() {
    while (isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        /**  @brief : Plus de polling ici ,
        il est géré dans update() j'ai commenter l'ancienne
        car cette methode est encore en test phase */
    }
}

void NetworkServer::registerHandler(uint8_t messageType, MessageHandler handler)
{
  messageHandlers[messageType] = std::move(handler);
}

void NetworkServer::update(ECS::Registry &registry)
{
    while (enet_host_service(server, &event, 0) > 0) {
        messageQueue.push(event);
    }
    while (messageQueue.pop(event)) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                HandleClientConnection(event.peer, registry);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                HandleClientDisconnection(event.peer, registry);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handleClientMessage(event.peer, event.packet, registry);
                enet_packet_destroy(event.packet);
                break;
            default:
                break;
        }
    }

    auto &scenes = registry.getComponents<Scene>();
    for (auto &&[i, scene] : indexedZipper(scenes)) {
        if (scene.has_value()) {
            auto data = Protocol::serializeGameState(scene.value(), DataType::SCE);
            uint8_t entityId = static_cast<uint8_t>(i);
            data.insert(data.begin(), entityId);
            broadcast(data);
        }
    }
}

/**
 * @brief Handles a new client connection by assigning a unique ID and triggering optional handlers.
 *
 * This method is called when a new client connects to the server. It assigns the client a unique ID,
 * stores it in internal data structures, and attaches it to the client object (`peer`) for quick access.
 * It also triggers a message handler if one is defined for the connection event type (0xFF).
 *
 * @param peer A pointer to the ENetPeer object representing the connected client.
 *
 * @return None.
 *
 * @details
 * - The client's ID is generated using an incrementing counter (`nextClientId`) and stored in a map
 *   (`clientIds`) for future lookups.
 * - The ID is also attached to the `peer` object via its generic `data` field after converting it
 *   to a `void*`.
 * - A message indicating a new connection (type 0xFF) is created and passed to the appropriate
 *   message handler, if one is registered.
 *
 * @note
 * - The `peer->data` field is typecasted to and from `void*`. Ensure consistency when reading or
 *   modifying this field in other parts of the code.
 * - The message handler system allows extensibility for custom connection logic.
 *
 * @see
 * - `std::unordered_map` for the `clientIds` and `messageHandlers` structures.
 * - `ENetPeer` for the client representation.
 */


/*
    - player contient clientId
    - On veut différencier les inputSystem
    - Chaque inputstate a un clientId
    -
*/

void NetworkServer::initPlayer(ECS::Registry &registry, uint32_t clientId, uint32_t &playerEntityId)
{
    ECS::Entity entityPlayer = registry.spawnEntity();
    playerEntityId = static_cast<uint32_t>(entityPlayer);

    Player player(playerEntityId);
    Controllable controllable(false, false, false, false, false, playerEntityId);

    registry.addComponent<Velocity>(entityPlayer, Velocity{0.0f, 0.0f});
    registry.addComponent<Player>(entityPlayer, player);
    registry.addComponent<Controllable>(entityPlayer, controllable);

    Animatable animatable({ {1, 3, false, false, 5, static_cast<int>(clientId), 1.0f},
                              {0, 5, false, false, 5, static_cast<int>(clientId), 100.0f} }, 2);
    animatable.animationMap["Down"] = 1;
    animatable.animationMap["Idle"] = 2;
    animatable.animationMap["Up"] = 3;
    registry.addComponent<Health>(entityPlayer, Health{30, 30, false});
    registry.addComponent<TextRenderable>(entityPlayer, TextRenderable{"Life", "Life 700", "../../client/assets/fonts/r-type.ttf", 15, 0 , 0, 1, true, 255.0, 255.0, 255.0, 255.0});
    registry.addComponent<Animatable>(entityPlayer, animatable);
    registry.addComponent<Renderable>(entityPlayer, Renderable{
        "../../client/assets/sprites/r-typesheet42.gif", 166, 17, 0.0f, 0.0f, 1, true, true, 0});
    registry.addComponent<Color>(entityPlayer, Color{255.f, 255.f, 255.f, 255.f});
    registry.addComponent<Transformable>(entityPlayer, Transformable{200.0f, 300.0f, 0.0f, 2.1f, 2.1f});
    registry.addComponent<ClockComponent>(entityPlayer, ClockComponent{0.0f});
    MultiCollidable multiColl;
    multiColl.collidables.push_back(Collidable{
        33.0f, 36.0f, false,
        CollisionCategory::Player,
        static_cast<uint8_t>(CollisionCategory::EnemyMissile)
    });
    multiColl.collidables.push_back(Collidable{
        33.0f, 36.0f, false,
        CollisionCategory::Player,
        static_cast<uint8_t>(CollisionCategory::ShieldBonus)
    });
    multiColl.collidables.push_back(Collidable{
        33.0f, 36.0f, false,
        CollisionCategory::Player,
        static_cast<uint8_t>(CollisionCategory::Heal)
    });
    multiColl.collidables.push_back(Collidable{
        33.0f, 36.0f, false,
        CollisionCategory::Player,
        static_cast<uint8_t>(CollisionCategory::Enemy)
    });
    registry.addComponent<MultiCollidable>(entityPlayer, multiColl);
    registry.addComponent<Score>(entityPlayer, Score{0, 0, false});
    _clientToEntity[clientId] = playerEntityId;
}

void NetworkServer::HandleClientConnection(ENetPeer *peer, ECS::Registry &registry)
{
    uint32_t clientId = nextClientId++;
    clientIds[peer] = clientId;

    uint32_t playerEntityId;
    initPlayer(registry, clientId, playerEntityId);

    std::vector<uint8_t> clientIdMessage = {0xFF};
    {
        uint8_t *cIdBytes = reinterpret_cast<uint8_t*>(&clientId);
        clientIdMessage.insert(clientIdMessage.end(), cIdBytes, cIdBytes + sizeof(uint32_t));
    }
    {
        uint8_t *entIdBytes = reinterpret_cast<uint8_t*>(&playerEntityId);
        clientIdMessage.insert(clientIdMessage.end(), entIdBytes, entIdBytes + sizeof(uint32_t));
    }

    ENetPacket *packet = enet_packet_create(
        clientIdMessage.data(),
        clientIdMessage.size(),
        ENET_PACKET_FLAG_RELIABLE
    );
    enet_peer_send(peer, 0, packet);
}


/** @brief: Handles a client disconnection from the server
*
* This method removes the client from internal tracking structures and triggers
* a disconnection message handler if one is registered.
*
* @param peer A pointer to the ENetPeer object representing the disconnected client.
* @return None.
*
* @details
* - Retrieves the client's ID from the 'client ids' map and removes the client entry.
* - Constructs a disconnection message (type 0xFE) and calls the associated handler, if defined.
*
* @note
* - Ensure that the client ID is properly tracked and removed to avoid memory issues.
*/
void NetworkServer::HandleClientDisconnection(ENetPeer *peer, ECS::Registry &registry)
{
    uint32_t clientId = clientIds[peer];

    auto it = _clientToEntity.find(clientId);
    if (it != _clientToEntity.end()) {
        nextClientId--;
        size_t playerEntity = it->second;
        registry.killEntity(registry.entityFromIndex(playerEntity));
        _clientToEntity.erase(it);
    }
    clientIds.erase(peer);
    std::vector<uint8_t> disconnectMessage = {0xFE};
    if (messageHandlers.count(0xFF)) {
        messageHandlers[0xFF](clientId, disconnectMessage);
    }
}


/**
 * @brief Handles and processes incoming client messages.
 *
 * This function extracts the message type and data from the received packet and calls the appropriate
 * message handler, if one exists.
 *
 * @param peer A pointer to the ENetPeer object representing the client.
 * @param packet A pointer to the ENetPacket containing the message data.
 *
 * @return None.
 *
 * @details
 * - Validates the packet size.
 * - Extracts the message type and data.
 * - Retrieves the client ID from `clientIds`.
 * - Calls the appropriate handler if a handler for the message type exists.
 */

void NetworkServer::handleClientMessage(ENetPeer* peer, ENetPacket* packet, ECS::Registry &registry)
{
    if (packet->dataLength < 1) return;

    uint32_t clientId = clientIds[peer];
    (void) clientId;

    Protocol::handleMessage(packet, registry);
}
/**
 * @brief Broadcasts the current game state to all clients.
 *
 * This function serializes the game state and sends it as a reliable packet to all connected clients.
 *
 * @param state The current game state to be broadcasted.
 *
 * @return None.
 *
 * @details
 * - Serializes the `state` into a vector of bytes.
 * - Creates a reliable ENet packet containing the serialized data.
 * - Broadcasts the packet to all clients using the `enet_host_broadcast` function.
 */

void NetworkServer::broadcast(const std::vector<uint8_t> &data)
{
    ENetPacket* packet = enet_packet_create(
        data.data(),
        data.size(),
        ENET_PACKET_FLAG_RELIABLE
    );
    enet_host_broadcast(server, 0, packet);
}

/**
 * @brief Finds the peer based on IDs of all clients.
 *
 * This method travels thru the different peer and associated id
 * to find the correct id and return the correct associated peer
 *
 * @param client id
 * @return if found --> peer
 * @return if not found --> a null pointer for memory management
 */

ENetPeer* NetworkServer::FindPeersPerIds(uint32_t clientId)
{
    for (const auto& [peer, id] : clientIds) {
        if (id == clientId) {
            return peer;
        }
    }
    return nullptr;
}

size_t NetworkServer::getClientCount() const
{
    return clientIds.size();
}
