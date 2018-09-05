#pragma once

#include "../utils/GameContext.h"
#include "../utils/GameState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"

class BaseState : public GameState<GameContext, GameStateType> {

  protected:

    Font3x5 font3x5 = Font3x5();

  public:

    void renderPlayerDead(StateMachine & machine);
    void renderMonsterDead(StateMachine & machine);
    void renderPlayerStatistics(StateMachine & machine, bool overallFlash, bool flashXP, bool flashHP, bool flashArmour, bool flashGold, bool flashFood, bool showCardsView = false);
    void drawItem(uint8_t position, uint8_t const *imageName, bool showCardsView);
};