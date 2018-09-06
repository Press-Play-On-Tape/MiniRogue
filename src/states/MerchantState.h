#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"

char const error_01[] PROGMEM = "   You need\nmore gold!";
char const error_02[] PROGMEM = "You don't have\none to sell!";
char const error_03[] PROGMEM = "  You already\nhave enough!";

char const * const error_Captions[] = {
	error_01,
	error_02,
	error_03,
};

class MerchantState : public BaseState {
  
  enum class ViewState : uint8_t {
    Buying,
    Selling,
  };

  private:

    ViewState viewState = ViewState::Buying;
    
    uint8_t selectedItem = 0;
    uint8_t errorNumber = 0;
    
    bool flashXP = false;
    bool flashHP = false;
    bool flashGold = false;
    bool flashFood = false;


  public:

    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};