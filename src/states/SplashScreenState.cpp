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

  this->counter++;


  if (counter == 60) {
  
    machine.changeState(GameStateType::TitleScreen);

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void SplashScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & ardBitmap = machine.getContext().ardBitmap;

  ardBitmap.drawCompressed(25, 17, Images::Ppot_01, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(43, 26, Images::Ppot_02, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(73, 26, Images::Ppot_02, WHITE, ALIGN_NONE, MIRROR_NONE);

  if (this->counter < 30) {
    ardBitmap.drawCompressed(58, 26, Images::Ppot_02, WHITE, ALIGN_NONE, MIRROR_NONE);
  }
  else {
    ardBitmap.drawCompressed(58, 26, Images::Ppot_03, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

}