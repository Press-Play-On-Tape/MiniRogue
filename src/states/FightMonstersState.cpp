#include "FightMonstersState.h"

#include "../images/Images.h"
#include "../fonts/Font3x5.h"
#include "../utils/Utils.h"


// ----------------------------------------------------------------------------
//  Initialise state ..
//
void FightMonstersState::activate(StateMachine & machine) {
  
	auto & gameStats = machine.getContext().gameStats;
	auto & playerStats = machine.getContext().playerStats;

	setDiceSelection(machine, false);

	switch (machine.getContext().gameState) {

		case GameStateType::Monster:
			this->monsterStats.hp = (gameStats.level + 1) + random(1, 7);
			this->monsterStats.dmg = gameStats.getMonsterDMG();
			break;

		case GameStateType::BossMonster:
			this->monsterStats.hp = 10 + (gameStats.getAreaId() * 5);
			this->monsterStats.dmg = gameStats.getBossMonsterDMG();
			break;

		case GameStateType::MonsterFromEvent:
			this->monsterStats.hp = (gameStats.level + 1) + random(1, 7);
			this->monsterStats.dmg = 5;
			break;

		default: break;

	}

	nextState = ViewState::RollDice;

	/*SJH*/ //this->monsterStats.hp = 1;
	/*SJH*/ //machine.getContext().playerStats.itemIce = 1;
	/*SJH*/ //machine.getContext().playerStats.itemFire = 1;
	/*SJH*/ //machine.getContext().playerStats.itemPoison = 1;

}


// ----------------------------------------------------------------------------
//  Handle state updates .. 
//
void FightMonstersState::update(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;
  auto justPressed = arduboy.justPressedButtons();

	switch (this->viewState) {

		case ViewState::HighlightMonsterStats:
		case ViewState::HighlightPlayerStats:

      #ifdef PRESS_A_TO_BYPASS
			if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER - 1; }
			#endif
			
			if (this->counter < FLASH_COUNTER) {

				this->counter++;

				if (counter == FLASH_COUNTER) {

					counter = 0;
					this->viewState = nextState;
					arduboy.resetFrameCount();

				}

			}
			break;

		case ViewState::RollDice:

      #ifdef PRESS_A_TO_BYPASS
			if (justPressed & A_BUTTON) { counter = sizeof(DiceDelay); }
			#endif

			if (counter < sizeof(DiceDelay)) {
				
				if (arduboy.everyXFrames(pgm_read_byte(&DiceDelay[counter]))) {

					for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
						if (!this->dice_Retain[i]) this->dice[i] = random(1, 7);
					}

					counter++;
					arduboy.resetFrameCount();

				}

			}
			else {


				// Highlight first available dice option ..

				this->selectedElement = SelectedElement::ItemPoison;
				this->selectedElement = nextDiceSelection(this->selectedElement);

				setDiceSelection(machine, true); 
				this->viewState = ViewState::DiceSelection;

			}
			break;

		case ViewState::DiceSelection:

			if (justPressed & LEFT_BUTTON) 		{ this->selectedElement = prevDiceSelection(this->selectedElement); }
			if (justPressed & RIGHT_BUTTON)		{ this->selectedElement = nextDiceSelection(this->selectedElement); }
			
			if (justPressed & A_BUTTON) {

				switch (this->selectedElement) {

					case SelectedElement::Dice1 ... SelectedElement::Dice4:

						counter = 0;
						this->dice_Retain[static_cast<uint8_t>(this->selectedElement) - 3] = false;
						this->dice_Sixes[static_cast<uint8_t>(this->selectedElement) - 3]++;
						this->viewState = ViewState::RollDice;
						break;

					case SelectedElement::Action:
						{
							uint8_t dmg = getMonsterDMG(machine);

							this->monsterStats.hp = clamp(this->monsterStats.hp - dmg, 0, 30);	
							setDiceSelection(machine, false);

							if (this->monsterStats.hp == 0) { 

								monsterIsDead(machine);

							}
							else {
								
								counter = 0;
								this->selectedElement = canUseWand(machine);

								if (dmg != 0) {

									if (this->selectedElement == SelectedElement::None) {

										this->viewState = ViewState::HighlightMonsterStats;
										if (this->ice == 0) {
											this->nextState = ViewState::Defend;
										}
										else {
											this->nextState = ViewState::RollDice;
										}
										
									}
									else {
										this->viewState = ViewState::HighlightMonsterStats;
										this->nextState = ViewState::WandSelection;
									}

								}
								else {

									if (this->selectedElement == SelectedElement::None) {

										if (this->ice == 0) {
											this->viewState = ViewState::Defend;
										}
										else {
											this->viewState = ViewState::RollDice;
										}

									}
									else {
										this->viewState = ViewState::WandSelection;
									}

								}

							}

						}
						break;

					default: break;

				}
			
			}

			break;

		case ViewState::WandSelection:

			if (justPressed & UP_BUTTON) 			{ this->selectedElement = prevWandSelection(machine, this->selectedElement); }
			if (justPressed & DOWN_BUTTON) 		{ this->selectedElement = nextWandSelection(machine, this->selectedElement); }
			
			if (justPressed & A_BUTTON) {

				switch (this->selectedElement) {

					case SelectedElement::ItemFire:

						playerStats.items[static_cast<uint8_t>(Wand::Fire)]--;
						this->monsterStats.hp = clamp(this->monsterStats.hp - 10, 0, 30);	
						setDiceSelection(machine, false);

						if (this->monsterStats.hp == 0) { 

							monsterIsDead(machine);

						}
						else {

							this->viewState = ViewState::ItemFireUsed;

						}
						break;

					case SelectedElement::ItemIce:
						this->ice = 2;
						this->viewState = ViewState::ItemIceUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Ice)]--;
						break;

					case SelectedElement::ItemPoison:
						this->poison = true;
						this->viewState = ViewState::ItemPoisonUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Poison)]--;
						break;

					default: 
						if (this->ice == 0) {
							this->viewState = ViewState::Defend;
						}
						else {
							this->viewState = ViewState::RollDice;
						}
						break;

				}
			
			}

			break;

		case ViewState::Defend:

			playerStats.hp = clamp(playerStats.hp - this->monsterStats.dmg + playerStats.armour, 0, 20);	
			if (playerStats.hp == 0) {

				this->viewState = ViewState::HighlightPlayerStats;
				this->nextState = ViewState::PlayerDead;

			}
			else {

				setDiceSelection(machine, false);
				this->viewState = ViewState::HighlightPlayerStats;
				this->nextState = ViewState::RollDice;
			}
			
			break;

		case ViewState::MonsterDead:

      #ifdef PRESS_A_TO_BYPASS
			if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER - 1; }
			#endif
			
			if (this->counter < FLASH_COUNTER) {

				this->counter++;

				if (counter == FLASH_COUNTER) {

					this->viewState = ViewState::MonsterDead_Wait;

				}

			}

			break;

		case ViewState::MonsterDead_Wait:

			if (justPressed & A_BUTTON) {

				gameStats.monsterDefeated = true;
        machine.changeState(gameStats.incRoom(playerStats)); 

			}

			break;

		case ViewState::PlayerDead:

			if (justPressed & A_BUTTON) {
				machine.changeState(GameStateType::TitleScreen);
			}

			break;

		case ViewState::ItemIceUsed:

			if (justPressed & A_BUTTON) {
				this->viewState = ViewState::RollDice;
			}

			break;
			
		case ViewState::ItemFireUsed:
		case ViewState::ItemPoisonUsed:

			if (justPressed & A_BUTTON) {
				this->viewState = ViewState::Defend;
			}

			break;

	}

}


void FightMonstersState::monsterIsDead(StateMachine & machine ) {
	
	auto & playerStats = machine.getContext().playerStats;
	auto & gameStats = machine.getContext().gameStats;

	switch (machine.getContext().gameState) {

		case GameStateType::Monster:
			playerStats.incXP(gameStats.getMonsterReward());
			break;

		case GameStateType::BossMonster:
		// 	playerStats.incXP(2);
		// 	break;

		// case GameStateType::MonsterFromEvent:
			playerStats.incXP(2);
			break;

		default: break;

	}

	this->viewState = ViewState::MonsterDead;
	
}


// ----------------------------------------------------------------------------
//  Render the state .. 
//
void FightMonstersState::render(StateMachine & machine) {

	auto & arduboy = machine.getContext().arduboy;
	auto & playerStats = machine.getContext().playerStats;
	auto & ardBitmap = machine.getContext().ardBitmap;

	bool flash = arduboy.getFrameCountHalf(20);

  BaseState::renderBackground(machine, true);
  // ardBitmap.drawCompressed(0, 0, Images::Background_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	// ardBitmap.drawCompressed(89, 0, Images::Background_Divider_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
  // ardBitmap.drawCompressed(105, 0, Images::Health_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);
	ardBitmap.drawCompressed(0, 0, Images::Monster_Items_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	{
		uint8_t const *imageName = nullptr;
		uint8_t const *maskName = nullptr;

		switch (machine.getContext().gameState) {

			case GameStateType::Monster:
			case GameStateType::MonsterFromEvent:

				imageName = Images::Monster_Only_Comp;
				maskName = Images::Monster_Only_Mask_Comp;
				break;

			case GameStateType::BossMonster:

				imageName = Images::BossMonster_Only_Comp;
				maskName = Images::BossMonster_Only_Mask_Comp;
				break;

			default: break;

		}

		ardBitmap.drawCompressed(58, 0, maskName, BLACK, ALIGN_NONE, MIRROR_NONE);
		ardBitmap.drawCompressed(58, 0, imageName, WHITE, ALIGN_NONE, MIRROR_NONE);
		ardBitmap.drawCompressed(35, 0, Images::Monster_LHS_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

	}


	// Monster statistics ..
	{

		if (this->viewState == ViewState::HighlightMonsterStats && flash) {
			font3x5.setTextColor(BLACK);
			arduboy.fillRect(20, 2, (this->monsterStats.hp < 10 ? 5 : 10), 7, WHITE);
		}

		font3x5.setCursor(21, 2);
		font3x5.print(this->monsterStats.hp);
		font3x5.setTextColor(WHITE);
		font3x5.setCursor(21, 10);
		font3x5.print(this->monsterStats.dmg);

	}


	// Inventory ..
	{
		font3x5.setCursor(21, 20);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Fire)]);
		font3x5.setCursor(21, 30);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Ice)]);
		font3x5.setCursor(21, 40);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Poison)]);

		if (this->ice > 0) 	arduboy.drawVerticalDottedLine(29, 29 + (this->ice * 4), 0, 2); 
		if (this->poison) 	arduboy.drawVerticalDottedLine(39, 47, 0, 2); 

	}


	// Dice ..

	for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
		SpritesB::drawOverwrite(3 + (i * 10), 52, Images::Dice, this->dice[i]);
	}


	// Marker ..
	
	if ((this->viewState == ViewState::DiceSelection || this->viewState == ViewState::WandSelection) && ((this->selectedElement == SelectedElement::Action && flash) || this->selectedElement != SelectedElement::Action)) {
		SpritesB::drawOverwrite(3 + (playerStats.xpTrack * 10), 53, Images::Marker, 0);
	}


	// Highlighted item ..
	{

		if (this->viewState == ViewState::DiceSelection || this->viewState == ViewState::WandSelection) {

		  if (!flash) {
			
				switch (this->selectedElement) {

					case SelectedElement::ItemFire ... SelectedElement::ItemPoison:
						SpritesB::drawPlusMask(3, 19 + (static_cast<uint8_t>(this->selectedElement) * 10), Images::Dice_Highlight_SelfMasked, 0);
						break;

					case SelectedElement::Dice1 ... SelectedElement::Dice4:
						SpritesB::drawPlusMask(3 + ((static_cast<uint8_t>(this->selectedElement) - 3) * 10), 52, Images::Dice_Highlight_SelfMasked, 0);
						break;

					default: break;
		
				}

			}

		}

	}


	// Player statistics ..

  BaseState::renderPlayerStatistics(machine,
    true, // Overall
    false, // XP
    (this->viewState == ViewState::HighlightPlayerStats), // HP
    false, // Armour
    false, // Gold
    false // Food
  );


	// Messages ..

	switch (this->viewState) {

		case ViewState::MonsterDead:

			BaseState::renderPlayerStatistics(machine,
				true, // Overall
				true, // XP
				false, // HP
				false, // Armour
				(machine.getContext().gameState == GameStateType::BossMonster), // Gold
				false // Food
			);
			
			BaseState::renderMonsterDead();
			break;

		case ViewState::MonsterDead_Wait:
			BaseState::renderMonsterDead();
			break;

		case ViewState::PlayerDead:

			BaseState::renderPlayerDead();
			break;

		case ViewState::ItemIceUsed:
		case ViewState::ItemFireUsed:
		case ViewState::ItemPoisonUsed:

			arduboy.fillRect(17, 10, 94, 42, BLACK);
			arduboy.drawFastHLine(18, 11, 92);
			arduboy.drawFastHLine(18, 40, 92);

			font3x5.setCursor(20, 14);
			font3x5.print(FlashString(itemUsed_Captions[ static_cast<uint8_t>(this->viewState) - static_cast<uint8_t>(ViewState::ItemIceUsed) ]));
			break;

		default: break;

	}

}



// ----------------------------------------------------------------------------
//  Get the previous dice selection.  Dices need to be 6 to select ..
//
SelectedElement FightMonstersState::prevDiceSelection(SelectedElement element) {

	if (element > SelectedElement::Dice4 && this->dice[3] == 6) 							{ return SelectedElement::Dice4; }
	if (element > SelectedElement::Dice3 && this->dice[2] == 6) 							{ return SelectedElement::Dice3; }
	if (element > SelectedElement::Dice2 && this->dice[1] == 6) 							{ return SelectedElement::Dice2; }
	if (element > SelectedElement::Dice1 && this->dice[0] == 6) 							{ return SelectedElement::Dice1; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the next dice or action selection.  Dices need to be 6 to select ..
//
SelectedElement FightMonstersState::nextDiceSelection(SelectedElement element) {

	if (element < SelectedElement::Dice1 && this->dice[0] == 6) 							{ return SelectedElement::Dice1; }
	if (element < SelectedElement::Dice2 && this->dice[1] == 6) 							{ return SelectedElement::Dice2; }
	if (element < SelectedElement::Dice3 && this->dice[2] == 6) 							{ return SelectedElement::Dice3; }
	if (element < SelectedElement::Dice4 && this->dice[3] == 6) 							{ return SelectedElement::Dice4; }
	if (element < SelectedElement::Action) 																		{ return SelectedElement::Action; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the previous wand selection ..
//
SelectedElement FightMonstersState::prevWandSelection(StateMachine & machine, SelectedElement element) {

	auto & playerStats = machine.getContext().playerStats;

	if (element > SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 	{ return SelectedElement::ItemPoison; }
	if (element > SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 				{ return SelectedElement::ItemIce; }
	if (element > SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 			{ return SelectedElement::ItemFire; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the next wand or action selection ..
//
SelectedElement FightMonstersState::nextWandSelection(StateMachine & machine, SelectedElement element) {
	
	auto & playerStats = machine.getContext().playerStats;

	if (element < SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 			{ return SelectedElement::ItemFire; }
	if (element < SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 				{ return SelectedElement::ItemIce; }
	if (element < SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 	{ return SelectedElement::ItemPoison; }
	if (element < SelectedElement::Action) 																																		{ return SelectedElement::Action; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the damage inflicted on the monster this roll of the dice ..
//
uint8_t FightMonstersState::getMonsterDMG(StateMachine & machine) {

	uint8_t damage = 0;

	for (uint8_t i = 0; i < machine.getContext().playerStats.xpTrack; i++) {
		if (this->dice[i] != 1) {
			damage = damage + this->dice[i] + (this->dice_Sixes[i] * 6);
		}
	}

	return damage + (poison ? 2 : 0);

}


// ----------------------------------------------------------------------------
//  Does the player have a wand to use?  If so, select it ..
//
SelectedElement FightMonstersState::canUseWand(StateMachine & machine) {

	SelectedElement test = SelectedElement::None;
	test = nextWandSelection(machine, test);

	return (test < SelectedElement::Dice1 ? test : SelectedElement::None);

}


// ----------------------------------------------------------------------------
//  Clear dice selection array ready to roll again ..
//
void  FightMonstersState::setDiceSelection(StateMachine & machine, bool value) {

	for (uint8_t i = 0; i < machine.getContext().playerStats.xpTrack; i++) {

		this->dice_Retain[i] = value;

		if (!value) {
			this->dice_Sixes[i] = 0;
			if (this->ice > 0) this->ice--;
		}

	}

}
