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

    void renderPlayerDead();
    void renderMonsterDead();
    void renderPlayerStatistics(StateMachine & machine, bool overallFlash, bool flashXP, bool flashHP, bool flashArmour, bool flashGold, bool flashFood);
    void renderBackground(StateMachine & machine, bool renderCorners);
    void drawItem(uint8_t position, uint8_t const *imageName);
    void renderMessageBox(StateMachine & machine, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

};