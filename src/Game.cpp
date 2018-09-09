//
// Copyright (C) 2018 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Modified by Filmote and Vampirics
//

#include "Game.h"

#include "states/States.h"
#include "utils/Utils.h"


void Game::setup(void) {

	auto & arduboy = this->context.arduboy;

	this->currentStateId = GameStateType::SplashScreen;
	this->currentState = createState(this->currentStateId);
	this->nextStateId = this->currentStateId;

  arduboy.boot();
  arduboy.flashlight();
  arduboy.systemButtons();
  arduboy.audio.begin();
  arduboy.initRandomSeed();
  arduboy.setFrameRate(60);

	this->currentState->activate(*this);

}

void Game::loop(void) {

	auto &arduboy = this->context.arduboy;
	if (!arduboy.nextFrame()) return;

	arduboy.pollButtons();

	if (this->currentState != nullptr) this->currentState->update(*this);
		
	arduboy.clear();

	if (this->currentState != nullptr) this->currentState->render(*this);

	arduboy.display();
	
	while (this->changePending) {

		this->currentState->deactivate(*this);

		if (this->currentStateId != this->nextStateId) {
			// Strictly speaking this does nothing, but I'm keeping it for the sake of correctness
			this->currentState->~GameState();
			this->currentState = this->createState(this->nextStateId);
			this->currentStateId = this->nextStateId;
		}

		this->changePending = false;
  	this->context.gameState = this->currentStateId;
		this->currentState->activate(*this);

	}

}

Game::Context & Game::getContext(void) {
	return this->context;
}

const Game::Context & Game::getContext(void) const {
	return this->context;
}

void Game::changeState(const StateId & stateId) {
	this->nextStateId = stateId;
	this->changePending = true;
}

Game::State * Game::createState(const StateId & stateType) {

	switch (stateType) {

		case GameStateType::Event: 		      					return new (&this->stateData[0])  EventState();
		case GameStateType::ShowCards:       					return new (&this->stateData[0])  ShowCardsState();
		case GameStateType::Resting:                  return new (&this->stateData[0])  RestingState();
		case GameStateType::TitleScreen:              return new (&this->stateData[0])  TitleScreenState();
		case GameStateType::Trap:              				return new (&this->stateData[0])  TrapState();
		case GameStateType::Treasure:      		      	return new (&this->stateData[0])  TreasureState();
		case GameStateType::Merchant:      		      	return new (&this->stateData[0])  MerchantState();
		case GameStateType::SplashScreen:             return new (&this->stateData[0])  SplashScreenState();
		case GameStateType::Winner: 			            return new (&this->stateData[0])  GameOverState();
		case GameStateType::PlayerDead: 	            return new (&this->stateData[0])  GameOverState();

		case GameStateType::Monster:   								
		case GameStateType::BossMonster:
		case GameStateType::MonsterFromEvent: 				return new (&this->stateData[0])  FightMonstersState();

		default: return nullptr;

	}

}