#pragma once

#include <stdint.h>

#define _DEBUG

constexpr const static uint8_t FLASH_DELAY = 24;
constexpr const static uint8_t FLASH_COUNTER = 70;

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
  GameOver,
  ShowCards,
	SplashScreen,
	TitleScreen,
  Winner
};


enum class Wand : uint8_t { 
  Fire,
  Ice,
  Poison
};

struct PlayerStats {

  uint8_t armour;
  uint8_t food;
  uint8_t gold;
  uint8_t hp;
  uint8_t xpTrack = 1;
  uint8_t xp;
  uint8_t items[3];

  void incXP(uint8_t value) {

    xp = xp + value;

    if (xp >= 6 && xpTrack < 4) {

      xp = xp - 6;
      xpTrack++;

    }
  
  }

  void resetGame() {

    items[0] = 0;
    items[1] = 0;
    items[2] = 0;
    xpTrack = 1;
    xp = 0;

  }

  uint8_t itemCount() {

    return items[0] + items[1] + items[2];

  }

};


struct GameStats {

  uint8_t skillLevel = 0;
  uint8_t level = 0;
  uint8_t room = 0;
  uint8_t selectedCard = 0;
  bool monsterDefeated = false;

  void dropArea() {

    switch (level) {
      
      case 0 ... 5:     level = level + 3;    break;
      case 6 ... 9:     level = level + 4;    break;
      case 10 ... 14:   level = level + 5;    break;

    }

  }

  uint8_t getAreaId() {

    switch (level) {

      case 0 ... 2:    return 0;
      case 3 ... 5:    return 1;
      case 6 ... 9:    return 2;
      case 10 ... 14:  return 3;
      default:         return 4;

    }
    
  }

  bool isLastLevelInArea() {

    switch (level) {
      
      case 2:
      case 5:
      case 9:
      case 14:
      case 19:
        return true;

      default:
        return false;

    }
  
  }

  void incRoom(PlayerStats & playerStats) { 
  
    room++;

    switch (room) {

      case 1:   selectedCard = 0;   break;
      case 2:   selectedCard = 1;   break;
      case 3:   selectedCard = 3;   break;
      case 4:   selectedCard = 4;   break;
      case 5:   selectedCard = 6;   break;
      
    }
    
    if ((room == 6 && isLastLevelInArea()) || (room == 5 && !isLastLevelInArea())) {

      playerStats.food--;

      if (playerStats.food > 0) {

        room = 0;
        level++;
        selectedCard = 0;
        monsterDefeated = false;

      }
      
    }

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