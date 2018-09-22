#pragma once

/*
  Copyright (C) 2018 Pharap (@Pharap)

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  Modified by Filmote and Vampirics
*/

#include "utils/Utils.h"
#include "utils/GameContext.h"
#include "utils/GameState.h"
#include "utils/GameStateMachine.h"
#include "states/States.h"

class Game : public GameStateMachine<GameContext, GameStateType> {

  private:

    EventState eventState;
    TreasureState treasureState;
    ShowCardsState showCardsState;
    FightMonstersState fightMonstersState;
    MerchantState merchantState;
    RestingState restingState;
    TitleScreenState titleScreenState;
    TrapState trapState;
    SplashScreenState splashScreenState;
    GameOverState gameOverState;
    
  public:

    void setup(void);
    void loop(void);

};