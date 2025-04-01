

// #ifndef SERVERGAMESTATE_HPP_
// #define SERVERGAMESTATE_HPP_


// #pragma once
// #include "../../shared/include/IGameState.hpp"

// class ServerGameState : public IGameState
// {
//     public:
//         explicit ServerGameState(ECS::Registry& registry);

//         /** @brief: Implem of specific to server :) */
//         GameSnap captureState() override;
//         void applyState(const GameSnap& snapshot) override;

//         /** @brief: Implem of methods specific to server */
//         void broadcastStateToClients();
//         void syncControlledEntities();

//     protected:
//     private:
//         ECS::Registry& _registry;
//         void validateMovement(std::optional<Velocity>& vel);
// };

// #endif /* !SERVERGAMESTATE_HPP_ */
