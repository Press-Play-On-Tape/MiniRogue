#include "BaseState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

void BaseState::renderSpinningCard(StateMachine & machine, int8_t x, int8_t y, uint8_t i) {

  auto & ardBitmap = machine.getContext().ardBitmap;
  ardBitmap.drawCompressed(x, y, Images::spinning_mask[i], BLACK, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(x, y, Images::spinning_card[i], WHITE, ALIGN_NONE, MIRROR_NONE);

}

void BaseState::renderBackground(StateMachine & machine) {

	auto & ardBitmap = machine.getContext().ardBitmap;
	ardBitmap.drawCompressed(0, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(64, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);
  
}

void BaseState::renderMessageBox(StateMachine & machine, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {

	auto & arduboy = machine.getContext().arduboy;

  arduboy.fillRect(x, y, w, h, BLACK);
  arduboy.drawFastHLine(x + 3, y + 2, w - 6);
  arduboy.drawFastHLine(x + 3, y + h - 3, w - 6);
  arduboy.drawFastVLine(x + 2, y + 3, h - 6);
  arduboy.drawFastVLine(x + w - 3, y + 3, h - 6);

}

void BaseState::renderPlayerDead() {

  Sprites::drawOverwrite(38, 25, Images::Message_PlayerDead, 0);

}

void BaseState::renderMonsterDead(StateMachine & machine) {

  renderMessageBox(machine, 34, 24, 60, 18);
  font3x5.setCursor(40, 29);
  font3x5.print(F("Enemy Killed"));

}

void BaseState::renderBackground(StateMachine & machine, bool renderCorners) {

	auto & ardBitmap = machine.getContext().ardBitmap;

  if (renderCorners) ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  for (uint8_t y= 0; y < 64; y = y + 10)  ardBitmap.drawCompressed(89, y, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

}

void BaseState::renderPlayerStatistics(StateMachine & machine, bool overallFlash, bool flashXP, bool flashHP, bool flashArmour, bool flashGold, bool flashFood) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


  for (uint8_t x = 0; x < 6; x++) {
    
    uint8_t val = 0;
    bool flag = false;

    switch (x)  {

      case 0:   
        val = playerStats.xpTrack;       
        break;

      case 1:   
        val = playerStats.xp;       
        flag = flashXP;
        break;

      case 2:   
        val = playerStats.hp;
        flag = flashHP;
        break;

      case 3:   
        val = playerStats.armour;
        flag = flashArmour;
        break;

      case 4:   
        val = playerStats.gold;
        flag = flashGold;
        break;

      case 5:   
        val = playerStats.food;
        flag = flashFood;
        break;

    }

    if (overallFlash && flag && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, (x * 9), (val < 10 ? 5 : 10), 7, WHITE);
    }

    font3x5.setCursor(120, (x * 9));
    font3x5.print(val);
    font3x5.setTextColor(WHITE);  

  }

  {

    uint8_t position = 0;

    for (uint8_t x = 0; x < 4; x ++ ) {

      if (playerStats.items[x] > 0) { drawItem(position, Images::Wands[x]); position++; }
      if (playerStats.items[x] > 1) { drawItem(position, Images::Wands[x]); position++; }

    }

    arduboy.drawRect(106, 55, 9, 9, WHITE);
    arduboy.drawRect(117, 55, 9, 9, WHITE);

  }

}

void BaseState::drawItem(uint8_t position, uint8_t const *imageName) {

  Sprites::drawOverwrite(107 + (position * 11), 56, imageName, 0);

}