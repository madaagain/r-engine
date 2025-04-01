#pragma once

#include <cstdint>
#include <vector>

#include "ProtocolSerialization.hpp"
#include "Registry.hpp"

class IClient
{
    public:
        using MessageHandler = std::function<void(const std::vector<uint8_t> &data)>;
        virtual ~IClient() = default;
        virtual bool connect(const char *address, uint16_t port, ECS::Registry &registry) = 0;
        virtual void disconnect() = 0;
        virtual void registerHandler(DataType messageType, MessageHandler handler) = 0;
        virtual void update(ECS::Registry &registry) = 0;
        virtual void sendDataToServer(const std::vector<uint8_t> &data) = 0;
        virtual uint32_t getClientId() = 0;
        virtual size_t getPlayerEntityId() = 0;
};
