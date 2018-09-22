#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"

class SplashScreenState : public GameState<GameContext, GameStateType> {

  private:
    FadeOutEffect fadeOutEffect;

  public:
    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};