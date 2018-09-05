#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../images/Images.h"

class TitleScreenState : public GameState<GameContext, GameStateType> {

  private:
    uint8_t restart = 0;
    
  public:	
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;
    
};

