#pragma once

#include "main.h"
#include "board.h"
#include "item.h"
#include "ghost.h"
#include "pacman.h"
#include "pellet.h"

typedef enum
{
	GameBeginState,
	LevelBeginState,
	GamePlayState,
	WinState,
	DeathState,
	GameoverState
} GameState;

typedef enum
{
	One,
	Two
} Player;

//what you do in current state

//what causes you to transition from state A -> state B

//elevator stationary
	//action: do nothing
	//transition
		//-> moving up (someone presses a button upstairs)
		//-> moving down (someone presses a button downstairs)
		//-> door opening (someone presses button on current level)
		//-> go down/ up (timeout on a specific level)

typedef struct
{
	GameState gameState;
	unsigned int ticksSinceModeChange;
	Pacman pacman;
	Pacman pacman_enemy;
	Ghost ghosts[4];
	Board board[STAGE_COUNT];
	PelletHolder pelletHolder[STAGE_COUNT];
	GameItem gameItem1, gameItem2, gameItem3, gameItem4, gameItem5;
	int highscore;
	int currentLevel;

	int stageLevel; // it means stage level
	ModeState mode;
	RemoteRole role;
} PacmanGame;

//Updates the game 1 tick, or 1/60th of a second.
void game_tick(PacmanGame *game);

//Renders the game in its current state.
void game_render(PacmanGame *game);

//Returns true if the game is finished and is ready to hand back to the menu system.
bool is_game_over(PacmanGame *game);

//Call this at start of level 1 to initialize all entities and game objects.
void gamestart_init(PacmanGame *game, int mode);

//Call this at the start of each level to reinitialize all entities to a default state.
void level_init(PacmanGame *game);

//Call after pacman dies to reinitialize entities to continue the same level.
void pacdeath_init(PacmanGame *game);

//Returns the length of the given integer.
int int_length(int num);
