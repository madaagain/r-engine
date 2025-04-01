

#include "../include/ClientGameState.hpp"
ClientGameState::ClientGameState(ECS::Registry& registry) : _registry(registry)
{
}

void ClientGameState::applyState(const GameSnap& snapshot)
{
    for (const auto& [entityId, playerState] : snapshot.players) {
        ECS::Entity entity = _registry.spawnEntity();

        try {
            entity = _registry.entityFromIndex(entityId);
        } catch (const std::out_of_range&) {
            entity = _registry.spawnEntity();
        }

        auto& positions = _registry.getComponents<Transformable>();
        if (entityId < positions.size() && positions[entityId].has_value()) {
            auto& pos = positions[entityId].value();
            pos.positionX = playerState.x;
            pos.positionY = playerState.y;
        } else {
            /** @brief: Créer une nouvelle postion */
            _registry.addComponent<Transformable>(entity, Transformable{playerState.x, playerState.y});
        }
        std::cout << "État reçu avec " << snapshot.players.size() << " joueurs" << std::endl;
        std::cout << "État reçu du serveur - Nombre d'entités : " << snapshot.players.size() << std::endl;
        auto& velocities = _registry.getComponents<Velocity>();
        if (entityId < velocities.size() && velocities[entityId].has_value()) {
            auto& vel = velocities[entityId].value();
            vel.vx = playerState.vx;
            vel.vy = playerState.vy;
        } else {
            _registry.addComponent<Velocity>(entity, Velocity{playerState.vx, playerState.vy});
        }
    }
}

ClientGameState::GameSnap ClientGameState::captureState()
{
    GameSnap snapshot;
    snapshot.timeStamp = static_cast<uint32_t>(std::time(nullptr));

    auto& posx = _registry.getComponents<Transformable>();
    auto& velx = _registry.getComponents<Velocity>();

    for (auto &&[i, pos, vel] : indexedZipper(posx, velx)) {
        if (pos.has_value() && vel.has_value()) {
            StateOfPlayer state {
                pos->positionX,
                pos->positionY,
                vel->vx,
                vel->vy,
                static_cast<uint32_t>(i),
                true
            };
            snapshot.players[i] = state;
        }
    }
    return snapshot;
    /** @todo: Jack ma  */
}
