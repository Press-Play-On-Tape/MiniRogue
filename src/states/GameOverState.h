#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "BaseState.h"

class GameOverState : public BaseState {

  private:

  public:
    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};