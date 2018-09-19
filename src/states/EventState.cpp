#include "EventState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

const static uint8_t NO_OF_CARDS_IN_FLIP = 13; 

const static uint8_t CARD_DRAW_ORDER[] = { 2, 1, 0, 2, 0, 1, 0, 1, 2 };

// ----------------------------------------------------------------------------
//  Initialise state ..
//
void EventState::activate(StateMachine & machine) {

  (void)machine;
  
  viewState = ViewState::RollDice;
  counter = 0;
  dice[0] = 0;
  dice[1] = 0;
  dice[2] = 0;
  skillCheck = 0;
  selection = 0;

}

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
      
			if (counter < NO_OF_CARDS_IN_FLIP - 1) {

				this->dice[1] = random(1, 7);
				counter++;

			}      
			else {

        this->viewState = ViewState::TakeOrKeep;
        this->counter = 40;

			}
			break;

    case ViewState::TakeOrKeep:

      if (this->counter < FLASH_COUNTER) {

        if (justPressed & A_BUTTON) { 

          viewState = ViewState::UpdateStats; 
          this->selection = 1; 
          counter = 0; 

        }
        else if (justPressed > 0) {

          counter = FLASH_COUNTER; 
          this->selection = (justPressed & RIGHT_BUTTON ? 1 : 0);

        }
        else {

          this->counter++;

        }

      }
      else {

        if (this->dice[1] != 6) { // You can not choose to take a skill test if you rolled a 6 (monster) ..

          if (justPressed & LEFT_BUTTON && this->selection == 1)  { this->selection = 0; } 
          if (justPressed & RIGHT_BUTTON && this->selection == 0) { this->selection = 1; } 
        
        }

        if (justPressed & A_BUTTON) { 
          
          counter = 0;

          if (this->selection == 0) {

            this->selection = 1;
            this->viewState = ViewState::UpdateStats;

          }
          else {

            this->viewState = ViewState::SkillCheck;

          }

        }

      }

      break;

    case ViewState::SkillCheck:
      
			if (counter < NO_OF_CARDS_IN_FLIP - 1) {

        this->skillCheck = random(1, 7);
        counter++;

			}
			else {
        
        this->counter = 0;
        arduboy.resetFrameCount();
        this->viewState = ViewState::SkillCheckResult;

			}
			break;

    case ViewState::SkillCheckResult:

      if (justPressed & A_BUTTON) { counter = FLASH_COUNTER; }

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
			else {

        if (justPressed & A_BUTTON) {

          if (this->skillCheck <= playerStats.xpTrack) {
            
            this->counter = 0;
            this->selection = 1;

            this->dice[0] = this->dice[1] - 1;  if (this->dice[0] < 1) this->dice[0] = 6;
            this->dice[2] = this->dice[1] + 1;  if (this->dice[2] > 6) this->dice[2] = 1;
            
            this->viewState = ViewState::SelectCard;

          }
          else {

            machine.changeState(gameStats.incRoom(playerStats)); 

          }

        }

			}
			break;

    case ViewState::SelectCard:

      if (justPressed & LEFT_BUTTON && this->selection > 0)   { arduboy.resetFrameCount(); this->selection--; } 
      if (justPressed & RIGHT_BUTTON && this->selection < 2)  { arduboy.resetFrameCount(); this->selection++; } 

      if (justPressed & A_BUTTON)  { 

        this->counter = 0;
        this->viewState = ViewState::UpdateStats;

      } 
       
      break;


    case ViewState::UpdateStats:

      if (counter == 0) {

        switch (this->dice[this->selection]) {

          case 1: playerStats.incGold(2);     break;
          case 2: playerStats.incHP(2);       break;
          case 3: playerStats.incFood(1);     break;
          case 4: playerStats.incXP(2);       break;
          case 5: playerStats.incArmour(1);   break;
          case 6:                             break;

        }

      }

      if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER; justPressed = A_BUTTON; }

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
      else {

        if (justPressed & A_BUTTON) { 

          if (this->dice[this->selection] < 6) {
            
            machine.changeState(gameStats.incRoom(playerStats)); 

          }
          else {
            machine.changeState(GameStateType::MonsterFromEvent);
          }

        }

      }

      break;

		case ViewState::PlayerDead:

      if (justPressed & A_BUTTON) { 
        machine.changeState(GameStateType::PlayerDead);
			}

			break;

  }

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void EventState::render(StateMachine & machine) {

  auto & arduboy = machine.getContext().arduboy;
  auto & ardBitmap = machine.getContext().ardBitmap;
 	auto & playerStats = machine.getContext().playerStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  for (int8_t i = -12; i < 80; i = i + 20) {
    ardBitmap.drawCompressed(i, 40, Images::Event_Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

  BaseState::renderBackground(machine, true);
  
  switch (this->viewState) {

    case ViewState::RollDice:

      BaseState::renderLargeSpinningCard(machine, 28, 8, this->counter);
      
      if (counter < NO_OF_CARDS_IN_FLIP) {

        if (Images::Large_Spinning_Inlays[this->counter] > 0) {
          for (uint8_t i = 0, j = 0; i < Images::Large_Spinning_Inlays[this->counter]; i++, j = j + 2) {
            ardBitmap.drawCompressed(32 + (this->counter * 2) + j, 8, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);
          }
        }

      }
      else {

        ardBitmap.drawCompressed(30, 10, Images::Event_Dice[this->dice[1] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

      }

      break;

    case ViewState::TakeOrKeep:

      renderLargeSpinningCard(machine, 28, 8, this->dice[1]);
      font3x5.setCursor(4, 0);

      if (counter < FLASH_COUNTER || this->dice[1] == 6) {

        printEventName(this->dice[1]);

      }
      else {

        font3x5.print(" Take~skill~test? ");
        arduboy.fillRect(71 + (this->selection == 0 ? 0 : 6), 0, 5, 7, WHITE);

        if (this->selection == 0) { font3x5.setTextColor(BLACK); }
        font3x5.print("N~");
        font3x5.setTextColor(WHITE);

        if (this->selection == 1) { font3x5.setTextColor(BLACK); }
        font3x5.print("Y");
        font3x5.setTextColor(WHITE);

      }

      break;

    case ViewState::SkillCheck:
    case ViewState::SkillCheckResult:

      font3x5.setCursor(3, 3);
      font3x5.print(F("Select~card?"));
      font3x5.setCursor(66, 3);
      SpritesB::drawOverwrite(51, 2, Images::Dice, this->skillCheck);

      if (viewState == ViewState::SkillCheckResult) {

        if (this->counter < FLASH_COUNTER && flash) font3x5.setTextColor(BLACK);

        if (this->skillCheck <= playerStats.xpTrack) {

          if (this->counter < FLASH_COUNTER && flash) arduboy.fillRect(64, 3, 15, 7, WHITE);
          font3x5.print(F("Yes"));

        }
        else {

          if (this->counter < FLASH_COUNTER && flash) arduboy.fillRect(64, 3, 11, 7, WHITE);
          font3x5.print(F("No"));

        }

        font3x5.setTextColor(WHITE);

      }

      break;

    case ViewState::SelectCard:

      for (uint8_t i = 0; i < 3; i++) {

        uint8_t renderIdx = CARD_DRAW_ORDER[(this->selection * 3) + i];

        switch (renderIdx) {

          case 0:
            renderLargeSpinningCard(machine, 2, 15, this->dice[0]);
            if (flash && this->selection == 0) ardBitmap.drawCompressed(2, 15, Images::Large_Spinning_Card_Highlight, BLACK, ALIGN_NONE, MIRROR_NONE);
            break;

          case 1:
            renderLargeSpinningCard(machine, 28, 8, this->dice[1]);
            if (flash && this->selection == 1) ardBitmap.drawCompressed(28, 8, Images::Large_Spinning_Card_Highlight, BLACK, ALIGN_NONE, MIRROR_NONE);
            break;

          case 2:
            renderLargeSpinningCard(machine, 54, 15, this->dice[2]);
            if (flash && this->selection == 2) ardBitmap.drawCompressed(54, 15, Images::Large_Spinning_Card_Highlight, BLACK, ALIGN_NONE, MIRROR_NONE);
            break;

        }

      }
      // BaseState::renderLargeSpinningCard(machine, 2, 11, 0);
      // BaseState::renderLargeSpinningCard(machine, 54, 11, 0);
      // BaseState::renderLargeSpinningCard(machine, 28, 8, 0);
      // ardBitmap.drawCompressed(4, 13, Images::Event_Dice[this->dice[0] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      // ardBitmap.drawCompressed(56, 13, Images::Event_Dice[this->dice[2] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      // ardBitmap.drawCompressed(30, 10, Images::Event_Dice[this->dice[1] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      font3x5.setCursor(4, 0);
      printEventName(this->dice[this->selection]);

      break;


    case ViewState::UpdateStats:

      renderLargeSpinningCard(machine, 28, 8, this->dice[this->selection]);
      // BaseState::renderLargeSpinningCard(machine, 28, 8, 0);
      // ardBitmap.drawCompressed(30, 10, Images::Event_Dice[this->dice[1] - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      font3x5.setCursor(4, 0);
      printEventName(this->dice[this->selection]);

      break;

    case ViewState::PlayerDead:

      BaseState::renderPlayerDead();
      break;

  }

	// Player statistics ..

	static const FlashSettings diceHelper[] PROGMEM =
	{
		FlashSettings::None,
		FlashSettings::FlashGold,
		FlashSettings::FlashHP,
		FlashSettings::FlashFood,
		FlashSettings::FlashXP,
	};

	const FlashSettings settings = (this->dice[this->selection] < 5) ? static_cast<FlashSettings>(pgm_read_byte(&diceHelper[this->dice[this->selection]])) : FlashSettings::None;
	const bool shouldFlash = (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER);

	BaseState::renderPlayerStatistics(machine, shouldFlash, settings);

}

void EventState::printEventName(uint8_t dice) {

  font3x5.print(FlashString(eventDice_Captions[dice - 1]));

}

void EventState::renderLargeSpinningCard(StateMachine & machine, uint8_t x, uint8_t y, uint8_t dice) {

  auto & ardBitmap = machine.getContext().ardBitmap;
  auto & arduboy = machine.getContext().arduboy;

  arduboy.fillRect(x, y, 32, 32, BLACK);
  BaseState::renderLargeSpinningCard(machine, x, y, 0);
  ardBitmap.drawCompressed(x + 2, y + 2, Images::Event_Dice[dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

}