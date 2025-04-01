

#pragma once

#include "enet/enet.h"
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <chrono>
#include <variant>
#include <iostream>
#include <cstring>
#include <map>

#include "Registry.hpp"
#include "../components/Components.hpp"

enum class DataType : uint8_t
{
    TRA = 1,
    VEL = 2,
    CON = 3,
    PLA = 4,
    HEA = 5,
    REN = 6,
    SCE = 7,
    AUD = 8,
    ANI = 9,
    ANIM = 10,
    PAR = 11,
    CLO = 12,
    SHA = 13,
    COLO = 14,
    SOU = 15,
    SCO = 16,
    TEX = 17
};

template <typename T>
struct is_std_vector : std::false_type {};

template <typename T, typename Alloc>
struct is_std_vector<std::vector<T, Alloc>> : std::true_type {};

template <typename T>
constexpr bool is_std_vector_v = is_std_vector<T>::value;

template <typename T>
struct is_std_map : std::false_type {};

template <typename Key, typename Value, typename Comp, typename Alloc>
struct is_std_map<std::map<Key, Value, Comp, Alloc>> : std::true_type {};

template <typename T>
constexpr bool is_std_map_v = is_std_map<T>::value;

namespace Protocol
{

    template <typename Component>
    void updateRegistry(ECS::Registry &registry, size_t entityId, const Component &component)
    {
        auto &components = registry.getComponents<Component>();
        if (entityId >= registry.getEntityCount()) {
            while (entityId >= registry.getEntityCount()) {
                // std::cout << "Entity spawned ID" << std::endl;
                registry.spawnEntity();
            }
        }
        if (entityId >= components.size()) {
            registry.addComponent<Component>(registry.entityFromIndex(entityId), component);
        } else {
            if (!components[entityId].has_value()) {
                registry.addComponent<Component>(registry.entityFromIndex(entityId), component);
            } else {

                *components[entityId] = component;
            }
        }
    }

    template <typename Component>
    inline std::vector<uint8_t> serializeGameState(const Component &comp, DataType dataType)
    {
        std::vector<uint8_t> data;

        try {
            data.push_back(static_cast<uint8_t>(dataType));
            std::apply([&](auto&&... args) {
                ([&data](const auto &arg) {
                    using ArgType = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<ArgType, std::string>) {
                        data.push_back(static_cast<uint8_t>(arg.size()));
                        data.insert(data.end(), arg.begin(), arg.end());
                    } else if constexpr (is_std_vector_v<ArgType>) {
                        data.push_back(static_cast<uint8_t>(arg.size()));
                        for (const auto &elem : arg) {
                            const uint8_t *bytes = reinterpret_cast<const uint8_t*>(&elem);
                            data.insert(data.end(), bytes, bytes + sizeof(elem));
                        }
                    } else if constexpr (is_std_map_v<ArgType>) {
                        data.push_back(static_cast<uint8_t>(arg.size()));
                        for (const auto& [key, mapped] : arg) {
                            if constexpr (std::is_same_v<typename ArgType::key_type, std::string>) {
                                data.push_back(static_cast<uint8_t>(key.size()));
                                data.insert(data.end(), key.begin(), key.end());
                            } else {
                                const uint8_t *keyBytes = reinterpret_cast<const uint8_t*>(&key);
                                data.insert(data.end(), keyBytes, keyBytes + sizeof(key));
                            }

                            const uint8_t *mappedBytes = reinterpret_cast<const uint8_t*>(&mapped);
                            data.insert(data.end(), mappedBytes, mappedBytes + sizeof(mapped));
                        }
                    } else {
                        const uint8_t *bytes = reinterpret_cast<const uint8_t*>(&arg);
                        data.insert(data.end(), bytes, bytes + sizeof(arg));
                    }
                }(args), ...);
            }, comp.as_tuple());
        } catch (const std::exception& e) {
            std::cerr << "Serialization error: " << e.what() << std::endl;
        }

        return data;
    }

    template <typename T, size_t Index = 0, typename Tuple>
    inline void deserializeHelper(const std::vector<uint8_t>& data, Tuple& tuple, size_t& offset)
    {
        if constexpr (Index < std::tuple_size<Tuple>::value) {
            using FieldType = std::tuple_element_t<Index, Tuple>;
            if constexpr (std::is_same_v<FieldType, std::string>) {
                size_t length = static_cast<size_t>(data[offset]);
                offset++;
                std::string value(data.begin() + offset, data.begin() + offset + length);
                std::get<Index>(tuple) = value;
                offset += length;
            } else if constexpr (is_std_vector_v<FieldType>) {
                size_t length = static_cast<size_t>(data[offset]);
                offset++;
                FieldType value;
                value.reserve(length);
                for (size_t i = 0; i < length; ++i) {
                    typename FieldType::value_type elem;
                    std::memcpy(&elem, data.data() + offset, sizeof(elem));
                    offset += sizeof(elem);
                    value.push_back(elem);
                }
                std::get<Index>(tuple) = value;
            } else if constexpr (is_std_map_v<FieldType>) {
                size_t length = static_cast<size_t>(data[offset]);
                offset++;
                FieldType value;
                for (size_t i = 0; i < length; ++i) {
                    typename FieldType::key_type key;
                    typename FieldType::mapped_type mapped;
                    if constexpr (std::is_same_v<typename FieldType::key_type, std::string>) {
                        size_t keyLength = static_cast<size_t>(data[offset]);
                        offset++;
                        key = std::string(data.begin() + offset, data.begin() + offset + keyLength);
                        offset += keyLength;
                    } else {
                        std::memcpy(&key, data.data() + offset, sizeof(key));
                        offset += sizeof(key);
                    }
                    std::memcpy(&mapped, data.data() + offset, sizeof(mapped));
                    offset += sizeof(mapped);
                    value[key] = mapped;
                }
                std::get<Index>(tuple) = value;
            } else {
                std::memcpy(&std::get<Index>(tuple), data.data() + offset, sizeof(FieldType));
                offset += sizeof(FieldType);
            }
            deserializeHelper<T, Index + 1>(data, tuple, offset);
        }
    }

    template <typename T>
    inline T deserialize(const std::vector<uint8_t>& data)
    {
        T result{};
        auto tuple = result.as_tuple();
        size_t offset = 0;

        deserializeHelper<T>(data, tuple, offset);

        std::apply([&](auto&&... args) {
            result = T{args...};
        }, tuple);

        return result;
    }

    inline void handleMessage(ENetPacket *packet, ECS::Registry &registry)
    {
        if (packet->dataLength < 2) {
            return;
        }

        size_t entityId = static_cast<size_t>(packet->data[0]);
        DataType messageType = static_cast<DataType>(packet->data[1]);
        std::vector<uint8_t> data(packet->data + 2, packet->data + packet->dataLength);

        switch (messageType) {
            case DataType::TRA: {
                Transformable pos = deserialize<Transformable>(data);
                updateRegistry(registry, entityId, pos);
                break;
            }
            case DataType::VEL: {
                Velocity vel = deserialize<Velocity>(data);
                updateRegistry(registry, entityId, vel);
                break;
            }
            case DataType::REN: {
                Renderable render = deserialize<Renderable>(data);
                updateRegistry(registry, entityId, render);
                break;
            }
            case DataType::PAR: {
                ParallaxScroller parallax = deserialize<ParallaxScroller>(data);
                updateRegistry(registry, entityId, parallax);
                break;
            }
            case DataType::PLA: {
                Player player = deserialize<Player>(data);
                updateRegistry(registry, entityId, player);
                break;
            }
            case DataType::CON: {
                Controllable cont = Protocol::deserialize<Controllable>(data);
                updateRegistry(registry, entityId, cont);
                break;
            }
            case DataType::SHA: {
                Shape shape = Protocol::deserialize<Shape>(data);
                updateRegistry(registry, entityId, shape);
                break;
            }
            case DataType::COLO: {
                Color color = Protocol::deserialize<Color>(data);
                updateRegistry(registry, entityId, color);
                break;
            }
            case DataType::SOU: {
                Audio sound = Protocol::deserialize<Audio>(data);
                updateRegistry(registry, entityId, sound);
                break;
            }
            case DataType::TEX: {
                TextRenderable txtRend = Protocol::deserialize<TextRenderable>(data);
                Protocol::updateRegistry(registry, entityId, txtRend);
                break;
            }
            default:
                break;
        }
    }

}
