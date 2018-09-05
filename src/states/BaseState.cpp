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

void BaseState::renderPlayerStatistics(StateMachine & machine, bool overallFlash, bool flashXP, bool flashHP, bool flashArmour, bool flashGold, bool flashFood, bool showCardsView) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
  auto & gameStats = machine.getContext().gameStats;

  const bool flash = arduboy.getFrameCountHalf(FLASH_DELAY);


	// Player statistics ..

  { // XP
    if (overallFlash && flashXP && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, 0, 10, 7, WHITE);
    }

    font3x5.setCursor(120, 0);
    font3x5.print(playerStats.xpTrack);
    font3x5.print(playerStats.xp);
    font3x5.setTextColor(WHITE);  
  }
  
  { // HP
    if (overallFlash && flashHP && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, 11, (playerStats.hp < 10 ? 5 : 10), 7, WHITE);
    }

    font3x5.setCursor(120, 11);
    font3x5.print(playerStats.hp);
    font3x5.setTextColor(WHITE);
  }

  { // Armour
    if (overallFlash && flashArmour && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, 22, (playerStats.armour < 10 ? 5 : 10), 7, WHITE);
    }

    font3x5.setCursor(120, 22);
    font3x5.print(playerStats.armour);
    font3x5.setTextColor(WHITE);
  }

  { // Gold
    if (overallFlash && flashGold && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, 33, (playerStats.gold < 10 ? 5 : 10), 7, WHITE);
    }

    font3x5.setCursor(120, 33);
    font3x5.print(playerStats.gold);
    font3x5.setTextColor(WHITE);
  }

  { // Food
    if (overallFlash && flashFood && flash) {
      font3x5.setTextColor(BLACK);
      arduboy.fillRect(119, 44, (playerStats.food < 10 ? 5 : 10), 7, WHITE);
    }

    font3x5.setCursor(120, 44);
    font3x5.print(playerStats.food);
    font3x5.setTextColor(WHITE);
  }

  {

    uint8_t position = 0;

    if (playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) { drawItem(position, Images::IceWand, showCardsView); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 1) { drawItem(position, Images::IceWand, showCardsView); position++; }

    if (playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) { drawItem(position, Images::FireWand, showCardsView); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 1) { drawItem(position, Images::FireWand, showCardsView); position++; }

    if (playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) { drawItem(position, Images::PoisonWand, showCardsView); position++; }
    if (playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 1) { drawItem(position, Images::PoisonWand, showCardsView); position++; }

    arduboy.drawRect((showCardsView ? 107 : 106), 55, 9, 9, WHITE);
    arduboy.drawRect((showCardsView ? 118 : 117), 55, 9, 9, WHITE);

  }

}

void BaseState::drawItem(uint8_t position, uint8_t const *imageName, bool showCardsView) {

  Sprites::drawOverwrite((showCardsView ? 108 : 107) + (position * 11), 56, imageName, 0);

}