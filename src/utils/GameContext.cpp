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

#include "GameContext.h"
#include "Utils.h"

GameContext::GameContext() {

  for (uint8_t i = 0; i < 6; i++) { 
    this->cards[i] = static_cast<GameStateType>(i + 1); 
  }

}

void GameContext::resetGame() {

  this->gameStats.resetGame();
  this->playerStats.resetGame();

}


