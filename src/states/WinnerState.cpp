#include "WinnerState.h"
#include "../utils/Arduboy2Ext.h"
#include "../images/Images.h"
#include "../utils/FadeEffects.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void WinnerState::activate(StateMachine & machine) {

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void WinnerState::update(StateMachine & machine) { 

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void WinnerState::render(StateMachine & machine) {

	auto & ardBitmap = machine.getContext().ardBitmap;

  ardBitmap.drawCompressed(0, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(0, 0, Images::Winner_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

}