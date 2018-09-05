#pragma once

#include "BaseState.h"
#include "../utils/Enums.h"
#include "../utils/FadeEffects.h"
#include "../fonts/Font3x5.h"

class RestingState : public BaseState {
  
  enum class ViewState : uint8_t {
    SelectReward,
    UpdateStats,
  };
  
  enum class SelectedItem : uint8_t {
    Food,
    Heal,
    Weapon,
  };

  private:

    ViewState viewState = ViewState::SelectReward;
    //Font3x5 font3x5 = Font3x5();

    SelectedItem selectedItem = SelectedItem::Food;
    SelectedItem prevSelection = SelectedItem::Food;
    
    uint8_t counter = 0;


  public:

    void update(StateMachine & machine) override;
    void render(StateMachine & machine) override;

};