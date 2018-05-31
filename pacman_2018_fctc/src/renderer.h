#pragma once

#include <stdbool.h>

#include "board.h"
#include "item.h"
#include "ghost.h"
#include "pacman.h"
#include "pellet.h"
#include "text.h"
#include "missile.h"

//Holds data relating to a single rendered ghost-row.
typedef struct
{
	GhostType  type;
	const char *description;
	const char *name;
	TextColor color;
} GhostDisplayRow;

//Class handles the drawing of all entities in the game

//
// Vanity screen renderering
//

void draw_vanity_charnickname(void);
void draw_vanity_text(const char *text, int x, int y);
void draw_vanity_ghostline(GhostDisplayRow *row, int y, bool description, bool name);
void draw_vanity_corporate_info(void);
void draw_vanity_pellet_info(bool flashing);
void draw_vanity_animation(int dt);

//
// Instructions screen renderering
//

void draw_instrc_info(void);
void draw_instrc_corporate_info(void);

//
// Common menu rendering
//

void draw_common_oneup(bool flashing, int score);
void draw_common_twoup(bool flashing, int score);
void draw_common_highscore(int highscore);

void draw_stage(int curLvl);

void draw_common_indicator(int mode, int x, int y);

void draw_credits(int numCredits);

//
// Game-only word rendering
//

void draw_game_playerone_start(void);
void draw_game_playertwo_start(void);
void draw_game_ready(void);
void draw_game_gameover(void);

//
// Fruit renderering
//

void draw_item_indicators(int currentLevel);
void draw_item_game(int currentLevel, GameItem *gameItem);

//
// Pellet renderering
//

void draw_pellet_small(Pellet *p);
void draw_pellet_power(Pellet *p, bool flashing);

void draw_small_pellets(PelletHolder *ph);
void draw_large_pellets(PelletHolder *ph, bool flashing);

//
// Pacman renderering
//

void draw_pacman(Pacman *pacman);
void draw_pacman_static(Pacman *pacman);
void draw_pacman_death(Pacman *pacman, unsigned int timeSinceDeath);
void draw_pacman_lives(int numLives);

//
// Pacman2 renderering
//

void draw_pacman2(Pacman *pacman);
void draw_pacman2_static(Pacman *pacman);
void draw_pacman2_death(Pacman *pacman, unsigned int timeSinceDeath);
void draw_pacman2_lives(int numLives);

//
// Ghost rendering
//

void draw_ghost(Ghost *ghost);
bool draw_scared_ghost(Ghost *ghost, unsigned int timeGhostScared);
void draw_eyes(Ghost *ghost);

void draw_missile(Missile *missile);
//
// Points rendering
//

void draw_item_pts(GameItem *gameItem);

//
// Board rendering
//

void draw_board(Board *board);
void draw_board_flash(Board *board);
