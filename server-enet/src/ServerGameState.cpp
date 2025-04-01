

#include "../include/ServerGameState.hpp"
#include <iostream>

// ServerGameState::ServerGameState(ECS::Registry& registry) : _registry(registry)
// {
//     ;

// }

// void ServerGameState::applyState(const GameSnap& snapshot)
// {
//     for (const auto& [entityId, playerState] : snapshot.players) {
//         std::cout << "???" << std::endl;
//         ECS::Entity entity = _registry.spawnEntity();

//         try {
//             entity = _registry.entityFromIndex(entityId);
//         } catch (const std::out_of_range&) {
//             entity = _registry.spawnEntity();
//         }

//         auto& positions = _registry.getComponents<Transformable>();
//         if (entityId < positions.size() && positions[entityId].has_value()) {
//             auto& pos = positions[entityId].value();
//             pos.positionX = playerState.x;
//             pos.positionY = playerState.y;
//         } else {
//             /** @brief: Créer une nouvelle postion */
//             _registry.addComponent<Transformable>(entity, Transformable{playerState.x, playerState.y});
//         }

//         auto& velocities = _registry.getComponents<Velocity>();
//         if (entityId < velocities.size() && velocities[entityId].has_value()) {
//             auto& vel = velocities[entityId].value();
//             vel.vx = playerState.vx;
//             vel.vy = playerState.vy;
//         } else {
//             _registry.addComponent<Velocity>(entity, Velocity{playerState.vx, playerState.vy});
//         }
//     }
// }

// ServerGameState::GameSnap ServerGameState::captureState()
// {
//     GameSnap snapshot;
//     snapshot.timeStamp = static_cast<uint32_t>(std::time(nullptr));

//     auto& posx = _registry.getComponents<Transformable>();
//     auto& velx = _registry.getComponents<Velocity>();

//     for (auto&& [i, pos, vel] : indexedZipper(posx, velx)) {
//         if (pos.has_value() && vel.has_value()) {
//             // std::cout << pos->x << std::endl;
//             StateOfPlayer state {
//                 pos->positionX,
//                 pos->positionY,
//                 vel->vx,
//                 vel->vy,
//                 static_cast<uint32_t>(i),
//                 true
//             };
//             // std::cout << state.vx << std::endl;
//             snapshot.players[i] = state;
//         }
//     }
//     return snapshot;
// }

// void NetworkServer::BroadCastGameState(const IGameState::GameSnap& snapshot)
// {
//     // L'implémentation utilise maintenant le GameSnap
//     std::vector<uint8_t> data = serializeGameState(snapshot);

//     ENetPacket* packet = enet_packet_create(
//         data.data(),
//         data.size(),
//         ENET_PACKET_FLAG_RELIABLE
//     );

//     enet_host_broadcast(server, 0, packet);
// }


// void ServerGameState::syncControlledEntities()
// {
//     auto& velocities = _registry.getComponents<Velocity>();
//     auto& controllables = _registry.getComponents<Controllable>();
//     /** @todo Jackma */
// }

// void ServerGameState::validateMovement(std::optional<Velocity>& vel)
// {
//     /** @todo Jackma */
// }

// ServerGameState::~ServerGameState()
// {
// }
