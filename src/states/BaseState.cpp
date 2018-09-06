#include "BaseState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

void BaseState::renderPlayerDead(StateMachine & machine) {

  Sprites::drawOverwrite(38, 24, Images::Message_PlayerDead, 0);

}

void BaseState::renderMonsterDead(StateMachine & machine) {

  Sprites::drawOverwrite(39, 24, Images::Message_MonsterDead, 0);

}

void BaseState::renderPlayerStatistics(StateMachine & machine, bool overallFlash, bool flashXP, bool flashHP, bool flashArmour, bool flashGold, bool flashFood) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
  auto & gameStats = machine.getContext().gameStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  for (uint8_t x = 0; x < 5; x++) {
    
    uint8_t val = 0;
    bool flag = false;

    switch (x)  {

      case 0:   
        val = playerStats.xp;       
        flag = flashXP;
        break;

      case 1:   
        val = playerStats.hp;
        flag = flashHP;
        break;

      case 2:   
        val = playerStats.armour;
        flag = flashArmour;
        break;

      case 3:   
        val = playerStats.gold;
        flag = flashGold;
        break;

      case 4:   
        val = playerStats.food;
        flag = flashFood;
        break;

    }

    if (overallFlash && flag && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, (x * 11), 5, 7, WHITE);
    }

    font3x5.setCursor(120, (x * 11));
    font3x5.print(val);
    font3x5.setTextColor(WHITE);  

  }

  {

    uint8_t position = 0;

    if (playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) { drawItem(position, Images::IceWand); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 1) { drawItem(position, Images::IceWand); position++; }

    if (playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) { drawItem(position, Images::FireWand); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 1) { drawItem(position, Images::FireWand); position++; }

    if (playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) { drawItem(position, Images::PoisonWand); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 1) { drawItem(position, Images::PoisonWand); position++; }

    arduboy.drawRect(106, 55, 9, 9, WHITE);
    arduboy.drawRect(117, 55, 9, 9, WHITE);

  }

}

void BaseState::drawItem(uint8_t position, uint8_t const *imageName) {

  Sprites::drawOverwrite(107 + (position * 11), 56, imageName, 0);

}