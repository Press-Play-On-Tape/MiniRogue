#include "SplashScreenState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void SplashScreenState::activate(StateMachine & machine) {

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void SplashScreenState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
  auto justPressed = arduboy.justPressedButtons();

	if (justPressed > 0) this->counter = 1;

  if (this->counter > 0) {

    this->counter++;

    if (counter == 30) {
    
      machine.changeState(GameStateType::TitleScreen);

    }

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void SplashScreenState::render(StateMachine & machine) {

	auto & ardBitmap = machine.getContext().ardBitmap;
	auto & arduboy = machine.getContext().arduboy;

  ardBitmap.drawCompressed(47, 17, Images::Ppot_Buttons, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(43, 26, Images::Ppot_ButtonUp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(73, 26, Images::Ppot_ButtonUp, WHITE, ALIGN_NONE, MIRROR_NONE);

  if (this->counter == 0) {

    ardBitmap.drawCompressed(58, 26, Images::Ppot_ButtonUp, WHITE, ALIGN_NONE, MIRROR_NONE);
    ardBitmap.drawCompressed(26, 46, Images::Ppot_Caption, WHITE, ALIGN_NONE, MIRROR_NONE);

  }
  else {

    ardBitmap.drawCompressed(58, 26, Images::Ppot_ButtonDown, WHITE, ALIGN_NONE, MIRROR_NONE);
    ardBitmap.drawCompressed(35, 46, Images::Ppot_Loading, WHITE, ALIGN_NONE, MIRROR_NONE);

    uint8_t i = (this->counter / 5 ) % 3;

    for (uint8_t j = 0; j < i + 1; j++) {
      
        arduboy.drawPixel(70 + (j * 2), 54);

    }

  }

}