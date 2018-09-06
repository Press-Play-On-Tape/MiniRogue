#include "EventState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void EventState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::RollDice:

      #ifdef PRESS_A_TO_BYPASS
      if (justPressed & A_BUTTON) { counter = sizeof(DiceDelay); }
      #endif
      
			if (counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[counter]))) {

					this->dice = random(1, 7);
					counter++;
					arduboy.resetFrameCount();

				}

			}
			else {

        switch (this->dice) {

          case 1: if (playerStats.gold < 20)    playerStats.gold++;     break;
          case 2: if (playerStats.hp < 20)      playerStats.hp++;       break;
          case 3: if (playerStats.food < 10)    playerStats.food++;     break;
          case 4:                               playerStats.incXP(1);   break;
          case 5: if (playerStats.armour < 5)   playerStats.armour++;   break;
          case 6:                                                       break;

        }

        this->viewState = ViewState::UpdateStats;
        this->counter = 0;

			}
			break;

    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
      else {

        if (justPressed & A_BUTTON) { 

          if (this->dice < 6) {
            
            gameStats.incRoom(playerStats);
            machine.changeState(GameStateType::ShowCards); 

          }
          else {
            machine.changeState(GameStateType::MonsterFromEvent);
          }

        }

      }

      break;

		case ViewState::PlayerDead:

      if (justPressed & A_BUTTON) { 
        machine.changeState(GameStateType::GameOver);
			}

			break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void EventState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
  auto & gameStats = machine.getContext().gameStats;
  auto & ardBitmap = machine.getContext().ardBitmap;
  

  // Render common parts ..

  for (int8_t i = -12; i < 80; i = i + 20) {
    ardBitmap.drawCompressed(i, 40, Images::Event_Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

  ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  
  switch (this->viewState) {

    case ViewState::RollDice:
    case ViewState::UpdateStats:

      ardBitmap.drawCompressed(28, 8, Images::Large_Spinning_Cards[12], WHITE, ALIGN_NONE, MIRROR_NONE);
      ardBitmap.drawCompressed(30, 10, Images::Event_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      if(this->viewState == ViewState::UpdateStats) {
        font3x5.setCursor(4, 0);
        printEventName();
      }

      break;

    case ViewState::PlayerDead:

      BaseState::renderPlayerDead(machine);
      break;

  }


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER), // Overall
    (this->dice == 4), // XP
    (this->dice == 2), // HP
    (this->dice == 5), // Armour
    (this->dice == 1), // Gold
    (this->dice == 3) // Food
  );

}

void EventState::printEventName() {

  font3x5.print(FlashString(eventDice_Captions[this->dice - 1]));

}