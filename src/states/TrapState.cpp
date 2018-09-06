#include "TrapState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

constexpr const static uint8_t NO_OF_CARDS_IN_FLIP = 13; 


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void TrapState::update(StateMachine & machine) { 

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

  switch (this->viewState) {

    case ViewState::SkillCheckResult:

      #ifdef PRESS_A_TO_BYPASS
        if (justPressed & A_BUTTON) { counter = FLASH_COUNTER; }
      #endif

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
			else {

        if (justPressed & A_BUTTON) {

          if (this->dice <= playerStats.armour) {

            gameStats.incRoom(playerStats);
        		machine.changeState(GameStateType::ShowCards); 

          }
          else {

            this->counter = 0;
            this->viewState = ViewState::RollDice;

          }

        }

			}
			break;

    case ViewState::SkillCheck:
    case ViewState::RollDice:

      #ifdef PRESS_A_TO_BYPASS
      if (justPressed & A_BUTTON) { counter = sizeof(DiceDelay); }
      #endif
      
			if (counter < NO_OF_CARDS_IN_FLIP) {

        this->dice = random(1, 7);
        counter++;

			}
			else {

        if (this->viewState == ViewState::RollDice) {

          switch (this->dice) {

            case 1: if (playerStats.food > 0)     playerStats.food--;     break;
            case 2: if (playerStats.gold > 0)     playerStats.gold--;     break;
            case 3: if (playerStats.armour > 0)   playerStats.armour--;   break;
            case 4: if (playerStats.hp > 1)       playerStats.hp--;       break;
            case 5: if (playerStats.xp > 1)       playerStats.xp--;       break;

            case 6: 
              
              if (playerStats.hp > 3) {
                playerStats.hp = playerStats.hp - 3; 
                gameStats.dropArea();
              }
              else {
                playerStats.hp = 0;
              }

            break;

          }


          // Have we died?

          if (playerStats.hp > 0) {

            arduboy.resetFrameCount();
            this->counter = 0;
            this->viewState = ViewState::UpdateStats;

          }
          else {

            viewState = ViewState::PlayerDead;

          }

        }
        else {
          
          this->counter = 0;
          arduboy.resetFrameCount();
          this->viewState = ViewState::SkillCheckResult;

        }

			}
			break;

    case ViewState::UpdateStats:

      if (this->counter < FLASH_COUNTER) {

        this->counter++;

      }
      else {

        if (justPressed & A_BUTTON) {
          gameStats.incRoom(playerStats);
      		machine.changeState(GameStateType::ShowCards); 
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
void TrapState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
  auto & ardBitmap = machine.getContext().ardBitmap;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  // Render common parts ..

  ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(0, 40, Images::Trap_LHS_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(79, 40, Images::Trap_RHS_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

  for (uint8_t i = 9; i < 70; i = i + 14) {
    ardBitmap.drawCompressed(i, 40, Images::Trap_Single_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  }

  switch (this->viewState) {

    case ViewState::SkillCheck:
    case ViewState::SkillCheckResult:

      font3x5.setCursor(3, 3);
      font3x5.print(F("Evade~trap?"));
      font3x5.setCursor(66, 3);
      SpritesB::drawOverwrite(51, 2, Images::Dice, this->dice);

      if (viewState == ViewState::SkillCheckResult) {

        if (this->counter < FLASH_COUNTER && flash) font3x5.setTextColor(BLACK);

        if (this->dice <= playerStats.armour) {

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

    case ViewState::RollDice:

      ardBitmap.drawCompressed(28, 8, Images::Large_Spinning_Cards[this->counter], WHITE, ALIGN_NONE, MIRROR_NONE);

      if (counter < NO_OF_CARDS_IN_FLIP) {

        if (Images::Large_Spinning_Inlays[this->counter] > 0) {
          for (uint8_t i = 0, j = 0; i < Images::Large_Spinning_Inlays[this->counter]; i++, j = j + 2) {
            ardBitmap.drawCompressed(32 + (this->counter * 2) + j, 8, Images::Large_Spinning_Card_Inlay, WHITE, ALIGN_NONE, MIRROR_NONE);
          }
        }

      }
      else {

        ardBitmap.drawCompressed(30, 10, Images::Trap_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

      }

      break;

    case ViewState::UpdateStats:

      ardBitmap.drawCompressed(28, 8, Images::Large_Spinning_Cards[12], WHITE, ALIGN_NONE, MIRROR_NONE);
      ardBitmap.drawCompressed(30, 10, Images::Trap_Dice[this->dice - 1], WHITE, ALIGN_NONE, MIRROR_NONE);
      font3x5.setCursor(4, 0);
      printTrapName();

      break;

    case ViewState::PlayerDead:

      BaseState::renderPlayerDead(machine);
      break;

  }


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    (this->viewState == ViewState::UpdateStats && this->counter < FLASH_COUNTER), // Overall
    (this->dice == 5), // XP
    (this->dice == 4), // HP
    (this->dice == 3), // Armour
    (this->dice == 2), // Gold
    (this->dice == 1) // Food
  );

}

char const trapDice_Caption_01[] PROGMEM = "Mildew       ~-1~Food";
char const trapDice_Caption_02[] PROGMEM = "Tripwire     ~-1~Gold";
char const trapDice_Caption_03[] PROGMEM = "Rusted~Armour   -1~AR";
char const trapDice_Caption_04[] PROGMEM = "Spring~Blade    -1~HP";
char const trapDice_Caption_05[] PROGMEM = "Moving~Walls    -1~XP";
char const trapDice_Caption_06[] PROGMEM = "Pit     ~-3~HP,~+Fall";

char const * const trapDice_Captions[] = {
	trapDice_Caption_01,
	trapDice_Caption_02,
	trapDice_Caption_03,
	trapDice_Caption_04,
	trapDice_Caption_05,
	trapDice_Caption_06,
};

void TrapState::printTrapName() {

  font3x5.print(FlashString(trapDice_Captions[this->dice - 1]));

}
