#include "GameOverState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
#include "../utils/FadeEffects.h"


#define EEPROM_START                  EEPROM_STORAGE_SPACE_START + 130
#define EEPROM_START_C1               EEPROM_START
#define EEPROM_START_C2               EEPROM_START + 1
#define EEPROM_SCORE                  EEPROM_START + 2



// ----------------------------------------------------------------------------
//  Initialise state ..
//
void GameOverState::activate(StateMachine & machine) {
	
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	switch (machine.getContext().gameState) {

		case GameStateType::PlayerDead:
			viewState = ViewState::PlayerDead; 
			break;

		case GameStateType::Winner:
			viewState = ViewState::Winner; 
			break;

		default: break;

	}

	this->score = 0;
	this->score += (gameStats.skillLevel * 3);
	this->score += (playerStats.xpTrack * 2);
	this->score += ((gameStats.level + 1) * 3);
	this->score += (playerStats.hp * 2);
	this->score += (playerStats.food);
	this->score += (playerStats.armour);
	this->score += (playerStats.gold * 2);
	this->score += (playerStats.bossesKilled * 2);
	this->score += (playerStats.itemCount());

	this->highScore = EEPROM.read(EEPROM_SCORE);

	if (this->score > this->highScore) {

		this->highScore = this->score;
		EEPROM.update(EEPROM_SCORE, this->score);

	}

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void GameOverState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto justPressed = arduboy.justPressedButtons();

  if (justPressed & A_BUTTON) { 
		
		switch (this->viewState) {

			case ViewState::PlayerDead:
			case ViewState::Winner: 
				viewState = ViewState::HighScore; 
				break;

			case ViewState::HighScore:
				machine.changeState(GameStateType::TitleScreen); 
				break;

		}

	}

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void GameOverState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & ardBitmap = machine.getContext().ardBitmap;
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	BaseState::renderTitleBackground(machine, (viewState != ViewState::HighScore));

	switch (this->viewState) {

		case ViewState::PlayerDead:
			ardBitmap.drawCompressed(29, 21, Images::Title_Game_Over_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
			break;

		case ViewState::Winner:
			ardBitmap.drawCompressed(24, 15, Images::Winner_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
			break;

		case ViewState::HighScore:
			{
				ardBitmap.drawCompressed(20, 21, Images::High_Score_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

				font3x5.setCursor(23, 20);
				font3x5.print(FlashString(level_Captions[ static_cast<uint8_t>(gameStats.skillLevel) ]));
				font3x5.setCursor(56, 20);
				renderTwoDigitNumeric(gameStats.skillLevel * 3);

				font3x5.setCursor(33, 29);
				renderTwoDigitNumeric(playerStats.xpTrack * 2);
				renderTwoDigitNumeric((gameStats.level + 1) * 3);
				renderTwoDigitNumeric(playerStats.hp * 2);
				renderTwoDigitNumeric(playerStats.food);

				font3x5.setCursor(56, 29);
				renderTwoDigitNumeric(playerStats.armour);
				renderTwoDigitNumeric(playerStats.gold * 2);
				renderTwoDigitNumeric(playerStats.bossesKilled * 2);
				renderTwoDigitNumeric(playerStats.itemCount());

				font3x5.setCursor(95, 20);
				renderThreeDigitNumeric(this->score);
				font3x5.setCursor(95, 29);
				renderThreeDigitNumeric(this->highScore);

			}
			break;


	}


}

void GameOverState::renderTwoDigitNumeric(uint8_t val) { 

	if (val < 10) font3x5.print(F("0"));
	font3x5.print(val);
	font3x5.print(F("\n"));

}

void GameOverState::renderThreeDigitNumeric(uint8_t val) { 

	if (val < 100) font3x5.print(F("0"));
	if (val < 10) font3x5.print(F("0"));

	font3x5.print(val);
	font3x5.print(F("\n"));

}


/* ----------------------------------------------------------------------------
 *   Is the EEPROM initialised?
 *
 *   Looks for the characters 'A' and 'R' in the first two bytes of the EEPROM
 *   memory range starting from byte EEPROM_STORAGE_SPACE_START.  If not found,
 *   it resets the settings ..
 */

const uint8_t letter1 = 'M'; 
const uint8_t letter2 = 'R'; 

void GameOverState::initEEPROM(bool forceClear) {

  byte c1 = EEPROM.read(EEPROM_START_C1);
  byte c2 = EEPROM.read(EEPROM_START_C2);

  if (forceClear || c1 != letter1 || c2 != letter2) { 

    uint8_t score = 0;

    EEPROM.update(EEPROM_START_C1, letter1);
    EEPROM.update(EEPROM_START_C2, letter2);
    EEPROM.put(EEPROM_SCORE, score);

  }

}