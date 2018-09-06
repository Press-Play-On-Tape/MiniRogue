#include "ShowCardsState.h"
#include "../images/Images.h"
#include "../utils/Utils.h"
#include "../fonts/Font3x5.h"

constexpr const static uint8_t NO_OF_CARDS_IN_FLIP = 7; 
constexpr const static uint8_t DEAL_DELAY = 5; 
constexpr const static uint8_t CARD_SHOW_ALL = 255;
constexpr const static uint8_t CARD_NONE_SELECTED = 255;
constexpr const static uint8_t ODD_COLS_Y = 23;
constexpr const static uint8_t EVEN_COLS_UPPER_Y = 8;
constexpr const static uint8_t EVEN_COLS_LOWER_Y = 36;
constexpr const static uint8_t COL_SPACING = 20;
constexpr const static int8_t  COL_1_X = -1;
constexpr const static uint8_t COL_2_X = COL_1_X + COL_SPACING;
constexpr const static uint8_t COL_3_X = COL_2_X + COL_SPACING;
constexpr const static uint8_t COL_4_X = COL_3_X + COL_SPACING;
constexpr const static uint8_t COL_5_X = COL_4_X + COL_SPACING;

const int8_t  cardPositionX[] PROGMEM = { COL_1_X, COL_2_X, COL_2_X, COL_3_X, COL_4_X, COL_4_X, COL_5_X };
const uint8_t cardPositionY[] PROGMEM = { ODD_COLS_Y, EVEN_COLS_UPPER_Y, EVEN_COLS_LOWER_Y, ODD_COLS_Y, EVEN_COLS_UPPER_Y, EVEN_COLS_LOWER_Y, ODD_COLS_Y };
const uint8_t cardIndexToRoom[] PROGMEM = { 1, 2, 2, 3, 4, 4, 5 };


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void ShowCardsState::activate(StateMachine & machine) {
	
	auto & arduboy = machine.getContext().arduboy;
  auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	this->numberOfCardsToDisplay = (gameStats.isLastLevelInArea() ? 7 : 6);
  arduboy.resetFrameCount();

	if (gameStats.room == 0) {

		this->viewState = ViewState::DealCards;
		this->displayCard = 0;
		this->counter = NO_OF_CARDS_IN_FLIP;
		gameStats.room = 1;


		// Shuffle cards ..

		machine.getContext().cards[6] = GameStateType::BossMonster;
		
		for (uint8_t i = 0; i < 10; i++) { 
			
			uint8_t x = random(0, 6);
			uint8_t y = random(0, 6);
			swap(machine.getContext().cards[x], machine.getContext().cards[y]);

		}

		machine.getContext().cards[0] = GameStateType::Event; 			//SJH
		// machine.getContext().cards[1] = GameStateType::Monster; 		//SJH
		// machine.getContext().cards[2] = GameStateType::Resting;		//SJH
		// machine.getContext().cards[3] = GameStateType::Merchant;			//SJH
		// machine.getContext().cards[4] = GameStateType::Trap;				//SJH
		// machine.getContext().cards[5] = GameStateType::Treasure;		//SJH

	}
	else {

		this->displayCard = CARD_SHOW_ALL;

		if (playerStats.food > 0) {
			this->counter = NO_OF_CARDS_IN_FLIP;
			this->viewState = ViewState::PlayCard;
		}
		else {
			this->viewState = ViewState::PlayerDead;
		}
		
	}
  
}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void ShowCardsState::update(StateMachine & machine) {
	
	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

	switch (this->viewState) {

		case ViewState::DealCards:

			if (arduboy.everyXFrames(DEAL_DELAY)) {

				if (this->displayCard < numberOfCardsToDisplay) {
					this->displayCard++;
				}
				else {
					this->displayCard = CARD_SHOW_ALL;
        	this->viewState = ViewState::PlayCard;
					this->counter = NO_OF_CARDS_IN_FLIP;
				}

			}
			break;

		case ViewState::PlayCard:

			if ((justPressed & UP_BUTTON) && (gameStats.selectedCard == 2 || gameStats.selectedCard == 5)) 		{ gameStats.selectedCard--;}
			if ((justPressed & DOWN_BUTTON) && (gameStats.selectedCard == 1 || gameStats.selectedCard == 4)) 	{ gameStats.selectedCard ++; }

			if (justPressed & A_BUTTON) { 
		
				machine.changeState(machine.getContext().cards[gameStats.selectedCard]); 
				
			}

			break;

		case ViewState::PlayerDead:

      if (justPressed & A_BUTTON) { 
        machine.changeState(GameStateType::GameOver);
			}

			break;

	}

}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void ShowCardsState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;
	auto & ardBitmap = machine.getContext().ardBitmap;
	
	uint8_t room = gameStats.room;


	// Display area names ..

	font3x5.setTextColor(WHITE);
	font3x5.setCursor(0, 0);

  #ifndef AREA_IN_LEVEL
	font3x5.print(FlashString(area_Captions[gameStats.getAreaId()]));
  #else
	font3x5.print("A");
	font3x5.print(gameStats.getAreaId() + 1);
	font3x5.print("~L");
	font3x5.print(gameStats.level + 1);
	font3x5.print(FlashString(area_Captions[gameStats.getAreaId()]));
  #endif

	// Draw background ..

	ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);	
	for (uint8_t i = 0; i < 94; i = i + 10) {
		ardBitmap.drawCompressed(i, 18, Images::Card_Border_Top_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
		ardBitmap.drawCompressed(i, 50, Images::Card_Border_Bottom_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	}
	ardBitmap.drawCompressed(98, 18, Images::Card_Border_Right_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    true,  // Overall
    false, // XP
    false, // HP
    false, // Armour
    false, // Gold
    false  // Food
  );

  const bool flash = arduboy.getFrameCountHalf(12);

	for (uint8_t i = 0; i < 7; i++) {

		if (this->displayCard >= i) {

			int8_t x =  pgm_read_byte(&cardPositionX[i]);
			uint8_t y = pgm_read_byte(&cardPositionY[i]);
			uint8_t r = pgm_read_byte(&cardIndexToRoom[i]);

			if (this->counter == NO_OF_CARDS_IN_FLIP || (room != r && this->counter > 0) || this->counter == 0) {
			ardBitmap.drawCompressed(x, y, Images::Card_Outline_Comp_Mask, BLACK, ALIGN_NONE, MIRROR_NONE);
			ardBitmap.drawCompressed(x, y, Images::Card_Outline_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
			}
			if (this->displayCard == CARD_SHOW_ALL && (gameStats.selectedCard == i) && flash && playerStats.food > 0) {
				ardBitmap.drawCompressed(x, y, Images::Card_Outline_Highlight_Comp, BLACK, ALIGN_NONE, MIRROR_NONE);
			}

			if ((room > r) || (room == r && this->counter == 0) || (i == 6 && this->numberOfCardsToDisplay == 6)) {
				arduboy.fillRect(x + 3, y + 4, 14, 21, BLACK);
				SpritesB::drawSelfMasked(x + 3, y + 6, Images::Card_Faces, (i == 6 && this->numberOfCardsToDisplay == 6 ? 7 : static_cast<uint8_t>(machine.getContext().cards[i]) - 1));
			}

			if (room == r && this->counter > 0 && this->displayCard == CARD_SHOW_ALL) {
				arduboy.fillRect(x + 3, y + 4, 14, 21, BLACK);
				ardBitmap.drawCompressed(x, y, Images::spinning_mask[this->counter - 1], BLACK, ALIGN_NONE, MIRROR_NONE);
				ardBitmap.drawCompressed(x, y, Images::spinning_card[this->counter - 1], WHITE, ALIGN_NONE, MIRROR_NONE);

			}

		}

	}

	if (this->counter > 0 && this->displayCard == CARD_SHOW_ALL) this->counter --;


	// Are we dead?

  if (viewState == ViewState::PlayerDead) {

    BaseState::renderPlayerDead(machine);

	}

}

