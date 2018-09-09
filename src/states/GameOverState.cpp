#include "GameOverState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
#include "../utils/FadeEffects.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void GameOverState::activate(StateMachine & machine) {

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void GameOverState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto justPressed = arduboy.justPressedButtons();

  if (justPressed & A_BUTTON) { machine.changeState(GameStateType::TitleScreen); }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void GameOverState::render(StateMachine & machine) {

	auto & ardBitmap = machine.getContext().ardBitmap;

	ardBitmap.drawCompressed(0, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(64, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);

	if (machine.getContext().gameState == GameStateType::PlayerDead) {

		ardBitmap.drawCompressed(29, 21, Images::Title_Game_Over_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	}
	else {

		ardBitmap.drawCompressed(24, 15, Images::Winner_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	}

}