#include "TreasureState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void TreasureState::activate(StateMachine & machine) { 

  (void)machine;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TreasureState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::InitialRoll:
    case ViewState::RollDice:

      #ifdef PRESS_A_TO_BYPASS
      if (justPressed & A_BUTTON) { counter = sizeof(DiceDelay); }
      #endif
      
			if (this->counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[this->counter]))) {

          if (playerStats.itemCount() == 2) {
  					this->dice = random(4, 7);
          }
          else {
  					this->dice = random(1, 7);
          }

					this->counter++;
					arduboy.resetFrameCount();

				}

			}
			else {

        if (this->viewState == ViewState::RollDice) {
            
          this->foundTreasure = true;

          switch (this->dice) {

            case 1: playerStats.items[static_cast<uint8_t>(Wand::Fire)]++; break;
            case 2: playerStats.items[static_cast<uint8_t>(Wand::Ice)]++; break;
            case 3: playerStats.items[static_cast<uint8_t>(Wand::Poison)]++; break;
            case 4: playerStats.incHP(5); break;
            case 5: playerStats.incArmour(1); break;
            case 6: playerStats.incXP(1); break;

            break;

          }
            
          this->counter = 0;
          this->viewState = ViewState::UpdateStats;
          playerStats.incGold(gameStats.monsterDefeated ? 2 : 1);

        }
        else {

          // Initial Roll ..

          if (this->dice >= 5) {
            
            // if (arduboy.justPressed(A_BUTTON)) {
            if (justPressed & A_BUTTON) {
              this->counter = 0;
              this->viewState = ViewState::RollDice;
            }

          }
          else {

            playerStats.incGold(gameStats.monsterDefeated ? 2 : 1);
            this->counter = 0;
            this->viewState = ViewState::UpdateStats;   

          }

        }

			}
			break;

    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }

      if (justPressed & A_BUTTON) {

        machine.changeState(gameStats.incRoom(playerStats)); 

      }

      break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void TreasureState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;
  
  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(0, 0, Images::Chest_Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

  switch (this->viewState) {

    case ViewState::InitialRoll:

      if (this->counter < sizeof(DiceDelay)) {
       
        ardBitmap.drawCompressed(14, 16, Images::Chest_Closed_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
        SpritesB::drawSelfMasked(48, 1, Images::Dice, this->dice);
      
        font3x5.setCursor(3, 2);
        font3x5.print(F("Open~chest:"));

      }
      else {

        renderChestResults(machine);

      }

      break;

    case ViewState::RollDice:

      renderSelectTreasure(machine);
      break;

    case ViewState::UpdateStats:

      if (this->foundTreasure) {

        renderSelectTreasure(machine);

      }
      else {

        renderChestResults(machine);

      }

      break;

  }


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    (this->viewState == ViewState::UpdateStats && this->foundTreasure && this->counter < FLASH_COUNTER), // Overall
    (this->dice == 6), // XP
    (this->dice == 4), // HP
    (this->dice == 5), // Armour
    true, // Gold
    false // Food
  );

  if (this->viewState == ViewState::UpdateStats && this->foundTreasure && this->counter < FLASH_COUNTER && flash) {

    font3x5.setCursor(10, 0);
    printCaption(this->dice - 1); 

  }

}

void TreasureState::printCaption(uint8_t index) {

  font3x5.print(FlashString(treasureDice_Captions[index]));

}


void TreasureState::renderSelectTreasure(StateMachine & machine) {
	
  auto & ardBitmap = machine.getContext().ardBitmap;
  
  ardBitmap.drawCompressed(14, 8, Images::Chest_Open_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(35, 15, Images::Chest_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

}


void TreasureState::renderChestResults(StateMachine & machine) { 

  auto & ardBitmap = machine.getContext().ardBitmap;

  font3x5.setHeight(9);
  font3x5.setCursor(3, 2);
  SpritesB::drawOverwrite(48, 1, Images::Dice, this->dice);
  ardBitmap.drawCompressed(14, 16, Images::Chest_Closed_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  
  if (this->dice >= 5) {

    printCaption(6);

  }
  else {

    printCaption(7);

  }

  font3x5.setHeight(8);

}
