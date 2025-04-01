

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <vector>
#include <memory>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "Components.hpp"

enum class MsgTypes {
    ClientConnect,
    Welcome,
    ServerPing,
    ClientDisconnect,
    AKeyPressed,
    Move,
    GameState,
    GameStateRequest
};

enum class Direction : uint8_t {
    Up,
    Down,
    Left,
    Right
};

struct MoveCommand {
    Direction direction;
};



struct GameState {
    std::unordered_map<uint32_t, Transformable> clientPositions;
};

namespace net
{
    template <typename T>
    struct message_header
    {
        T id{};
        uint32_t size = 0;
    };

    template <typename T>
    struct message
    {
        message_header<T> header{};
        std::vector<uint8_t> body;

        mutable size_t extractionOffset = 0;

        size_t size() const
        {
            return sizeof(message_header<T>) + body.size();
        }

        friend std::ostream &operator<<(std::ostream &os, const message<T> &msg)
        {
            os << "ID: " << static_cast<int>(msg.header.id)
               << " Size: " << msg.header.size;
            return os;
        }

        // Serialization operator for standard types
        template <typename DataType>
        friend message<T> &operator<<(message<T> &msg, const DataType &data)
        {
            static_assert(std::is_standard_layout<DataType>::value,
                          "Data is too complex to be pushed into vector");
            size_t i = msg.body.size();
            msg.body.resize(msg.body.size() + sizeof(DataType));
            std::memcpy(msg.body.data() + i, &data, sizeof(DataType));
            msg.header.size = msg.body.size();

            return msg;
        }

        // Deserialization operator for standard types
        template <typename DataType>
        friend message<T> &operator>>(message<T> &msg, DataType &data)
        {
            static_assert(std::is_standard_layout<DataType>::value,
                          "Data is too complex to be pulled from vector");

            if (msg.extractionOffset + sizeof(DataType) > msg.body.size())
                throw std::runtime_error("Not enough data in message body to extract");

            std::memcpy(&data, msg.body.data() + msg.extractionOffset, sizeof(DataType));
            msg.extractionOffset += sizeof(DataType);

            return msg;
        }

        // Serialization operator for std::string
        friend message<T> &operator<<(message<T> &msg, const std::string &data)
        {
            uint32_t length = static_cast<uint32_t>(data.size());
            msg << length;

            size_t i = msg.body.size();
            msg.body.resize(msg.body.size() + length);
            std::memcpy(msg.body.data() + i, data.data(), length);
            msg.header.size = msg.body.size();

            return msg;
        }

        // Deserialization operator for std::string
        friend message<T> &operator>>(message<T> &msg, std::string &data)
        {
            uint32_t length;
            msg >> length;

            if (msg.extractionOffset + length > msg.body.size())
                throw std::runtime_error("Not enough data in message body to extract string");

            data.resize(length);
            std::memcpy(&data[0], msg.body.data() + msg.extractionOffset, length);
            msg.extractionOffset += length;

            return msg;
        }

        // Serialization operator for Transformable
        friend message<T> &operator<<(message<T> &msg, const Transformable &data)
        {
            msg << data.positionX;
            msg << data.positionY;
            msg << data.rotation;
            msg << data.scaleX;
            msg << data.scaleY;
            return msg;
        }

        // Deserialization operator for Transformable
        friend message<T> &operator>>(message<T> &msg, Transformable &data)
        {
            msg >> data.positionX;
            msg >> data.positionY;
            msg >> data.rotation;
            msg >> data.scaleX;
            msg >> data.scaleY;
            return msg;
        }

        // Serialization operator for Velocity
        friend message<T> &operator<<(message<T> &msg, const Velocity &data)
        {
            msg << data.vx;
            msg << data.vy;
            return msg;
        }

        // Deserialization operator for Velocity
        friend message<T> &operator>>(message<T> &msg, Velocity &data)
        {
            msg >> data.vx;
            msg >> data.vy;
            return msg;
        }

        // Serialization operator for Controllable
        friend message<T> &operator<<(message<T> &msg, const Controllable &data)
        {
            msg << data.moveLeft;
            msg << data.moveRight;
            msg << data.moveUp;
            msg << data.moveDown;
            msg << data.attack;
            return msg;
        }

        // Deserialization operator for Controllable
        friend message<T> &operator>>(message<T> &msg, Controllable &data)
        {
            msg >> data.moveLeft;
            msg >> data.moveRight;
            msg >> data.moveUp;
            msg >> data.moveDown;
            msg >> data.attack;
            return msg;
        }

        // Serialization operator for Player
        friend message<T> &operator<<(message<T> &msg, const Player &data)
        {
            msg << data.playerId;
            return msg;
        }

        // Deserialization operator for Player
        friend message<T> &operator>>(message<T> &msg, Player &data)
        {
            msg >> data.playerId;
            return msg;
        }

        // Serialization operator for Health
        friend message<T> &operator<<(message<T> &msg, const Health &data)
        {
            msg << data.currentHealth;
            msg << data.maxHealth;
            return msg;
        }

        // Deserialization operator for Health
        friend message<T> &operator>>(message<T> &msg, Health &data)
        {
            msg >> data.currentHealth;
            msg >> data.maxHealth;
            return msg;
        }

        // Serialization operator for Renderable
        friend message<T> &operator<<(message<T> &msg, const Renderable &data)
        {
            msg << data.textureId;
            msg << data.width;
            msg << data.height;
            msg << data.originX;
            msg << data.originY;
            msg << data.zOrder;
            msg << data.visible;
            return msg;
        }

        // Deserialization operator for Renderable
        friend message<T> &operator>>(message<T> &msg, Renderable &data)
        {
            msg >> data.textureId;
            msg >> data.width;
            msg >> data.height;
            msg >> data.originX;
            msg >> data.originY;
            msg >> data.zOrder;
            msg >> data.visible;
            return msg;
        }

        // Serialization operator for Scene
        friend message<T> &operator<<(message<T> &msg, const Scene &data)
        {
            msg << data.currentScene;
            return msg;
        }

        // Deserialization operator for Scene
        friend message<T> &operator>>(message<T> &msg, Scene &data)
        {
            msg >> data.currentScene;
            return msg;
        }

        // Serialization operator for Audio
        friend message<T> &operator<<(message<T> &msg, const Audio &data)
        {
            msg << data.audioPath;
            msg << data.isPlaying;
            msg << data.loop;
            msg << data.volume;
            return msg;
        }

        // Deserialization operator for Audio
        friend message<T> &operator>>(message<T> &msg, Audio &data)
        {
            msg >> data.audioPath;
            msg >> data.isPlaying;
            msg >> data.loop;
            msg >> data.volume;
            return msg;
        }

        // Serialization operator for Animatable
        friend message<T> &operator<<(message<T> &msg, const Animatable &data)
        {
            msg << data.currentFrame;
            msg << data.frameCount;
            msg << data.frameDuration;
            msg << data.animationTime;
            return msg;
        }

        // Deserialization operator for Animatable
        friend message<T> &operator>>(message<T> &msg, Animatable &data)
        {
            msg >> data.currentFrame;
            msg >> data.frameCount;
            msg >> data.frameDuration;
            msg >> data.animationTime;
            return msg;
        }

        // Serialization operator for ParallaxScroller
        friend message<T> &operator<<(message<T> &msg, const ParallaxScroller &data)
        {
            msg << data.scrollSpeedX;
            msg << data.scrollSpeedY;
            msg << data.zLayer;
            msg << data.infiniteScroll;
            msg << data.offsetX;
            msg << data.offsetY;
            return msg;
        }

        // Deserialization operator for ParallaxScroller
        friend message<T> &operator>>(message<T> &msg, ParallaxScroller &data)
        {
            msg >> data.scrollSpeedX;
            msg >> data.scrollSpeedY;
            msg >> data.zLayer;
            msg >> data.infiniteScroll;
            msg >> data.offsetX;
            msg >> data.offsetY;
            return msg;
        }

        // Serialization operator for ClockComponent
        friend message<T> &operator<<(message<T> &msg, const ClockComponent &data)
        {
            msg << data.dt;
            return msg;
        }

        // Deserialization operator for ClockComponent
        friend message<T> &operator>>(message<T> &msg, ClockComponent &data)
        {
            msg >> data.dt;
            return msg;
        }

        // Serialization operator for MoveCommand
        friend message<T> &operator<<(message<T> &msg, const MoveCommand &cmd)
        {
            msg << static_cast<uint8_t>(cmd.direction);
            return msg;
        }

        // Deserialization operator for MoveCommand
        friend message<T> &operator>>(message<T> &msg, MoveCommand &cmd)
        {
            uint8_t dir;
            msg >> dir;
            cmd.direction = static_cast<Direction>(dir);
            return msg;
        }

        // Serialization operator for GameState
        friend message<T>& operator<<(message<T>& msg, const GameState& data)
        {
            // Serialize clientPositions map
            uint32_t clientCount = static_cast<uint32_t>(data.clientPositions.size());
            msg << clientCount;
            for (const auto& [clientID, transform] : data.clientPositions)
            {
                msg << clientID;
                msg << transform;
            }

            return msg;
        }

        // Deserialization operator for GameState
        friend message<T>& operator>>(message<T>& msg, GameState& data)
        {
            uint32_t clientCount;
            msg >> clientCount;
            data.clientPositions.clear();
            for(uint32_t i = 0; i < clientCount; ++i)
            {
                uint32_t clientID;
                Transformable transform;
                msg >> clientID;
                msg >> transform;
                data.clientPositions[clientID] = transform;
            }

            return msg;
        }
    };

    template<typename T>
    class Connection;

    template <typename T>
    struct owned_message
    {
        std::shared_ptr<Connection<T>> remote = nullptr;
        message<T> msg;

        friend std::ostream &operator<<(std::ostream &os, const owned_message<T> &msg)
        {
            os << msg.msg;
            return os;
        }
    };
}

#endif // MESSAGE_HPP_
