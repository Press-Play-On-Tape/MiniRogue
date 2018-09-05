#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"

class MerchantState : public BaseState {
  
  enum class ViewState : uint8_t {
    Buying,
    Selling,
  };

  private:

    ViewState viewState = ViewState::Buying;
    
    uint8_t selectedItem = 0;
    bool flashXP = false;
    bool flashHP = false;
    bool flashGold = false;
    bool flashFood = false;


  public:

    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};