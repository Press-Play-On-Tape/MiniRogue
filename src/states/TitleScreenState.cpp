#include "TitleScreenState.h"
#include "../images/Images.h"

constexpr const static uint8_t MIN_LEVEL = 0;
constexpr const static uint8_t MAX_LEVEL = 3;
constexpr const static uint8_t UPLOAD_DELAY = 16;


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TitleScreenState::update(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();
  auto pressed = arduboy.pressedButtons();

	// Restart ?

	if (pressed & DOWN_BUTTON) {

		if (restart < UPLOAD_DELAY) {
			restart++;
		}
		else {
			arduboy.exitToBootloader();
		}

	}
	else {
		restart = 0;
	}


	// Handle other input ..

	if ((justPressed & LEFT_BUTTON) && gameStats.skillLevel > MIN_LEVEL) 		--gameStats.skillLevel;
	if ((justPressed & RIGHT_BUTTON) && gameStats.skillLevel < MAX_LEVEL) 	++gameStats.skillLevel;

	if (justPressed & A_BUTTON) {

		constexpr const static uint8_t ELEMENTS_PER_ROW = 4;

		uint8_t idx = gameStats.skillLevel * ELEMENTS_PER_ROW;
		playerStats.xp = 0;
		playerStats.armour = pgm_read_byte(&InitSettings[idx++]);
		playerStats.hp = pgm_read_byte(&InitSettings[idx++]);
		playerStats.gold = pgm_read_byte(&InitSettings[idx++]);
		playerStats.food = pgm_read_byte(&InitSettings[idx]);
		playerStats.xpTrack = 1;

		machine.getContext().resetGame();
		machine.changeState(GameStateType::ShowCards);
	
		//SJH playerStats.hp = 20; 
		//SJH playerStats.items[0] = 1; 
		//SJH playerStats.items[1] = 1; 
		
	}

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TitleScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & ardBitmap = machine.getContext().ardBitmap;

	BaseState::renderTitleBackground(machine, true);
	ardBitmap.drawCompressed(28, 17, Images::Title_Mini_Rogue_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(19, 56, Images::Title_Levels_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	{

    static const uint8_t xPos[] PROGMEM = { 19, 42, 74, 96 };
    static const uint8_t width[] PROGMEM = { 15, 23, 14, 11 };
		arduboy.drawFastHLine(pgm_read_byte(&xPos[gameStats.skillLevel]), 63, pgm_read_byte(&width[gameStats.skillLevel]));

	}

}

