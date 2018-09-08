#pragma once

// #include "../utils/GameContext.h"
// #include "../utils/GameState.h"
#include "BaseState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"


char const treasureDice_Caption_01[] PROGMEM = " Gain~a~Fire~Wand!";
char const treasureDice_Caption_02[] PROGMEM = " Gain~an~Ice~Wand!";
char const treasureDice_Caption_03[] PROGMEM = "Gain~a~Poison~Wand!";
char const treasureDice_Caption_04[] PROGMEM = "Heal~Potion~+~5HP!";
char const treasureDice_Caption_05[] PROGMEM = "  Gain~an~Armour!";
char const treasureDice_Caption_06[] PROGMEM = "Upgrade~your~Weapon!";
char const treasureDice_Caption_07[] PROGMEM = "Open~chest:\nGold~and~treasure!";
char const treasureDice_Caption_08[] PROGMEM = "Open~chest:\nYou~found~gold~only!";

char const * const treasureDice_Captions[] = {
	treasureDice_Caption_01,
	treasureDice_Caption_02,
	treasureDice_Caption_03,
	treasureDice_Caption_04,
	treasureDice_Caption_05,
	treasureDice_Caption_06,
	treasureDice_Caption_07,
	treasureDice_Caption_08,
}; 

class TreasureState : public BaseState {
  
  enum class ViewState : uint8_t {
    InitialRoll,
    RollDice,
    UpdateStats
  };

  private:

    void printCaption(uint8_t index);
    void renderSelectTreasure(StateMachine & machine);
    void renderChestResults(StateMachine & machine);

    ViewState viewState = ViewState::InitialRoll;
    
    uint8_t counter = 0;
    uint8_t dice = 0;
    bool foundTreasure = false;


  public:

    void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};