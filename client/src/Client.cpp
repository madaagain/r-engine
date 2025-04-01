

#include "Client.hpp"

void Client::sendConnectMessage()
{
    net::message<MsgTypes> msg;
    msg.header.id = MsgTypes::ClientConnect;
    send(msg);
    std::cout << "Sent Connect message to server.\n";
}

void Client::handleWelcomeMessage(net::message<MsgTypes>& msg)
{
    uint32_t senderID;
    std::string welcomeText;
    msg >> senderID;
    msg >> welcomeText;
    std::cout << "Server says: " << welcomeText << "\n";
    setConnected(true);
    setID(senderID);
}

void Client::sendDisconnectMessage()
{
    if (getConnected()) {
        net::message<MsgTypes> msg;
        msg.header.id = MsgTypes::ClientDisconnect;

        std::vector<uint8_t> sendBuffer;
        size_t headerSize = sizeof(net::message_header<MsgTypes>);
        sendBuffer.resize(headerSize);
        std::memcpy(sendBuffer.data(), &msg.header, headerSize);
        try {
            socket.send_to(asio::buffer(sendBuffer), server_endpoint);
            std::cout << "Sent Disconnect message to server.\n";
        } catch (const std::exception& e) {
            std::cerr << "Error sending disconnect message: " << e.what() << "\n";
        }
        setConnected(false);
    }
}

void Client::handleClientDisconnect(net::message<MsgTypes>& msg)
{
    uint32_t clientID;
    msg >> clientID;
    std::cout << "Client " << clientID << " disconnected.\n";
}

void Client::pingServer()
{
    if (getConnected()) {
        std::cout << "Sending ping to server.\n";

        net::message<MsgTypes> msg;
        msg.header.id = MsgTypes::ServerPing;
        auto now = std::chrono::system_clock::now();
        int64_t now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        msg << now_ms;
        send(msg);
    } else {
        std::cout << "Cannot send ping; client is not connected.\n";
    }
}

void Client::handleServerPing(net::message<MsgTypes>& msg)
{
    int64_t timeThenEpochMs;
    msg >> timeThenEpochMs;

    auto timeThen = std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(timeThenEpochMs));
    auto timeNow = std::chrono::system_clock::now();

    std::chrono::duration<double> ping = timeNow - timeThen;
    std::cout << "Ping: " << ping.count() << " seconds\n";
}

void Client::handleAKeyPressed(net::message<MsgTypes>& msg)
{
    if (msg.header.size < sizeof(uint32_t)) {
        std::cerr << "Invalid AKeyPressed message size.\n";
        return;
    }
    uint32_t senderID;
    msg >> senderID;
    std::cout << "Client " << senderID << " pressed the 'A' key.\n";
}

void Client::sendAKeyPressed()
{
    if (getConnected()) {
        net::message<MsgTypes> msg;
        msg.header.id = MsgTypes::AKeyPressed;
        msg << getID();
        send(msg);
        std::cout << "Sent AKeyPressed message to server.\n";
    } else {
        std::cout << "Cannot send AKeyPressed; client is not connected.\n";
    }
}

void Client::handleIncomingMessages()
{
    while (!incoming().empty()) {
        auto msg = incoming().popFront().msg;
        switch (msg.header.id) {
            case MsgTypes::Welcome:
                handleWelcomeMessage(msg);
                break;
            case MsgTypes::ClientDisconnect:
                handleClientDisconnect(msg);
                break;
            case MsgTypes::ServerPing:
                handleServerPing(msg);
                break;
            case MsgTypes::AKeyPressed:
                handleAKeyPressed(msg);
                break;
            case MsgTypes::GameState:
                std::cout << "Received GameState message.\n";
                handleGameState(msg);
                break;
            default:
                std::cout << "Unknown message received.\n";
                break;
        }
    }
}

void Client::sendMove(Direction direction)
{
    if (getConnected()) {
        net::message<MsgTypes> msg;
        msg.header.id = MsgTypes::Move;
        MoveCommand cmd{ direction };
        msg << cmd;
        send(msg);
        std::cout << "Sent Move command: " << static_cast<int>(direction) << "\n";
    } else {
        std::cout << "Cannot send Move; client is not connected.\n";
    }
}

void Client::handleGameState(net::message<MsgTypes>& msg)
{
    try {
        GameState gameState;
        msg >> gameState;

        std::cout << "Received GameState with " << gameState.clientPositions.size() << " clients.\n";

        if (onGameState) {
            onGameState(gameState);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error handling GameState message: " << e.what() << "\n";
    }
}
