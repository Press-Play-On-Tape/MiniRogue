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

  if (justPressed > 0) {

    this->counter = 1;

    TCCR3A = _BV(COM3A0); // set toggle on compare mode (which connects the pin)
    OCR3A = 7812; // 128 Hz
  }

  if (this->counter > 0) {

    this->counter++;

    if      (counter == 13)  { // 256 Hz
      OCR3A = 3905;
    }
    else if (counter == 26)  { // 512 Hz
      OCR3A = 1952;
    }
    else if (counter == 39)  { // 1000 Hz
      OCR3A = 999;
    }
    else if (counter == 55)  { // 2000 Hz
      OCR3A = 499;
    }
    else if (counter == 71)  { // 4000 Hz
      OCR3A = 249;
    }
    else if (counter == 87)  { // 8820 Hz
      OCR3A = 112;
    }
    else if (counter == 103) { // 11025 Hz
      OCR3A = 90;
    }
    else if (counter == 120) {

      TCCR3A = 0; // set normal mode (which disconnects the pin)

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
    ardBitmap.drawCompressed(44, 46, Images::Ppot_Loading, WHITE, ALIGN_NONE, MIRROR_NONE);

    uint8_t i = (this->counter / 15) % 4;

    for (uint8_t j = 0; j < i; j++) {
      
        arduboy.drawPixel(79 + (j * 2), 49);

    }

  }

}
