#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

struct MonsterStats {
  uint8_t hp;
  uint8_t dmg;
};

char const itemUsed_Caption_01[] PROGMEM = "You used an ice wand.\n  Monster is frozen\n  for two turns.";
char const itemUsed_Caption_02[] PROGMEM = "You used a fire wand.\n   Monster loses 10\n      HP points.";
char const itemUsed_Caption_03[] PROGMEM = "You used a poison wand.\n  Deal 2 extra damage\n       per turn.";

char const * const itemUsed_Captions[] = {
	itemUsed_Caption_01,
	itemUsed_Caption_02,
	itemUsed_Caption_03,
};

enum class ViewState : uint8_t {
  HighlightMonsterStats,
  HighlightPlayerStats,
  RollDice,
  DiceSelection,
  WandSelection,
  Defend,
  MonsterDead,
  MonsterDead_Wait,
  PlayerDead,
  ItemIceUsed,
  ItemFireUsed,
  ItemPoisonUsed
};

enum class SelectedElement : int8_t {
  None = -1,
  ItemFire,
  ItemIce,
  ItemPoison,
  Dice1,
  Dice2,
  Dice3,
  Dice4,
  Action,
};

class FightMonstersState : public BaseState {

  private:

    ViewState viewState = ViewState::RollDice;
    ViewState nextState = ViewState::RollDice;

    MonsterStats monsterStats;

    SelectedElement selectedElement = SelectedElement::None;
    uint8_t dice[4];
    uint8_t dice_Sixes[4];

    uint8_t counter = 0;
    uint8_t ice = 0;

    bool dice_Retain[4];
    bool poison = false;


  private:

    SelectedElement prevDiceSelection(SelectedElement index);
    SelectedElement nextDiceSelection(SelectedElement index);
    SelectedElement prevWandSelection(StateMachine & machine, SelectedElement index);
    SelectedElement nextWandSelection(StateMachine & machine, SelectedElement index);

    SelectedElement canUseWand(StateMachine & machine);
    uint8_t getMonsterDMG(StateMachine & machine);
    void setDiceSelection(StateMachine & machine, bool value);
    void monsterIsDead(StateMachine & machine);

  public:	

  	void activate(StateMachine & machine) override;
    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;
    
};