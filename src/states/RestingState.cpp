#include "RestingState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void RestingState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::SelectReward:

      if ((justPressed & LEFT_BUTTON) && this->selectedItem == SelectedItem::Weapon)     { this->selectedItem = this->prevSelection; }
      if ((justPressed & RIGHT_BUTTON) && this->selectedItem != SelectedItem::Weapon)    { this->prevSelection = this->selectedItem; this->selectedItem = SelectedItem::Weapon; }
      if ((justPressed & DOWN_BUTTON) && this->selectedItem == SelectedItem::Food)       { this->selectedItem = SelectedItem::Heal; }
      if ((justPressed & UP_BUTTON) && this->selectedItem == SelectedItem::Heal)         { this->selectedItem = SelectedItem::Food; }

      if (justPressed & A_BUTTON)   { 

        switch (this->selectedItem) {

          case SelectedItem::Weapon:
            playerStats.incXP(1);
            break;

          case SelectedItem::Food:
            playerStats.food++;
            break;

          case SelectedItem::Heal:
            playerStats.hp = playerStats.hp + 2;
            break;

        }

        this->viewState = ViewState::UpdateStats;

      }

      break;

    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

        if (counter == FLASH_COUNTER) {

          machine.changeState(gameStats.incRoom(playerStats)); 

        }

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
void RestingState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);

  BaseState::renderBackground(machine, false);
  // ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  // ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(0, 0, Images::Card_Resting_Large_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    (this->viewState == ViewState::UpdateStats), // Overall
    (this->selectedItem == SelectedItem::Weapon), // XP
    (this->selectedItem == SelectedItem::Heal), // HP
    false, // Armour
    false, // Gold
    (this->selectedItem == SelectedItem::Food) // Food
  );

  if (flash) {

    uint8_t a = 0;
    uint8_t b = 48;
    uint8_t c = 0;
    uint8_t d = 34;
    
    switch (this->selectedItem) {

      case SelectedItem::Food:
        // a = 0;
        // b = 48;
        // c = 0;
        // d = 34;
        break;

      case SelectedItem::Heal:
        // a = 0;
        // b = 48;
        c = 36;
        d = 63;
        break;

      case SelectedItem::Weapon:
        a = 50;
        b = 87;
        // c = 0;
        d = 63;
        break;

    }

    arduboy.drawHorizontalDottedLine(a, b, c, BLACK);
    arduboy.drawHorizontalDottedLine(a, b, d, BLACK);
    arduboy.drawVerticalDottedLine(c, d, a, BLACK);
    arduboy.drawVerticalDottedLine(c, d, b, BLACK);

  }

}
