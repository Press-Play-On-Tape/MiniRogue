#include "MerchantState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void MerchantState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  if (justPressed > 0) this->errorNumber = 0;

  switch (this->viewState) {

    case ViewState::Buying:

      if ((justPressed & UP_BUTTON) && this->selectedItem > 0)            this->selectedItem--;
      if ((justPressed & DOWN_BUTTON) && this->selectedItem < 5)          this->selectedItem++;

      if (justPressed & A_BUTTON) {

        switch (this->selectedItem) {

          case 0:
            
            if (playerStats.gold == 0) {
              this->errorNumber = 1;
            } 
            else if (playerStats.food == 10) { 
              this->errorNumber = 3;
            } 
            else {
              this->flashFood = true; 
              playerStats.food++; 
              playerStats.gold--; 
            }   
            
            break;

          case 1:

            if (playerStats.gold == 0) {
              this->errorNumber = 1;
            } 
            else if (playerStats.hp == 20) { 
              this->errorNumber = 3;
            } 
            else { 
              this->flashHP = true; 
              playerStats.hp++; 
              playerStats.gold--; 
            }   
            
            break;

          case 2:

            if (playerStats.gold < 3) {
              this->errorNumber = 1;
            } 
            else if (playerStats.hp == 20) { 
              this->errorNumber = 3;
            } 
            else { 
              this->flashHP = true; 
              playerStats.hp = clamp(playerStats.hp + 4, 0, 20); 
              playerStats.gold = playerStats.gold - 3; 
            }   
            
            break;

          case 3 ... 5:

            if (playerStats.gold >= 8) {

              if (playerStats.itemCount() < 2) { 

                playerStats.items[this->selectedItem - 3]++; 
                playerStats.gold = playerStats.gold - 8; 
                flashGold = true;

              }
              else {
                this->errorNumber = 3;
              }

            }
            else {
              this->errorNumber = 1;
            }   
            
           break;

        }

        if (flashHP || flashXP || flashFood) { flashGold = true; }

      }

			break;

    case ViewState::Selling:

      if ((justPressed & UP_BUTTON) && this->selectedItem > 0)            this->selectedItem--;
      if ((justPressed & DOWN_BUTTON) && this->selectedItem < 3)          this->selectedItem++;
      
      if (justPressed & A_BUTTON) {

        if (playerStats.items[this->selectedItem] > 0) {

          playerStats.items[this->selectedItem]--;
          playerStats.gold = playerStats.gold + 4;
          flashGold = true;

        }
        else {
          this->errorNumber = 2;
        }

      } 

      break;

  }

  if ((justPressed & LEFT_BUTTON) || (justPressed & RIGHT_BUTTON)) {
    
    this->viewState = static_cast<ViewState>(!static_cast<bool>(this->viewState));
    this->selectedItem = 0;

  }

  if (justPressed & B_BUTTON) {

    machine.changeState(gameStats.incRoom(playerStats)); 

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void MerchantState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(39, 0, Images::Merchant_Only_Mask_Comp, BLACK, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(39, 0, Images::Merchant_Only_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  
  arduboy.drawFastHLine(2, 60, 85, WHITE);
  arduboy.drawHorizontalDottedLine(3, 87, 62, WHITE);
  arduboy.drawHorizontalDottedLine(4, 87, 63, WHITE);

  {
    
    uint8_t const *imageName = nullptr;

    switch (this->viewState) {

      case ViewState::Buying:
        imageName = Images::Merchant_Buy_Comp;
        break;

      case ViewState::Selling:
        imageName = Images::Merchant_Sell_Comp;
        break;

    }

    ardBitmap.drawCompressed(2, 1, imageName, WHITE, ALIGN_NONE, MIRROR_NONE);

  }


  // Highlight ..

  Sprites::drawOverwrite(2, 8 + (selectedItem * 8), Images::Merchant_Highlight, 0);
  Sprites::drawOverwrite(33, 8 + (selectedItem * 8), Images::Merchant_Highlight, 0);


  // Error Message ?

  if (this->errorNumber > 0) {

    arduboy.fillRect(31, 23, 64, 26, BLACK);
    arduboy.drawFastHLine(34, 25, 58);
    arduboy.drawFastHLine(34, 46, 58);
    arduboy.drawFastVLine(33, 26, 20);
    arduboy.drawFastVLine(92, 26, 20);
    font3x5.setCursor(36, 28);
  	font3x5.print(FlashString(error_Captions[ this->errorNumber - 1]));

  }

  BaseState::renderPlayerStatistics(machine,
    true, // Overall
    flashXP, // XP
    flashHP, // HP
    false, // Armour
    flashGold, // Gold
    flashFood // Food
  );

}
