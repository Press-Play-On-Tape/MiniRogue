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
      if ((justPressed & DOWN_BUTTON) && this->selectedItem < 6)          this->selectedItem++;

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
              playerStats.incFood(1); 
              playerStats.decGold(1); 
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
              playerStats.incHP(1); 
              playerStats.decGold(1);
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
              playerStats.incHP(4); 
              playerStats.decGold(3); 
            }   
            
            break;

          case 3 ... 5:

            if (playerStats.gold >= 8) {

              if (playerStats.itemCount() < 2) { 

                playerStats.items[this->selectedItem - 3]++; 
                playerStats.decGold(8); 
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

          case 6:

            if (playerStats.gold >= 6) {

              if (playerStats.armour < 5) { 

                playerStats.incArmour(1); 
                playerStats.decGold(6); 
                flashArmour = true;

              }
              else {
                this->errorNumber = 3;
              }

            }
            else {
              this->errorNumber = 1;
            }   

        }

        if (flashHP || flashXP || flashFood || flashArmour) { flashGold = true; }

      }

			break;

    case ViewState::Selling:

      if ((justPressed & UP_BUTTON) && this->selectedItem > 0)            this->selectedItem--;
      if ((justPressed & DOWN_BUTTON) && this->selectedItem < 4)          this->selectedItem++;
      
      if (justPressed & A_BUTTON) {

        if (this->selectedItem <= 2) {

          if (playerStats.items[this->selectedItem] > 0) {

            playerStats.items[this->selectedItem]--;
            playerStats.incGold(4);
            flashGold = true;

          }
          else {
            this->errorNumber = 2;
          }

        }
        else {

          if (playerStats.armour > 0) {

            playerStats.decArmour(1);
            playerStats.incGold(3);
            flashGold = true;
            flashArmour = true;

          }
          else {
            this->errorNumber = 2;
          }

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


  // Render common parts ..

  BaseState::renderBackground(machine, true);
  ardBitmap.drawCompressed(39, 0, Images::Merchant_Only_Mask_Comp, BLACK, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(39, 0, Images::Merchant_Only_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  
  arduboy.drawFastHLine(35, 60, 51, WHITE);
  arduboy.drawHorizontalDottedLine(35, 87, 62, WHITE);
  arduboy.drawHorizontalDottedLine(36, 87, 63, WHITE);

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

  Sprites::drawOverwrite(2, 7 + (selectedItem * 8), Images::Merchant_Highlight, 0);
  Sprites::drawOverwrite(33, 7 + (selectedItem * 8), Images::Merchant_Highlight, 0);


  // Error Message ?

  if (this->errorNumber > 0) {

    BaseState::renderMessageBox(machine, 31, 23, 64, 26);
    font3x5.setCursor(36, 28);
  	font3x5.print(FlashString(error_Captions[ this->errorNumber - 1]));

  }

  BaseState::renderPlayerStatistics(machine,
    true, // Overall
    flashXP, // XP
    flashHP, // HP
    flashArmour, // Armour
    flashGold, // Gold
    flashFood // Food
  );

}
