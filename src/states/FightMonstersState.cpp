#include "FightMonstersState.h"

#include "../images/Images.h"
#include "../fonts/Font3x5.h"
#include "../utils/Utils.h"
#include "../utils/TreasureDice.h"


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

	//playerStats.xpTrack = 4; //SJH
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

			if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER - 1; }
			
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

			this->lastState = ViewState::RollDice;

			if (justPressed & A_BUTTON) { 

				counter = sizeof(DiceDelay); 

				for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
					if (!this->dice_Retain[i]) this->dice[i] = random(1, 7);
				}

			}

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
						this->dice_Retain[static_cast<uint8_t>(this->selectedElement) - 4] = false;
						this->dice_Sixes[static_cast<uint8_t>(this->selectedElement) - 4]++;
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
								this->lastState = ViewState::DiceSelection;
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
										this->selectedElement = SelectedElement::Action;
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
										this->selectedElement = SelectedElement::Action;
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

			this->lastState = ViewState::WandSelection;
	
			if (justPressed & UP_BUTTON || justPressed & LEFT_BUTTON) 			{ this->selectedElement = prevWandSelection(machine, this->selectedElement); }
			if (justPressed & DOWN_BUTTON || justPressed & RIGHT_BUTTON) 		{ this->selectedElement = nextWandSelection(machine, this->selectedElement); }
			
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

							this->lastState = ViewState::WandSelection;
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

					case SelectedElement::ItemHealing:
						this->viewState = ViewState::ItemHealingUsed;
						playerStats.items[static_cast<uint8_t>(Wand::Healing)]--;
						playerStats.incHP(8);
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

			if (justPressed & A_BUTTON) { this->counter = FLASH_COUNTER - 1; }
			
			if (this->counter < FLASH_COUNTER) {

				this->counter++;

				if (counter == FLASH_COUNTER) {

					this->lastState = this->viewState;
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
				machine.changeState(GameStateType::PlayerDead);
			}

			break;

		case ViewState::ItemIceUsed:
		case ViewState::ItemHealingUsed:

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
//			this->viewState = ViewState::MonsterDead;
			break;

		case GameStateType::BossMonster:
		 	{
				playerStats.incXP(2);

				if (playerStats.itemCount() == 2) {
					this->diceMonster = random(5, 7);
				}
				else {
					this->diceMonster = random(1, 7);
				}

				switch (this->diceMonster) {

					case 1: playerStats.items[static_cast<uint8_t>(Wand::Fire)]++; break;
					case 2: playerStats.items[static_cast<uint8_t>(Wand::Ice)]++; break;
					case 3: playerStats.items[static_cast<uint8_t>(Wand::Poison)]++; break;
					case 4: playerStats.items[static_cast<uint8_t>(Wand::Healing)]++; break;
					case 5: playerStats.incArmour(1); break;
					case 6: playerStats.incXP(2); break;

					break;

				}

//			this->viewState = ViewState::BossMonsterDead;

			}

		  	break;

		case GameStateType::MonsterFromEvent:
//		this->viewState = ViewState::MonsterDead;
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


	// Draw background ..

  BaseState::renderBackground(machine, true);
	ardBitmap.drawCompressed(0, 0, Images::Monster_Stats_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

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
	if (viewState == ViewState::WandSelection || lastState == ViewState::WandSelection) {

		ardBitmap.drawCompressed(0, 19, Images::Monster_Items_Comp, WHITE, ALIGN_NONE, MIRROR_NONE);

		font3x5.setCursor(21, 23);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Fire)]);
		font3x5.setCursor(21, 33);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Ice)]);
		font3x5.setCursor(21, 43);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Poison)]);
		font3x5.setCursor(21, 53);
		font3x5.print(playerStats.items[static_cast<uint8_t>(Wand::Healing)]);

		// if (this->ice > 0) 	arduboy.drawVerticalDottedLine(32, 35 + (this->ice * 3), 0, 2); 
		// if (this->poison) 	arduboy.drawVerticalDottedLine(42, 51, 0, 2); 

	}


	// Dice ..

	if (viewState == ViewState::DiceSelection || viewState == ViewState::RollDice || lastState == ViewState::DiceSelection) {
		for (uint8_t i = 0; i < playerStats.xpTrack; i++) {
			SpritesB::drawOverwrite(3 + (i * 10), 52, Images::Dice, this->dice[i]);
		}
	}


	// Marker ..
	
	if (this->viewState == ViewState::DiceSelection && ((this->selectedElement == SelectedElement::Action && flash) || this->selectedElement != SelectedElement::Action)) {
		SpritesB::drawOverwrite(3 + (playerStats.xpTrack * 10), 53, Images::Marker, 0);
	}

	if (this->viewState == ViewState::WandSelection && ((this->selectedElement == SelectedElement::Action && flash) || this->selectedElement != SelectedElement::Action)) {
		SpritesB::drawOverwrite(26, 53, Images::Marker, 0);
	}


	// Highlighted item ..
	{

		if (this->viewState == ViewState::DiceSelection || this->viewState == ViewState::WandSelection) {

		  if (!flash) {
			
				switch (this->selectedElement) {

					case SelectedElement::ItemFire ... SelectedElement::ItemHealing:
						SpritesB::drawPlusMask(3, 22 + (static_cast<uint8_t>(this->selectedElement) * 10), Images::Dice_Highlight_SelfMasked, 0);
						break;

					case SelectedElement::Dice1 ... SelectedElement::Dice4:
						SpritesB::drawPlusMask(3 + ((static_cast<uint8_t>(this->selectedElement) - 4) * 10), 52, Images::Dice_Highlight_SelfMasked, 0);
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

		case ViewState::HighlightPlayerStats:
	
			if (this->nextState == ViewState::PlayerDead) {
				BaseState::renderPlayerDead();
			}

			break;
			
		case ViewState::MonsterDead:
		//case ViewState::BossMonsterDead:

			BaseState::renderPlayerStatistics(machine,
				true, // Overall
				true, // XP
				false, // HP
				false, // Armour
				(machine.getContext().gameState == GameStateType::BossMonster), // Gold
				false // Food
			);

			// if (machine.getContext().gameState == GameStateType::BossMonster) {

    	// 	BaseState::renderMessageBox(machine);
			// 	font3x5.setCursor(56, 28);
			// 	font3x5.print(F("You killed the Boss!\n\n"));
			// 	font3x5.print(FlashString(treasureDice_Captions[ this->diceMonster]));

			// }
			// else {
			
				BaseState::renderMonsterDead();

			// }
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
		case ViewState::ItemHealingUsed:

			arduboy.fillRect(15, 17, 100, 32, BLACK);
			arduboy.drawFastHLine(18, 18, 94);
			arduboy.drawFastVLine(17, 19, 28);
			arduboy.drawFastHLine(18, 47, 94);
			arduboy.drawFastVLine(112, 19, 28);

			font3x5.setCursor(20, 21);
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

	if (element > SelectedElement::ItemHealing && playerStats.items[static_cast<uint8_t>(Wand::Healing)] > 0) 	{ return SelectedElement::ItemHealing; }
	if (element > SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 				{ return SelectedElement::ItemFire; }
	if (element > SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 					{ return SelectedElement::ItemIce; }
	if (element > SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 		{ return SelectedElement::ItemPoison; }

	return element;

}


// ----------------------------------------------------------------------------
//  Get the next wand or action selection ..
//
SelectedElement FightMonstersState::nextWandSelection(StateMachine & machine, SelectedElement element) {
	
	auto & playerStats = machine.getContext().playerStats;

	if (element < SelectedElement::ItemHealing && playerStats.items[static_cast<uint8_t>(Wand::Healing)] > 0) 	{ return SelectedElement::ItemHealing; }
	if (element < SelectedElement::ItemFire && playerStats.items[static_cast<uint8_t>(Wand::Fire)] > 0) 				{ return SelectedElement::ItemFire; }
	if (element < SelectedElement::ItemIce && playerStats.items[static_cast<uint8_t>(Wand::Ice)] > 0) 					{ return SelectedElement::ItemIce; }
	if (element < SelectedElement::ItemPoison && playerStats.items[static_cast<uint8_t>(Wand::Poison)] > 0) 		{ return SelectedElement::ItemPoison; }
	if (element < SelectedElement::Action) 																																			{ return SelectedElement::Action; }

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
