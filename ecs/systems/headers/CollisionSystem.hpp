#pragma once

#include "ISystems.hpp"
#include "Registry.hpp"
#include "Components.hpp"
#include <functional>
#include <map>
#include <utility>
#include <iostream>
#include <algorithm>
#include <vector>

struct CategoryPair
{
    CollisionCategory c1;
    CollisionCategory c2;

    bool operator<(const CategoryPair &other) const
    {
        int minA = std::min(static_cast<int>(c1), static_cast<int>(c2));
        int maxA = std::max(static_cast<int>(c1), static_cast<int>(c2));
        int minB = std::min(static_cast<int>(other.c1), static_cast<int>(other.c2));
        int maxB = std::max(static_cast<int>(other.c1), static_cast<int>(other.c2));
        return (minA < minB) || (minA == minB && maxA < maxB);
    }
};

namespace Systems
{
    class CollisionSystem : public ISystem
    {
    public:
        using CollisionHandler = std::function<void(ECS::Registry&, size_t, size_t)>;

        CollisionSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt) { }

        void registerCollisionHandler(CollisionCategory catA, CollisionCategory catB, CollisionHandler handler)
        {
            CategoryPair key{catA, catB};
            _handlers[key] = handler;
        }

        void update(float dt) override
        {
            (void)dt;
            auto &collidables = _registry.getComponents<Collidable>();
            for (auto &&[col] : Zipper(collidables)) {
                if (col.has_value()) {
                    col->isColliding = false;
                }
            }
            auto &multiCollidables = _registry.getComponents<MultiCollidable>();
            for (auto &&[multi] : Zipper(multiCollidables)) {
                if (multi.has_value()) {
                    for (auto &col : multi->collidables)
                        col.isColliding = false;
                }
            }
            auto &transformables = _registry.getComponents<Transformable>();
            std::vector<size_t> validEntities;
            for (auto &&[idx, colOpt, transOpt] : indexedZipper(collidables, transformables)) {
                if (transOpt.has_value() && colOpt.has_value()) {
                    validEntities.push_back(idx);
                }
            }
            for (auto &&[idx, multiOpt, transOpt] : indexedZipper(multiCollidables, transformables)) {
                if (transOpt.has_value() && multiOpt.has_value()) {
                    if (std::find(validEntities.begin(), validEntities.end(), idx) == validEntities.end())
                        validEntities.push_back(idx);
                }
            }
            for (size_t i = 0; i < validEntities.size(); ++i) {
                size_t idx1 = validEntities[i];
                for (size_t j = i + 1; j < validEntities.size(); ++j) {
                    size_t idx2 = validEntities[j];
                    const Transformable &t1 = transformables[idx1].value();
                    const Transformable &t2 = transformables[idx2].value();
                    std::vector<Collidable> boxes1 = getEntityCollidables(idx1);
                    std::vector<Collidable> boxes2 = getEntityCollidables(idx2);
                    for (const auto &c1 : boxes1) {
                        for (const auto &c2 : boxes2) {
                            bool canCollide1 = (c1.bitMask & static_cast<uint8_t>(c2.category)) != 0;
                            bool canCollide2 = (c2.bitMask & static_cast<uint8_t>(c1.category)) != 0;
                            if (canCollide1 && canCollide2 && checkCollision(t1, c1, t2, c2)) {
                                processCollision(idx1, idx2, c1, c2, canCollide1, canCollide2);
                            }
                        }
                    }
                }
            }
        }

    private:
        ECS::Registry &_registry;
        float &_dt;
        std::map<CategoryPair, CollisionHandler> _handlers;

        std::vector<Collidable> getEntityCollidables(size_t idx)
        {
            std::vector<Collidable> result;

            auto &collidables = _registry.getComponents<Collidable>();
            if (idx < collidables.size() && collidables[idx].has_value())
                result.push_back(collidables[idx].value());

            auto &multiCollidables = _registry.getComponents<MultiCollidable>();
            if (idx < multiCollidables.size() && multiCollidables[idx].has_value()) {
                for (const auto &c : multiCollidables[idx].value().collidables)
                    result.push_back(c);
            }
            return result;
        }

        bool checkCollision(const Transformable &t1, const Collidable &c1,
                            const Transformable &t2, const Collidable &c2)
        {
            float leftA = t1.positionX - (c1.width / 2.0f);
            float rightA = t1.positionX + (c1.width / 2.0f);
            float topA = t1.positionY - (c1.height / 2.0f);
            float bottomA = t1.positionY + (c1.height / 2.0f);
            float leftB = t2.positionX - (c2.width / 2.0f);
            float rightB = t2.positionX + (c2.width / 2.0f);
            float topB = t2.positionY - (c2.height / 2.0f);
            float bottomB = t2.positionY + (c2.height / 2.0f);
            bool overlapX = leftA < rightB && rightA > leftB;
            bool overlapY = topA < bottomB && bottomA > topB;
            return overlapX && overlapY;
        }

        void processCollision(size_t e1, size_t e2, const Collidable &c1, const Collidable &c2, bool canCollide1, bool canCollide2)
        {
            auto &collidables = _registry.getComponents<Collidable>();
            if (canCollide1 && e1 < collidables.size() && collidables[e1].has_value())
                collidables[e1].value().isColliding = true;
            if (canCollide2 && e2 < collidables.size() && collidables[e2].has_value())
                collidables[e2].value().isColliding = true;
            auto &multiCollidables = _registry.getComponents<MultiCollidable>();
            if (canCollide1 && e1 < multiCollidables.size() && multiCollidables[e1].has_value()) {
                for (auto &box : multiCollidables[e1].value().collidables) {
                    if (box.bitMask == static_cast<uint8_t>(c2.category))
                        box.isColliding = true;
                }
            }
            if (canCollide2 && e2 < multiCollidables.size() && multiCollidables[e2].has_value()) {
                for (auto &box : multiCollidables[e2].value().collidables) {
                    if (box.bitMask == static_cast<uint8_t>(c1.category))
                        box.isColliding = true;
                }
            }
            CategoryPair key{ c1.category, c2.category };
            auto it = _handlers.find(key);
            if (it != _handlers.end()) {
                it->second(_registry, e1, e2);
            } else {
                // std::cout << "[CollisionSystem] No handler registered for categories: " 
                //           << static_cast<int>(c1.category) << " & " 
                //           << static_cast<int>(c2.category) << std::endl;
            }
        }
    };
}
