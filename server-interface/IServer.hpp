#pragma once

#include "ProtocolSerialization.hpp"
#include "Registry.hpp"
#include <cstdint>
#include <vector>

class IServer 
{
    public:
        using MessageHandler = std::function<void(uint32_t clientId, const std::vector<uint8_t> &data)>;
        ~IServer() = default;
        virtual void registerHandler(uint8_t messageType, MessageHandler handler) = 0;
        virtual void update(ECS::Registry &registry) = 0;
        virtual void broadcast(const std::vector<uint8_t> &data) = 0;
};
