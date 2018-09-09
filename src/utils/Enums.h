#pragma once

#include <stdint.h>

#define _DEBUG

constexpr const static uint8_t FLASH_DELAY = 24;
constexpr const static uint8_t FLASH_COUNTER = 70;
constexpr const static uint8_t WINNER_LEVEL = 5;

const uint8_t InitSettings[] PROGMEM = {
	1, 5, 5, 6,
	0, 5, 3, 6,
  0, 4, 2, 5,
  0, 3, 1, 3
};


const uint8_t DiceDelay[] PROGMEM = { 
  1, 1, 1, 1, 1, 1, 1, 1, 
  2, 2, 2, 2, 4, 4, 4, 8, 8,   
};


enum class GameStateType : uint8_t {
  None,
	Resting,
	Treasure,
	Event,
  Merchant,
  Trap,
  Monster,
  BossMonster,
  MonsterFromEvent,
  GamePlay,
  ShowCards,
	SplashScreen,
	TitleScreen,
  Winner,
  PlayerDead
};


enum class Wand : uint8_t { 
  Fire,
  Ice,
  Poison,
  Healing
};

struct PlayerStats {

  int8_t armour;
  int8_t food;
  int8_t gold;
  int8_t hp;
  uint8_t xpTrack = 1;
  uint8_t xp;
  uint8_t items[4];

  void incArmour(int8_t val) {
  
    armour = armour + val;
    armour = (armour < 0 ? 0 : (armour > 5 ? 5 : armour));

  }

  void incFood(int8_t val) {
  
    food = food + val;
    food = (food < 0 ? 0 : (food > 10 ? 10 : food));

  }

  void incGold(int8_t val) {
  
    gold = gold + val;
    gold = (gold < 0 ? 0 : (gold > 10 ? 10 : gold));

  }

  void incHP(int8_t val) {
  
    hp = hp + val;
    hp = (hp < 0 ? 0 : (hp > 20 ? 20 : hp));

  }

  void incXP(uint8_t value) {

    xp = xp + value;

    if (xp >= 6 && xpTrack < 4) {

      xp = xp - 5;
      xpTrack++;

    }
  
  }

  void resetGame() {

    items[0] = 0;
    items[1] = 0;
    items[2] = 0;
    items[3] = 0;
    xpTrack = 1;
    //xpTrack = 2; /*sjh*/
    xp = 0;

  }

  uint8_t itemCount() {

    return items[0] + items[1] + items[2] + items[3];

  }

};


struct GameStats {

  uint8_t skillLevel = 1;
  uint8_t level = 1;
  uint8_t room = 0;
  uint8_t selectedCard = 0;
  bool monsterDefeated = false;

  void resetGame() {
    level = 0;
    room = 0;
    monsterDefeated = false;
    selectedCard = 0;
  }

  void dropArea() {

    switch (level) {
      
      case 0 ... 3:     level = level + 2;    break;
      case 4 ... 9:     level = level + 3;    break;
      case 10 ... 14:   break;

    }

  }

  uint8_t getAreaId() {

    switch (level) {

      case 0 ... 1:    return 0;
      case 2 ... 3:    return 1;
      case 4 ... 6:    return 2;
      case 7 ... 9:    return 3;
      default:         return 4;

    }
    
  }

  bool isLastLevelInArea() {

    switch (level) {
      
      case 1:
      case 3:
      case 6:
      case 9:
      case 13:
        return true;

      default:
        return false;

    }
  
  }

  GameStateType incRoom(PlayerStats & playerStats) { 
  
    room++;

    switch (room) {

      case 1:   selectedCard = 0;   break;
      case 2:   selectedCard = 1;   break;
      case 3:   selectedCard = 3;   break;
      case 4:   selectedCard = 4;   break;
      case 5:   selectedCard = 6;   break;
      
    }
 
    if ((room == 6 && isLastLevelInArea()) || (room == 5 && !isLastLevelInArea())) {

      playerStats.incFood(-1);

      if (playerStats.food > 0) {

        room = 0;
        level++;
        selectedCard = 0;
        monsterDefeated = false;

      }
      
    }

    return (getAreaId() == WINNER_LEVEL ? GameStateType::Winner : GameStateType::ShowCards);

  }


  uint8_t getMonsterDMG() {

    return (getAreaId() + 1) * 2;

  }


  uint8_t getBossMonsterDMG() {

    switch (getAreaId()) {

      case 0:   return 3;
      case 1:   return 5;
      case 2:   return 7;
      case 3:   return 9;
      case 4:   return 12;
      
      default:  return 0;

    }

  }
  
  uint8_t getMonsterReward() {

    return (getAreaId() + 1);

  }

};