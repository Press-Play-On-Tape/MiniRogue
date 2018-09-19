#include "BaseState.h"

#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../utils/Enums.h"
#include "../fonts/Font3x5.h"

void BaseState::renderSpinningCard(StateMachine & machine, int8_t x, int8_t y, uint8_t i, uint8_t ySpacing) {

  auto & ardBitmap = machine.getContext().ardBitmap;

  const auto spinning_mask = Images::spinning_mask[i];
  const auto spinning_card = Images::spinning_card[i];

  for (uint8_t j = 0; j < 4; ++j) {

    const uint8_t xOffset = ((j % 2) ? 10 : 0);
    const uint8_t yOffset = ((j < 2) ? y : y + ySpacing);
    ardBitmap.drawCompressed(x + xOffset, yOffset, spinning_mask, BLACK, ALIGN_NONE, j);

  }

  for (uint8_t j = 0; j < 4; ++j) {

    const uint8_t xOffset = ((j % 2) ? 10 : 0);
    const uint8_t yOffset = ((j < 2) ? y : y + ySpacing);
    ardBitmap.drawCompressed(x + xOffset, yOffset, spinning_card, WHITE, ALIGN_NONE, j);

  }

}

void BaseState::renderLargeSpinningCard(StateMachine & machine, int8_t x, int8_t y, uint8_t i) {

	auto & ardBitmap = machine.getContext().ardBitmap;

	const auto spinning_card = Images::Large_Spinning_Cards[i];

	for(uint8_t j = 0; j < 4; ++j) {

		const uint8_t xOffset = ((j % 2) ? 16 : 0);
		const uint8_t yOffset = ((j < 2) ? y : y + 16);
		ardBitmap.drawCompressed(x + xOffset, yOffset, spinning_card, WHITE, ALIGN_NONE, j);

	}

}

void BaseState::renderTitleBackground(StateMachine & machine, bool drawLowerLines) {

	auto & ardBitmap = machine.getContext().ardBitmap;
  auto & arduboy = machine.getContext().arduboy;

	ardBitmap.drawCompressed(0, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(64, 0, Images::Title_Blank_Comp, WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);

	ardBitmap.drawCompressed(5, 20, Images::Flames[(this->leftFlame == 3 ? 1: this->leftFlame)], WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(107, 20, Images::Flames[(this->rightFlame == 3 ? 1: this->rightFlame)], WHITE, ALIGN_NONE, MIRROR_HORIZONTAL);

  if (arduboy.everyXFrames(3)) { 
    this->leftFlame++;
    if (this->leftFlame % 4 == 0) this->leftFlame = 0;
    this->rightFlame++;
    if (this->rightFlame % 4 == 0) this->rightFlame = 0;
  }

  if (drawLowerLines) {
    arduboy.drawFastHLine(17, 49, 94);
    arduboy.drawFastHLine(17, 51, 94);
    arduboy.drawHorizontalDottedLine(17, 110, 53);
  }

}

void BaseState::renderMessageBox(StateMachine & machine, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {

	auto & arduboy = machine.getContext().arduboy;

  arduboy.fillRect(x, y, w, h, BLACK);
  arduboy.drawFastHLine(x + 3, y + 2, w - 6);
  arduboy.drawFastHLine(x + 3, y + h - 3, w - 6);
  arduboy.drawFastVLine(x + 2, y + 3, h - 6);
  arduboy.drawFastVLine(x + w - 3, y + 3, h - 6);

}

void BaseState::renderPlayerDead(uint8_t y) {

  Sprites::drawOverwrite(38, y, Images::Message_PlayerDead, 0);

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

void BaseState::renderPlayerStatistics(StateMachine & machine, bool overallFlash, FlashSettings settings){

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY) && overallFlash;

  for (uint8_t x = 0; x < 6; x++) {
    
    uint8_t val = 0;
    bool flag = false;

    switch (x)  {

      case 0:   
        val = playerStats.xpTrack;       
        break;

      case 1:   
        val = playerStats.xp;       
        flag = ((settings & FlashSettings::FlashXP) != 0);
        break;

      case 2:   
        val = playerStats.hp;
        flag = ((settings & FlashSettings::FlashHP) != 0);
        break;

      case 3:   
        val = playerStats.armour;
        flag = ((settings & FlashSettings::FlashArmour) != 0);
        break;

      case 4:   
        val = playerStats.gold;
        flag = ((settings & FlashSettings::FlashGold) != 0);
        break;

      case 5:   
        val = playerStats.food;
        flag = ((settings & FlashSettings::FlashFood) != 0);
        break;

    }

    if (flag && flash) {
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