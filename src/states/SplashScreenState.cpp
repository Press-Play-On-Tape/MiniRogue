#include "SplashScreenState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
#include "../utils/FadeEffects.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void SplashScreenState::activate(StateMachine & machine) {

  (void)machine;
  fadeOutEffect.reset(0, HEIGHT, 1); 

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void SplashScreenState::update(StateMachine & machine) { 

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void SplashScreenState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & ardBitmap = machine.getContext().ardBitmap;

  ardBitmap.drawCompressed(30, 14, Images::BootLogo, WHITE, ALIGN_NONE, MIRROR_NONE);

  if (!fadeOutEffect.isComplete()) {

    fadeOutEffect.draw(arduboy);
    fadeOutEffect.update();

  }

  if (fadeOutEffect.isComplete()) machine.changeState(GameStateType::TitleScreen);

}