

#ifndef SERVER_INTERFACE_HPP_
#define SERVER_INTERFACE_HPP_

#include <asio.hpp>
#include <thread>
#include <iostream>
#include <map>
#include <string>
#include <mutex>

#include "Queue.hpp"
#include "Connection.hpp"
#include "Message.hpp"

namespace net
{
    template <typename T>
    class serverInterface
    {

    public:
        serverInterface(uint16_t port)
            : socket(context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
        {
            std::cout << "[SERVER] Started on port " << port << "\n";
        }

        virtual ~serverInterface()
        {
            stop();
            std::cout << "\n[SERVER]: Shutting down\n";
        }

        bool start()
        {
            try {
                serverReceive();
                context_thread = std::thread([this]() { context.run(); });
            } catch (std::exception &e) {
                std::cerr << "[SERVER] Exception: " << e.what() << "\n";
                return false;
            }
            return true;
        }

        void stop()
        {
            context.stop();

            if (context_thread.joinable()) {
                context_thread.join();
            }
        }

        void sendToClient(std::shared_ptr<Connection<T>> client, const message<T> &msg)
        {
            std::cout << "Send to:" << client->getID() << std::endl;
            std::vector<uint8_t> sendBuffer;
            size_t headerSize = sizeof(message_header<T>);
            sendBuffer.resize(headerSize + msg.body.size());
            std::memcpy(sendBuffer.data(), &msg.header, headerSize);

            if (msg.body.size() > 0) {
                std::memcpy(sendBuffer.data() + headerSize, msg.body.data(), msg.body.size());
            }

            socket.async_send_to(
                asio::buffer(sendBuffer), client->getEndpoint(),
                [](std::error_code , std::size_t ) {});
        }

        void broadcastToAllClients(const message<T> &msg)
        {
            for (auto &[endpoint, client] : clients) {
                sendToClient(client, msg);
            }
        }

        void update(size_t max_messages = std::numeric_limits<size_t>::max())
        {
            size_t message_count = 0;
            while (message_count < max_messages && !incoming_messages.empty()) {
                auto msg = incoming_messages.popFront();
                onMessage(msg.remote, msg.msg);
                message_count++;
            }
        }

    protected:
        virtual void onMessage(std::shared_ptr<Connection<T>> client, message<T> &msg)
        {
            switch (msg.header.id) {
                case T::ServerPing: {
                    std::cout << "[" << client->getID() << "]: Server Ping\n";

                    int64_t timeThenEpochMs;
                    msg >> timeThenEpochMs;

                    message<T> reply;
                    reply.header.id = T::ServerPing;
                    reply << timeThenEpochMs;

                    sendToClient(client, reply);
                }
                break;
                default:
                    std::cout << "[" << client->getID() << "]: Unknown Message\n";
                    break;
                }
        }

    private:
        void serverReceive()
        {
            socket.async_receive_from(
                asio::buffer(receiveBuffer.data(), maxMessageSize), remote_endpoint,
                [this](std::error_code ec, std::size_t bytes_recvd) {
                    if (!ec && bytes_recvd > 0) {
                        processReceivedData(bytes_recvd);
                    } else {
                        std::cerr << "[SERVER] Receive error: " << ec.message() << std::endl;
                    }

                    serverReceive();
                });
        }

        void processReceivedData(std::size_t bytes_recvd)
        {
            message<T> msg;
            size_t headerSize = sizeof(message_header<T>);

            if (bytes_recvd < headerSize) {
                std::cerr << "[SERVER] Received packet too small for header\n";
                return;
            }

            std::memcpy(&msg.header, receiveBuffer.data(), headerSize);

            if (msg.header.size > 0) {
                if (bytes_recvd - headerSize < msg.header.size) {
                    std::cerr << "[SERVER] Incomplete message body received\n";
                    return;
                }

                msg.body.resize(msg.header.size);
                std::memcpy(msg.body.data(), receiveBuffer.data() + headerSize, msg.header.size);
            }

            std::shared_ptr<Connection<T>> client;
            std::lock_guard<std::mutex> lock(clientMutex);
            auto it = clients.find(remote_endpoint);
            if (it == clients.end()) {
                client = std::make_shared<Connection<T>>(remote_endpoint);
                client->setID(id_counter++);
                clients[remote_endpoint] = client;
                std::cout << "New client validated with ID: " << client->getID() << std::endl;
            } else {
                client = it->second;
            }

            incoming_messages.pushBack({ client, msg });
        }

        asio::io_context context;
        std::thread context_thread;
        asio::ip::udp::socket socket;
        Queue<owned_message<T>> incoming_messages;
        asio::ip::udp::endpoint remote_endpoint;
        static constexpr size_t maxMessageSize = 1024;
        std::array<uint8_t, maxMessageSize> receiveBuffer;

    protected:
        uint32_t id_counter = 10000;
        std::map<asio::ip::udp::endpoint, std::shared_ptr<Connection<T>>> clients;
        std::mutex clientMutex;

    };
}

#endif // SERVER_INTERFACE_HPP_
