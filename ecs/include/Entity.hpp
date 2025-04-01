#pragma once

#include <cstddef>

namespace ECS
{
    class Entity
    {
    public:
        explicit Entity(size_t entityId) : _entityId(entityId) {}
        operator size_t() const { return _entityId; }
        ~Entity() = default;

        bool operator==(const Entity &other) const { return _entityId == other._entityId; }
        bool operator!=(const Entity &other) const { return !(*this == other); }
        bool operator<(const Entity &other) const { return _entityId < other._entityId; }
        size_t get_id() const { return _entityId; }

    private:
        size_t _entityId;
    };
}
