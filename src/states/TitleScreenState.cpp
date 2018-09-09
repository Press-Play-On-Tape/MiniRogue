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
		
	}

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TitleScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & ardBitmap = machine.getContext().ardBitmap;

	ardBitmap.drawCompressed(0, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(64, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);
	ardBitmap.drawCompressed(28, 17, Images::Title_Mini_Rogue_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(19, 56, Images::Title_Levels_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	{
		uint8_t x = 0;
		uint8_t w = 0;

		switch (gameStats.skillLevel) {

			case 0:
				x = 19;
				w = 15; 
				break;

			case 1:
				x = 42;
				w = 23; 
				break;

			case 2:
				x = 74;
				w = 14; 
				break;

			case 3:
				x = 96;
				w = 11; 
				break;

		}
				
		arduboy.drawFastHLine(x, 63, w);

	}

}

