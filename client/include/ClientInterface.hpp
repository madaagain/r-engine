

#ifndef CLIENT_INTERFACE_HPP
#define CLIENT_INTERFACE_HPP

#include <asio.hpp>
#include <atomic>
#include <functional>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <iostream>
#include <array>

#include "Message.hpp"
#include "Queue.hpp"
#include "Connection.hpp"

namespace net
{
    template <typename T>
    class clientInterface
    {
    public:
        clientInterface()
            : socket(context)
        {
        }

        virtual ~clientInterface()
        {
            std::cout << "Alpha One out.\n";
            Disconnect();
        }

        bool Connect(const std::string& host, const uint16_t port)
        {
            try {
                asio::ip::udp::resolver resolver(context);
                asio::ip::udp::resolver::results_type endpoints = resolver.resolve(asio::ip::udp::v4(), host, std::to_string(port));

                server_endpoint = *endpoints.begin();

                socket.open(asio::ip::udp::v4());

                listenForMessages();

                context_thread = std::thread([this]() { context.run(); });
            } catch (std::exception& e) {
                std::cerr << "Client Exception: " << e.what() << "\n";
                return false;
            }
            return true;
        }

        void Disconnect()
        {
            context.stop();
            if (context_thread.joinable())
                context_thread.join();

            socket.close();
        }

        void send(const net::message<T>& msg)
        {
            asio::post(context,
                [this, msg]() {
                    bool writingMessage = !messages_out.empty();
                    messages_out.pushBack(msg);
                    if (!writingMessage) {
                        writeMessage();
                    }
                });
        }

        net::Queue<net::owned_message<T>>& incoming()
        {
            return messages_in;
        }

    protected:
        void writeMessage()
        {
            std::vector<uint8_t> sendBuffer;
            size_t headerSize = sizeof(net::message_header<T>);
            sendBuffer.resize(headerSize + messages_out.front().body.size());
            std::memcpy(sendBuffer.data(), &messages_out.front().header, headerSize);

            if (messages_out.front().body.size() > 0)
                std::memcpy(sendBuffer.data() + headerSize, messages_out.front().body.data(), messages_out.front().body.size());

            socket.async_send_to(
                asio::buffer(sendBuffer), server_endpoint,
                [this](std::error_code ec, std::size_t ) {
                    if (!ec) {
                        messages_out.popFront();
                        if (!messages_out.empty()) {
                            writeMessage();
                        }
                    }
                    else {
                        std::cerr << "Send error: " << ec.message() << "\n";
                        socket.close();
                    }
                });
        }

        void listenForMessages()
        {
            socket.async_receive_from(
                asio::buffer(receiveBuffer.data(), maxMessageSize), sender_endpoint,
                [this](std::error_code ec, std::size_t bytes_recvd) {
                    if (!ec && bytes_recvd > 0) {
                        processReceivedData(bytes_recvd);
                    } else {
                        std::cerr << "Receive error: " << ec.message() << "\n";
                        socket.close();
                    }

                    listenForMessages();
                });
        }

        void processReceivedData(std::size_t bytes_recvd)
        {
            net::message<T> msg;
            size_t headerSize = sizeof(net::message_header<T>);

            if (bytes_recvd < headerSize) {
                std::cerr << "Received packet too small for header\n";
                return;
            }

            std::memcpy(&msg.header, receiveBuffer.data(), headerSize);

            if (msg.header.size > 0) {
                if (bytes_recvd - headerSize < msg.header.size) {
                    std::cerr << "Incomplete message body received\n";
                    return;
                }

                msg.body.resize(msg.header.size);
                std::memcpy(msg.body.data(), receiveBuffer.data() + headerSize, msg.header.size);
            }
            messages_in.pushBack({ nullptr, msg });
        }

    public:
        bool isLinked = false;

    protected:
        asio::io_context context;
        std::thread context_thread;
        asio::ip::udp::socket socket;
        asio::ip::udp::endpoint server_endpoint;
        asio::ip::udp::endpoint sender_endpoint;

        net::Queue<net::message<T>> messages_out;
        net::Queue<net::owned_message<T>> messages_in;

        static constexpr size_t maxMessageSize = 1024;
        std::array<uint8_t, maxMessageSize> receiveBuffer;
    };
}

#endif // CLIENT_INTERFACE_HPP
