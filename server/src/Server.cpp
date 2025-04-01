

#include "Server.hpp"

Server::Server(int nPort, ECS::Registry registry)
    : net::serverInterface<MsgTypes>(nPort), _registry(registry)
{
}

Server::~Server()
{
    running = false;
    if (broadcasterThread.joinable())
        broadcasterThread.join();
}

bool Server::start()
{
    bool started = net::serverInterface<MsgTypes>::start();
    if (started)
    {
        startBroadcasting();
    }
    return started;
}

void Server::startBroadcasting()
{
    broadcasterThread = std::thread([this]() {
        while (running)
        {
            broadcastGameState();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
    });
}

void Server::sendMessageToClient(uint32_t clientID, const net::message<MsgTypes>& msg)
{
    std::lock_guard<std::mutex> lock(clientMutex);
    for (const auto& [endpoint, client] : clients) {
        if (client->getID() == clientID) {
            sendToClient(client, msg);
            std::cout << "Sent message to client ID " << clientID << "\n";
            return;
        }
    }
    std::cerr << "Client with ID " << clientID << " not found.\n";
}

void Server::onMessage(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg)
{
    switch (msg.header.id) {
        case MsgTypes::ClientConnect:
            handleClientConnect(client);
            break;
        case MsgTypes::ServerPing:
            handleServerPing(client, msg);
            break;
        case MsgTypes::ClientDisconnect:
            handleClientDisconnect(client);
            break;
        case MsgTypes::AKeyPressed:
            handleAKeyPressed(client, msg);
            break;
        case MsgTypes::Move:
            handleMove(client, msg);
            break;
        case MsgTypes::GameStateRequest:
            broadcastGameState();
            break;
        default:
            std::cout << "[" << client->getID() << "]: Unknown Message\n";
            break;
    }
}

void Server::handleClientConnect(std::shared_ptr<net::Connection<MsgTypes>> client)
{
    std::cout << "[" << client->getID() << "]: Client Connect\n";
    if (client->getID() == 0) {
        client->setID(clientIDCounter++);
        std::cout << "Assigned new ID to client: " << client->getID() << "\n";
    }

    net::message<MsgTypes> initMsg;
    initMsg.header.id = MsgTypes::Welcome;
    initMsg << client->getID();
    std::string welcomeMessage = "Welcome to the server!";
    initMsg << welcomeMessage;
    sendToClient(client, initMsg);

    // Initialize client's position in GameState futur -> _registry
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        gameState.clientPositions[client->getID()] = Transformable(400.0f, 300.0f, 0.0f, 1.0f, 1.0f); // Default position and scale
    }
    broadcastGameState();
}

void Server::handleClientDisconnect(std::shared_ptr<net::Connection<MsgTypes>> client)
{
    {
        std::lock_guard<std::mutex> lock(clientMutex);
        clients.erase(client->getEndpoint());
        gameState.clientPositions.erase(client->getID());
    }
    std::cout << "[" << client->getID() << "]: Client Disconnect\n";

    net::message<MsgTypes> disconnectMsg;
    disconnectMsg.header.id = MsgTypes::ClientDisconnect;
    disconnectMsg << client->getID();
    broadcastToAllClients(disconnectMsg);
}

void Server::handleServerPing(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg)
{
    std::cout << "[" << client->getID() << "]: Server Ping\n";
    int64_t timeThenEpochMs;
    msg >> timeThenEpochMs;
    net::message<MsgTypes> reply;
    reply.header.id = MsgTypes::ServerPing;
    reply << timeThenEpochMs;
    sendToClient(client, reply);
}

void Server::handleAKeyPressed(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg)
{
    (void)msg;
    std::cout << "[" << client->getID() << "]: A Key Pressed\n";
    net::message<MsgTypes> aKeyMsg;
    aKeyMsg.header.id = MsgTypes::AKeyPressed;
    aKeyMsg << client->getID();
    broadcastToAllClients(aKeyMsg);
}

void Server::handleMove(std::shared_ptr<net::Connection<MsgTypes>> client, net::message<MsgTypes>& msg)
{
    MoveCommand cmd;
    msg >> cmd;
    std::cout << "[" << client->getID() << "]: Move Command Received: " << static_cast<int>(cmd.direction) << "\n";

    float speed = 200.0f;
    float deltaTime = 0.016f;
    float newX, newY;

    {
        std::lock_guard<std::mutex> lock(clientMutex);
        auto it = gameState.clientPositions.find(client->getID());
        if (it != gameState.clientPositions.end()) {
            newX = it->second.positionX;
            newY = it->second.positionY;

            switch (cmd.direction) {
                case Direction::Up:
                    newY -= speed * deltaTime;
                    break;
                case Direction::Down:
                    newY += speed * deltaTime;
                    break;
                case Direction::Left:
                    newX -= speed * deltaTime;
                    break;
                case Direction::Right:
                    newX += speed * deltaTime;
                    break;
                default:
                    std::cerr << "Unknown Direction received.\n";
                    return;
            }

            newX = std::max(0.0f, std::min(newX, 800.0f));
            newY = std::max(0.0f, std::min(newY, 600.0f));
            it->second.positionX = newX;
            it->second.positionY = newY;
            std::cout << "Updated position for client " << client->getID() << " to (" << newX << ", " << newY << ")\n";
        } else {
            std::cerr << "Client " << client->getID() << " not found in GameState.\n";
            return;
        }
    }
    broadcastGameState();
}

void Server::broadcastGameState()
{
    GameState gameStateLocal;

    {
        std::lock_guard<std::mutex> lock(clientMutex);
        gameStateLocal = gameState;
    }

    net::message<MsgTypes> gameStateMsg;
    gameStateMsg.header.id = MsgTypes::GameState;
    gameStateMsg << gameStateLocal;

    broadcastToAllClients(gameStateMsg);
    std::cout << "Broadcasted GameState to all clients.\n";
}
