#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>
#include <queue>
#include <functional>
#include <stdexcept>

#include "Entity.hpp"
#include "SparseArray.hpp"

namespace ECS
{
    class Registry
    {
        public:
            template <typename Component>
            SparseArray<Component> &registerComponent();

            template <typename Component>
            SparseArray<Component> &getComponents();

            template <typename Component>
            SparseArray<Component> const &getComponents() const;

            Entity spawnEntity();
            Entity entityFromIndex(std::size_t index) const;
            void killEntity(const Entity &entity);

            template <typename Component>
            typename SparseArray<Component>::referenceType addComponent(const Entity &entity, Component &&component);

            template <typename Component>
            typename SparseArray<Component>::referenceType addComponent(const Entity &entity, const Component &component);

            template <typename Component, typename... Params>
            typename SparseArray<Component>::referenceType emplaceComponent(const Entity &entity, Params&&... params);

            template <typename Component>
            void removeComponent(const Entity &entity);

            template <typename... Components, typename Function, typename... Args>
            void addSystem(Function &&system, Args&&... args);

            void runSystems();

            std::size_t getEntityCount();

        private:
            //containers
            std::unordered_map<std::type_index, std::any> _componentsArray;
            std::vector<std::function<void(const Entity &)>> _eraseFunctions;
            std::vector<std::function<void(Registry&)>> _systems;

            //entity management
            std::queue<std::size_t> _availableEntityIds;
            std::size_t nextEntityId = 0;

            //helper
            void eraseEntityComponents(const Entity &entity);
    };

    template <typename Component>
    SparseArray<Component> &Registry::registerComponent()
    {
        std::type_index typeIdx(typeid(Component));
        if (this->_componentsArray.find(typeIdx) != this->_componentsArray.end()){
            throw std::runtime_error("Component type already registered");
        }
        this->_componentsArray[typeIdx] = SparseArray<Component>();
        this->_eraseFunctions.emplace_back([this](const Entity& entity) {
            auto &array = getComponents<Component>();
            size_t entityId = static_cast<size_t>(entity);
            if (entityId < array.size()) {
                array.erase(entityId);
            }
        });
        return std::any_cast<SparseArray<Component>&>(this->_componentsArray[typeIdx]);
    }

    inline std::size_t Registry::getEntityCount()
    {
        return nextEntityId;
    }

    template <typename Component>
    SparseArray<Component> &Registry::getComponents()
    {
        std::type_index typeIdx(typeid(Component));
        auto it = this->_componentsArray.find(typeIdx);
        if (it == this->_componentsArray.end()){
            throw std::runtime_error("Component type not registered");
        }
        return std::any_cast<SparseArray<Component>&>(it->second);
    }

    template <typename Component>
    const SparseArray<Component> &Registry::getComponents() const
    {
        std::type_index typeIdx(typeid(Component));
        auto it = this->_componentsArray.find(typeIdx);
        if (it == this->_componentsArray.end()) {
            throw std::runtime_error("Component type not registered.");
        }
        return std::any_cast<const SparseArray<Component>&>(it->second);
    }

    inline Entity Registry::spawnEntity()
    {
        size_t entityId;
        if (!this->_availableEntityIds.empty()) {
            entityId = this->_availableEntityIds.front();
            this->_availableEntityIds.pop();
        } else {
            entityId = nextEntityId++;
        }
        return Entity(entityId);
    }

    inline Entity Registry::entityFromIndex(std::size_t idx) const
    {
        if (idx >= nextEntityId) {
            throw std::out_of_range("Entity index out of range.");
        }
        return Entity(idx);
    }

    inline void Registry::killEntity(const Entity &entity)
    {
        size_t entityId = static_cast<size_t>(entity);
        this->_availableEntityIds.push(entityId);
        eraseEntityComponents(entity);
    }

    inline void Registry::eraseEntityComponents(const Entity &entity)
    {
        for (auto& eraseFunc : this->_eraseFunctions) {
            eraseFunc(entity);
        }
    }

    template <typename Component>
    typename SparseArray<Component>::referenceType Registry::addComponent(const Entity &entity, Component &&component)
    {
        auto &array = getComponents<Component>();
        size_t entityId = static_cast<size_t>(entity);
        return array.insertAt(entityId, std::forward<Component>(component));
    }

    template <typename Component>
    typename SparseArray<Component>::referenceType Registry::addComponent(const Entity &entity, const Component &component)
    {
        auto &array = getComponents<Component>();
        size_t entityId = static_cast<size_t>(entity);
        return array.insertAt(entityId, component);
    }

    template <typename Component, typename... Params>
    typename SparseArray<Component>::referenceType Registry::emplaceComponent(const Entity &entity, Params&&... params)
    {
        auto &array = getComponents<Component>();
        size_t entityId = static_cast<size_t>(entity);
        return array.emplaceAt(entityId, std::forward<Params>(params)...);
    }

    template <typename Component>
    void Registry::removeComponent(const Entity &entity)
    {
        auto &array = getComponents<Component>();
        size_t entityId = static_cast<size_t>(entity);
        if (entityId < array.size()) {
            array.erase(entityId);
        }
    }

    template <typename... Components, typename Function, typename... Args>
    void Registry::addSystem(Function&& system, Args&&... args)
    {
        auto boundArgs = std::make_tuple(std::forward<Args>(args)...);
        auto systemWrapper = [system = std::forward<Function>(system), boundArgs](Registry& registry) mutable {
            std::apply([&registry, &system](auto&... args) {
                if constexpr (std::is_invocable_v<Function, Registry&, Args..., SparseArray<Components>&...>) {
                    system(registry, args..., registry.getComponents<Components>()...);
                }
                else if constexpr (std::is_invocable_v<Function, Args..., SparseArray<Components>&...>) {
                    system(args..., registry.getComponents<Components>()...);
                }
                else {
                    static_assert(
                        std::is_invocable_v<Function, Registry&, Args..., SparseArray<Components>&...> ||
                        std::is_invocable_v<Function, Args..., SparseArray<Components>&...>,
                        "System fufuction must be invocable with (Registry&, Args..., Components...) or (Args..., Components...)"
                    );
                }
            }, boundArgs);
        };
        _systems.emplace_back(std::move(systemWrapper));
    }

    inline void Registry::runSystems()
    {
        for (auto &system: _systems)
            system(*this);
    }
}
