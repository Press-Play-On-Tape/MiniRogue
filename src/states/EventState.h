#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"


char const eventDice_Caption_01[] PROGMEM = "Found~Loot   ~+2~Gold";
char const eventDice_Caption_02[] PROGMEM = "Health~Potion   +2~HP";
char const eventDice_Caption_03[] PROGMEM = "Found~Meat    +1~Food";
char const eventDice_Caption_04[] PROGMEM = "Whetstone       +2~XP";
char const eventDice_Caption_05[] PROGMEM = "Found~Shield    +1~AR";
char const eventDice_Caption_06[] PROGMEM = "Monster         5~DMG";

char const * const eventDice_Captions[] = {
	eventDice_Caption_01,
	eventDice_Caption_02,
	eventDice_Caption_03,
	eventDice_Caption_04,
	eventDice_Caption_05,
	eventDice_Caption_06,
};

class EventState : public BaseState {
  
  enum class ViewState : uint8_t {
    RollDice,
    UpdateStats,
    PlayerDead
  };

  private:

    void printEventName();

    ViewState viewState = ViewState::RollDice;
    
    uint8_t counter = 0;
    uint8_t dice = 0;


  public:

    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};