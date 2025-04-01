

#ifndef CLIENTGAMESTATE_HPP_
#define CLIENTGAMESTATE_HPP_


#include "../../shared/include/IGameState.hpp"
#include <ctime>
#include <iostream>

class ClientGameState : public IGameState {
    public:
        explicit ClientGameState(ECS::Registry& registry);

        /** @brief: Herited method are here */

        GameSnap captureState() override;
        void applyState(const GameSnap& snapshot) override;

        /** @brief: Method specific to the class are here
         * pls follow this function for code maintement*/

        // void predictNextState();
        void reconcileWithServer(const GameSnap& serverSnapshot);


    protected:

    private:
        ECS::Registry& _registry;
        GameSnap _lastConfirmedState;
        void interpolatePosition(ECS::Entity entity, const StateOfPlayer& serverSate);
};

#endif /* !CLIENTGAMESTATE_HPP_ */
