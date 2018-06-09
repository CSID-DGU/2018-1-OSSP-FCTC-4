#include "game.h"
#include "animation.h"
#include "board.h"
#include "fps.h"
#include "input.h"
#include "pacman.h"
#include "pellet.h"
#include "physics.h"
#include "renderer.h"
#include "sound.h"
#include "text.h"
#include "window.h"
#include <stdlib.h>
#include <time.h>



static void process_item(PacmanGame *game);
static void process_player(Pacman *pacman, Board *board, Player player);
//static void process_fruit(PacmanGame *game);
static void process_ghosts(PacmanGame *game);
static void process_pellets(PacmanGame *game);
static void process_missiles(PacmanGame *game);
static bool check_pacghost_collision(PacmanGame *game);     //return true if pacman collided with any ghosts
static bool check_ghomissile_collision(PacmanGame *game);
static void enter_state(PacmanGame *game, GameState state); //transitions to/ from a state
static bool resolve_telesquare(PhysicsBody *body);          //wraps the body around if they've gone tele square

static Player death_player;

void game_tick(PacmanGame *game)
{

	Pacman *pac = &game->pacman;
	Pacman *pac2 = &game->pacman_enemy;

	// printf("life: %d / %d\n",game->pacman.livesLeft,game->pacman_enemy.livesLeft);

	unsigned dt = ticks_game() - game->ticksSinceModeChange;

	switch (game->gameState)
	{
		case GameBeginState:
			// plays the sound, has the "player 1" image, has the "READY!" thing

			break;
		case LevelBeginState:
			// similar to game begin mode except no sound, no "Player 1", and slightly shorter duration

			break;
		case GamePlayState:
			// everyone can move and this is the standard 'play' game mode
	
			if(game->pacman.livesLeft != -1) process_player(&game->pacman, &game->board[game->stageLevel], One);
			if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) process_player(&game->pacman_enemy, &game->board[game->stageLevel], Two);
			
			//process_player(&game->pacman, &game->board, One);
			//if(game->mode != SoloState) process_player(&game->pacman_enemy, &game->board, Two);
			
			
			process_ghosts(game);
			
			process_item(game);
			if(game->pacman.missile == 1) process_missiles(game);
			if(game->mode != SoloState && game->pacman_enemy.missile == 1) process_missiles(game);
			
			process_pellets(game);
			
			if (game->pacman.score > game->highscore) game->highscore = game->pacman.score;
			if (game->mode != SoloState && (game->pacman_enemy.score > game->highscore) ) game->highscore = game->pacman_enemy.score;

			break;
		case WinState:
			//pacman eats last pellet, immediately becomes full circle
			//everything stays still for a second or so
			//monsters + pen gate disappear
			//level flashes between normal color and white 4 times
			//screen turns dark (pacman still appears to be there for a second before disappearing)
			//full normal map appears again
			//pellets + ghosts + pacman appear again
			//go to start level mode

			break;
		case DeathState:
			// pacman has been eaten by a ghost and everything stops moving
			// he then does death animation and game starts again

			//everything stops for 1ish second

			//ghosts disappear
			//death animation starts
			//empty screen for half a second

			break;
		case GameoverState:
			// pacman has lost all his lives
			//it displays "game over" briefly, then goes back to main menu
			break;
		case ClearState:

			break;
	}

	//
	// State Transitions - refer to gameflow for descriptions
	//

	bool allPelletsEaten = game->pelletHolder[game->stageLevel].numLeft == 0;
	bool collidedWithGhost = check_pacghost_collision(game);
	bool collidedWithMissile = check_ghomissile_collision(game);
	
	int lives = game->pacman.livesLeft;
	int player2_lives = -1;
	if(game->mode != SoloState) player2_lives = game->pacman_enemy.livesLeft;
	
	switch (game->gameState)
	{
		case GameBeginState:
			if (dt > 2200) enter_state(game, LevelBeginState);

			break;
		case LevelBeginState:
			if (dt > 1800) enter_state(game, GamePlayState);
			game->pacman.godMode = false;
			if(game->mode != SoloState) {
				game->pacman_enemy.godMode = false;
				enter_state(game, GamePlayState);
			}
			break;
		case GamePlayState:

			//TODO: remove this hacks
			if (key_held(SDLK_k)) enter_state(game, DeathState);
			else if (allPelletsEaten) enter_state(game, WinState);
			else if (collidedWithGhost) enter_state(game, DeathState);
			break;
		case WinState:
			//if (transitionLevel) //do transition here
			if (dt > 4000){
				 enter_state(game, LevelBeginState);
				 if(game->currentLevel == 6) enter_state(game,ClearState);
			}
			break;
		case DeathState:
			if (dt > 4000)
			{
				if(game->mode == SoloState && lives == 0) enter_state(game, GameoverState);
				else if(game->mode != SoloState && ((lives == -1 && player2_lives == 0) || (lives == 0 && player2_lives == -1)) ) 
				{ printf("ok\n"); enter_state(game, GameoverState); }
				//if (lives == 0 && death_player == One) enter_state(game, GameoverState);
				//else if (player2_lives == 0 && death_player == Two) enter_state(game, GameoverState);
				else enter_state(game, LevelBeginState);
			}

			break;
		case GameoverState:
			if (dt > 2000)
			{
				//TODO: go back to main menu

			}
			break;
		case ClearState:
			if(dt > 2000)
			{
			}
			break;
	}
}

void game_render(PacmanGame *game, int tick)
{

	unsigned dt = tick - game->ticksSinceModeChange;
	static unsigned godDt = 0;
	static bool godChange = false;
	Pacman *pac = &game->pacman;
	Pacman *pac2 = &game->pacman_enemy;
	//common stuff that is rendered in every mode:
	// 1up + score, highscore, base board, lives, small pellets, fruit indicators
	draw_common_oneup(true, game->pacman.score);
	if(game->mode != SoloState) draw_common_twoup(true, game->pacman_enemy.score);
	
	draw_common_highscore(game->highscore);
	
	draw_pacman_lives(game->pacman.livesLeft);
	if(game->mode != SoloState) draw_pacman2_lives(game->pacman_enemy.livesLeft);
	
	draw_small_pellets(&game->pelletHolder[game->stageLevel]);

	//in gameover state big pellets don't render
	//in gamebegin + levelbegin big pellets don't flash
	//in all other states they flash at normal rate

	switch (game->gameState)
	{
		case GameBeginState:
			draw_game_playerone_start();
			draw_game_ready();

			draw_large_pellets(&game->pelletHolder[game->stageLevel], false);
			draw_board(&game->board[game->stageLevel]);
			break;
		case LevelBeginState:
			draw_game_ready();

			//we also draw pacman and ghosts (they are idle currently though)
			
			if(game->pacman.livesLeft != -1) draw_pacman_static(&game->pacman);
			if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) draw_pacman2_static(&game->pacman_enemy);
			
			//draw_pacman_static(&game->pacman);
			//if(game->mode != SoloState) draw_pacman2_static(&game->pacman_enemy);
			
			for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);
			if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);
			
			draw_large_pellets(&game->pelletHolder[game->stageLevel], false);
			draw_board(&game->board[game->stageLevel]);
			break;
		case GamePlayState:
			//stage 표시
			draw_stage(game->currentLevel);
			draw_large_pellets(&game->pelletHolder[game->stageLevel], true);
			draw_board(&game->board[game->stageLevel]);

			if (game->gameItem1[game->stageLevel].itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem1[game->stageLevel]);
			if (game->gameItem2[game->stageLevel].itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem2[game->stageLevel]);
			if (game->gameItem3[game->stageLevel].itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem3[game->stageLevel]);
			if (game->gameItem4[game->stageLevel].itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem4[game->stageLevel]);
			if (game->gameItem5[game->stageLevel].itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem5[game->stageLevel]);

			if (game->gameItem1[game->stageLevel].eaten && tick - game->gameItem1[game->stageLevel].eatenAt < 2000) draw_item_pts(&game->gameItem1[game->stageLevel]);
			if (game->gameItem2[game->stageLevel].eaten && tick - game->gameItem2[game->stageLevel].eatenAt < 2000) draw_item_pts(&game->gameItem2[game->stageLevel]);
			if (game->gameItem3[game->stageLevel].eaten && tick - game->gameItem3[game->stageLevel].eatenAt < 2000) draw_item_pts(&game->gameItem3[game->stageLevel]);
			if (game->gameItem4[game->stageLevel].eaten && tick - game->gameItem4[game->stageLevel].eatenAt < 2000) draw_item_pts(&game->gameItem4[game->stageLevel]);
			if (game->gameItem5[game->stageLevel].eaten && tick - game->gameItem5[game->stageLevel].eatenAt < 2000) draw_item_pts(&game->gameItem5[game->stageLevel]);			


			if(game->pacman.livesLeft != -1) draw_pacman(&game->pacman);
			
			//draw_pacman(&game->pacman);

			if(game->pacman.godMode == false) {
				for (int i = 0; i < 4; i++) {
					if(game->ghosts[i].isDead == 1) {
						draw_eyes(&game->ghosts[i]);
					} else
						draw_ghost(&game->ghosts[i]);
				}

				if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);

				if(pac->missile == 1)	
					for (int i = 0; i < 2; i++) draw_missile(&game->missiles[i]);
			}

			else {
				if(godChange == false) {
					game->pacman.originDt = tick;
					godChange = true;
				}
				godDt = tick - game->pacman.originDt;
				for (int i = 0; i < 4; i++) {
					if(game->ghosts[i].isDead == 1) {
						draw_eyes(&game->ghosts[i]);
					} 
					else if(draw_scared_ghost(&game->ghosts[i], godDt)){
						// nothing
						if(game->ghosts[i].isDead == 2) {
							draw_ghost(&game->ghosts[i]);
						}
					} 
					else {
						game->pacman.godMode = false;
						godChange = false;
						if(game->ghosts[i].isDead == 2)
							game->ghosts[i].isDead = 0;
					}
				}

				if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);
				if(pac->missile == 1)					
					for (int i = 0; i < 2; i++) draw_missile(&game->missiles[i]);				

			}
			
			if(game->mode != SoloState) {
					pac = &game->pacman_enemy;
					
					if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) draw_pacman2(&game->pacman_enemy);
					//draw_pacman2(&game->pacman_enemy);
			
					if(game->pacman_enemy.godMode == false) {
						for (int i = 0; i < 4; i++) {
							if(game->ghosts[i].isDead == 1) {
								draw_eyes(&game->ghosts[i]);
							} else
								draw_ghost(&game->ghosts[i]);
						}

						if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);

						if(pac->missile == 1)					
							for (int i = 0; i < 2; i++) draw_missile(&game->missiles[i]);				
							
					} else {

						if(godChange == false) {
							game->pacman_enemy.originDt = tick;
							godChange = true;
						}
						godDt = tick - game->pacman_enemy.originDt;
						for (int i = 0; i < 4; i++) {
							if(game->ghosts[i].isDead == 1) {
								draw_eyes(&game->ghosts[i]);
							} 
							else if(draw_scared_ghost(&game->ghosts[i], godDt)){
								// nothing
								if(game->ghosts[i].isDead == 2) {
									draw_ghost(&game->ghosts[i]);
								}
							} 
							else {
								game->pacman_enemy.godMode = false;
								godChange = false;
								if(game->ghosts[i].isDead == 2)
									game->ghosts[i].isDead = 0;
							}
						}

						if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);
						if(pac->missile == 1)					
							for (int i = 0; i < 2; i++) draw_missile(&game->missiles[i]);				

					}
			}
			break;
		case WinState:
			
			if(game->pacman.livesLeft != -1) draw_pacman_static(&game->pacman);
			if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) draw_pacman2_static(&game->pacman_enemy);
			
			//draw_pacman_static(&game->pacman);
			//if(game->mode != SoloState) draw_pacman2_static(&game->pacman_enemy);
			
			
			if (dt < 2000)
			{
				for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);
				if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);
				draw_board(&game->board[game->stageLevel]);
			}
			else
			{
				//stop rendering the pen, and do the flash animation
				draw_board_flash(&game->board[game->stageLevel]);
			}
			
			if(game->currentLevel == 6) {
				enter_state(game,ClearState);
			}

			break;
		case DeathState:
			//draw everything the same for 1ish second
			if (dt < 1000)
			{
				//draw everything normally

				//TODO: this actually draws the last frame pacman was on when he died
				if(game->pacman.livesLeft != -1) draw_pacman_static(&game->pacman);
				if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) draw_pacman2_static(&game->pacman_enemy);
				
				//draw_pacman_static(&game->pacman);
				//if(game->mode != SoloState) draw_pacman2_static(&game->pacman_enemy);

				for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);

				if(game->stageLevel == BOSS_STAGE) draw_ghost(&game->ghosts[4]);

// 				if(pac2->missile2 == 1 || pac->missile == 1)					
// 					for (int i = 0; i < 2; i++) draw_missile(&game->missiles[i]);

			}
			else
			{
				//draw the death animation
				if(game->pacman.livesLeft != -1) {
					if(game->death_player == One) draw_pacman_death(&game->pacman, dt - 1000);
					else draw_pacman_static(&game->pacman);
				}
				if(game->mode != SoloState && game->pacman_enemy.livesLeft != -1) {
					if(game->mode != SoloState && game->death_player == Two) draw_pacman2_death(&game->pacman_enemy, dt - 1000);
					else draw_pacman2_static(&game->pacman_enemy);
				}
				
				
				/*
				if(game->death_player == One) draw_pacman_death(&game->pacman, dt - 1000);
				else draw_pacman_static(&game->pacman);
				if(game->mode != SoloState && game->death_player == Two) draw_pacman2_death(&game->pacman_enemy, dt - 1000);
				else draw_pacman2_static(&game->pacman_enemy);
				*/
			}

			draw_large_pellets(&game->pelletHolder[game->stageLevel], true);
			draw_board(&game->board[game->stageLevel]);
			break;
		case GameoverState:
			draw_game_gameover();
			draw_board(&game->board[game->stageLevel]);
			draw_credits(num_credits());
			break;
		case ClearState:
			draw_game_clear();
			for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);
			process_ghosts(game);
			draw_board(&game->board[game->stageLevel]);
			draw_credits(num_credits());
			break;
	}
}

static void enter_state(PacmanGame *game, GameState state)
{
	//process leaving a state
	switch (game->gameState)
	{
		case GameBeginState:
			game->pacman.livesLeft--;
			if(game->mode != SoloState) game->pacman_enemy.livesLeft--;

			break;
		case WinState:
			game->currentLevel++;
			if(game->stageLevel < STAGE_COUNT -1 ){
				game->stageLevel++;
			}
			game->gameState = LevelBeginState;
			level_init(game);
			
			break;
		case DeathState:
			// Player died and is starting a new game, subtract a life
			if (state == LevelBeginState)
			{
				if(death_player == Two) game->pacman_enemy.livesLeft--;
				else game->pacman.livesLeft--;
				//printf("1: %d / 2: %d\n",game->pacman.livesLeft,game->pacman_enemy.livesLeft);
				pacdeath_init(game);
			}
		case GameoverState:
			
			// gamestart_init(&game, menuSystem.mode);
			break;
		case ClearState:
			break;
		default: ; //do nothing
	}

	//process entering a state
	
	bool allPelletsEaten = game->pelletHolder[game->stageLevel].numLeft == 0;

	switch (state)
	{
		case GameBeginState:
			stop_sound(IntrobgmSound);
			play_sound(LevelStartSound);

			break;
		case LevelBeginState:
			
			break;
		case GamePlayState:
			if(game->stageLevel == 0){
				play_sound(Stage1Sound);
				//if(allPelletsEaten) stop_sound(Stage1Sound);
			}
			if(game->stageLevel == 1){
				play_sound(Stage2Sound);
				//if(allPelletsEaten) stop_sound(Stage2Sound);
			}
			if(game->stageLevel == 2){
				play_sound(Stage3Sound);
				//if(allPelletsEaten) stop_sound(Stage3Sound);
			}
			if(game->stageLevel == 3){
				play_sound(Stage4Sound);
				//if(allPelletsEaten) stop_sound(Stage4Sound);
			}
			if(game->stageLevel == 4){
				play_sound(BossSound);
				//if(allPelletsEaten) stop_sound(Stage4Sound);
			}
			break;
		case WinState:
			stop_sound(Stage1Sound);	
			stop_sound(Stage2Sound);	
			stop_sound(Stage3Sound);	
			stop_sound(Stage4Sound);	
			stop_sound(BossSound);	
			play_sound(GameoverSound);
			break;
		case DeathState:
			play_sound(PacmanDeathSound);
			break;
		case GameoverState:
			play_sound(GameoverSound);
			printf("GameoverState\n");
			game->currentLevel = 1;
			game->stageLevel = 0;
			//invalidate the state so it doesn't effect the enter_state function
			game->gameState = -1;
			level_init(game);
			break;
		case ClearState:
			//stop_sound(BossSound);
			play_sound(EndingbgmSound);
			break;
	}

	game->ticksSinceModeChange = ticks_game();
	game->gameState = state;
}

//checks if it's valid that pacman could move in this direction at this point in time
bool can_move(Pacman *pacman, Board *board, Direction dir)
{
	//easy edge cases, tile has to be parallal with a direction to move it
	if ((dir == Up   || dir == Down ) && !on_vert(&pacman->body)) return false;
	if ((dir == Left || dir == Right) && !on_horo(&pacman->body)) return false;

	//if pacman wants to move on an axis and he is already partially on that axis (not 0)
	//it is always a valid move
	if ((dir == Left || dir == Right) && !on_vert(&pacman->body)) return true;
	if ((dir == Up   || dir == Down ) && !on_horo(&pacman->body)) return true;

	//pacman is at 0/0 and moving in the requested direction depends on if there is a valid tile there
	int x = 0;
	int y = 0;
	
	dir_xy(dir, &x, &y);	

	int newX = pacman->body.x + x;
	int newY = pacman->body.y + y;

	return is_valid_square(board, newX, newY) || is_tele_square(newX, newY);
}

static void process_player(Pacman *pacman, Board *board, Player player)
{
	
	if (pacman->missedFrames != 0)
	{
		pacman->missedFrames--;
		return;
	}

	Direction oldLastAttemptedDir = pacman->lastAttemptedMoveDirection;
	
	Direction newDir;

	bool dirPressed;
	if(player == One) {
		dirPressed = dir_pressed_now(&newDir);
	}
	else {
		dirPressed = dir_pressed_now_player2(&newDir);
	}
	// 눌려진 버튼에따라 움직이게함
	//if(!player) dirPressed = false;
	
	if (dirPressed)
	{
		//user wants to move in a direction
		pacman->lastAttemptedMoveDirection = newDir;

		//if player holds opposite direction to current walking dir
		//we can always just switch current walking direction
		//since we're on parallel line
		if (newDir == dir_opposite(pacman->body.curDir))
		{
			pacman->body.curDir = newDir;
			pacman->body.nextDir = newDir;
		}

		//if pacman was stuck before just set his current direction as pressed
		if (pacman->movementType == Stuck)
		{
			pacman->body.curDir = newDir;
		}

		pacman->body.nextDir = newDir;
	}
	else if (pacman->movementType == Stuck)
	{
		//pacman is stuck and player didn't move - player should still be stuck.
		//don't do anything
		return;
	}
	else
	{
		//user doesn't want to change direction and pacman isn't stuck
		//pacman can move like normal

		//just set the next dir to current dir
		pacman->body.nextDir = pacman->body.curDir;
	}

	pacman->movementType = Unstuck;

	int curDirX = 0;
	int curDirY = 0;
	int nextDirX = 0;
	int nextDirY = 0;

		dir_xy(pacman->body.curDir, &curDirX, &curDirY);
		dir_xy(pacman->body.nextDir, &nextDirX, &nextDirY);
	
	int newCurX = pacman->body.x + curDirX;
	int newCurY = pacman->body.y + curDirY;
	int newNextX = pacman->body.x + nextDirX;
	int newNextY = pacman->body.y + nextDirY;

	bool canMoveCur =  is_valid_square(board, newCurX, newCurY) || is_tele_square(newCurX, newCurY);
	bool canMoveNext = is_valid_square(board, newNextX, newNextY) || is_tele_square(newNextX, newNextY);

	//if pacman is currently on a center tile and can't move in either direction
	//don't move him
	if (on_center(&pacman->body) && !canMoveCur && !canMoveNext)
	{
		pacman->movementType = Stuck;
		pacman->lastAttemptedMoveDirection = oldLastAttemptedDir;

		return;
	}

	move_pacman(&pacman->body, canMoveCur, canMoveNext);

	//if pacman is on the center, and he couldn't move either of  his last directions
	//he must be stuck now
	if (on_center(&pacman->body) && !canMoveCur && !canMoveNext)
	{
		pacman->movementType = Stuck;
		return;
	}

	resolve_telesquare(&pacman->body);
}

static void process_ghosts(PacmanGame *game)
{
	int numOfGhosts = 4;
	if(game->stageLevel == BOSS_STAGE) numOfGhosts = 5;
	
	for (int i = 0; i < numOfGhosts; i++)
	{
		Ghost *g = &game->ghosts[i];

		if (g->movementMode == InPen)
		{
			//ghosts bob up and down - move in direction. If they hit a square, change direction
			bool moved = move_ghost(&g->body);

			if (moved && (g->body.y == 13 || g->body.y == 15))
			{
				g->body.nextDir = g->body.curDir;
				g->body.curDir = dir_opposite(g->body.curDir);
			}

			continue;
		}

		if (g->movementMode == LeavingPen)
		{
			//ghost is in center of tile
			//move em to the center of the pen (in x axis)
			//then more em up out the gate
			//when they are out of the gate, set them to be in normal chase mode then set them off on their way

			continue;
		}

		//all other modes can move normally (I think)
		MovementResult result = move_ghost(&g->body);
		resolve_telesquare(&g->body);

		if (result == NewSquare)
		{
			//if they are in a new tile, rerun their target update logic
			// execute ghost AI logic according to currentLeve
			execute_ghost_logic(game->currentLevel, g, g->ghostType, &game->ghosts[0], &game->pacman);

			g->nextDirection = next_direction(g, &game->board[game->stageLevel]);
		}
		else if (result == OverCenter)
		{
			//they've hit the center of a tile, so change their current direction to the new direction
			g->body.curDir = g->transDirection;
			g->body.nextDir = g->nextDirection;
			g->transDirection = g->nextDirection;
		}
	}
}

static void process_missiles(PacmanGame *game)
{
	for (int i = 0; i < 2; i++)
	{
		Missile *m = &game->missiles[i];

		if (m->m_movementMode == InPen)
		{
			//ghosts bob up and down - move in direction. If they hit a square, change direction
			bool moved = move_missile(&m->body);

			if (moved && (m->body.y == 13 || m->body.y == 15))
			{
				m->body.nextDir = m->body.curDir;
				m->body.curDir = dir_opposite(m->body.curDir);
			}

			continue;
		}

		if (m->m_movementMode == LeavingPen)
		{
			//ghost is in center of tile
			//move em to the center of the pen (in x axis)
			//then more em up out the gate
			//when they are out of the gate, set them to be in normal chase mode then set them off on their way

			continue;
		}

		//all other modes can move normally (I think)
		MovementResult result = move_missile(&m->body);
		resolve_telesquare(&m->body);

		if (result == NewSquare)
		{
			//if they are in a new tile, rerun their target update logic
			// execute ghost AI logic according to currentLeve
			execute_missile_logic(game->currentLevel, m, m->missileType, &game->missiles[0], &game->ghosts[0]);

			m->nextDirection = next_direction(m, &game->board[game->stageLevel]);
		}
		else if (result == OverCenter)
		{
			//they've hit the center of a tile, so change their current direction to the new direction
			m->body.curDir = m->transDirection;
			m->body.nextDir = m->nextDirection;
			m->transDirection = m->nextDirection;
		}
	}
}




static void process_item(PacmanGame *game)
{
	int pelletsEaten = game->pelletHolder[game->stageLevel].totalNum - game->pelletHolder[game->stageLevel].numLeft;

	GameItem *f1 = &game->gameItem1[game->stageLevel];
	GameItem *f2 = &game->gameItem2[game->stageLevel];
	GameItem *f3 = &game->gameItem3[game->stageLevel];
	GameItem *f4 = &game->gameItem4[game->stageLevel];
	GameItem *f5 = &game->gameItem5[game->stageLevel];

	int curLvl = game->currentLevel;

	if (pelletsEaten >= 20 && f1->itemMode == NotDisplaying)
	{
		f1->itemMode = Displaying;
		regen_item(f1, curLvl);
	}
	else if (pelletsEaten == 40 && f2->itemMode == NotDisplaying)
	{
		f2->itemMode = Displaying;
		regen_item(f2, curLvl);
	}
	else if (pelletsEaten == 60 && f3->itemMode == NotDisplaying)
	{
		f3->itemMode = Displaying;
		regen_item(f3, curLvl);
	}
	else if (pelletsEaten == 80 && f4->itemMode == NotDisplaying)
	{
		f4->itemMode = Displaying;
		regen_item(f4, curLvl);
	}
	else if (pelletsEaten == 100 && f5->itemMode == NotDisplaying)
	{
		f5->itemMode = Displaying;
		regen_item(f5, curLvl);
	}

	unsigned int f1dt = ticks_game() - f1->startedAt;
	unsigned int f2dt = ticks_game() - f2->startedAt;
	unsigned int f3dt = ticks_game() - f3->startedAt;
	unsigned int f4dt = ticks_game() - f4->startedAt;
	unsigned int f5dt = ticks_game() - f5->startedAt;

	Pacman *pac = &game->pacman;
	
	if(pac->itemRemainTime != 0) pac->itemRemainTime--;
	else {
		pac->body.velocity = 80;
		pac->itemRemainTime = 0;
		pac->protect = 0;
		pac->itemOn = false;
		pac->missile = 0;
		missiles_init(game->missiles);
	}
	
	if (f1->itemMode == Displaying)
	{
		if (f1dt > f1->displayTime) f1->itemMode = Displayed;
	}
	if (f2->itemMode == Displaying)
	{
		if (f2dt > f2->displayTime) f2->itemMode = Displayed;
	}
	if (f3->itemMode == Displaying)
		{
			if (f3dt > f3->displayTime) f3->itemMode = Displayed;
		}
	if (f4->itemMode == Displaying)
		{
			if (f4dt > f4->displayTime) f4->itemMode = Displayed;
		}
	if (f5->itemMode == Displaying)
		{
			if (f5dt > f5->displayTime) f5->itemMode = Displayed;
		}

	//check for collisions


	f1->item = Life;
	f2->item = Move_Fast;
	f3->item = Move_Slow;
	f4->item = Ghost_mode;
	f5->item = Tornado;
	
	
	if (f1->itemMode == Displaying && collides_obj(&pac->body, f1->x, f1->y))
	{
		f1->itemMode = Displayed;
		f1->eaten = true;
		f1->eatenAt = ticks_game();
		pac->score += item_points(f1->item);
		
		if(f1->item==Life) {
			pac->livesLeft += 1;		
            play_sound(BonusSound);
		}
		
		if(f1->item==Move_Fast) {
			pac->body.velocity = 120;
			pac->itemRemainTime = 150;
            pac->itemOn = true;			
            play_sound(BoosterSound);			
		}
		
		if(f1->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            pac->itemOn = true;			
            play_sound(Munch_Bsound);			
		}
		
		if(f1->item==Prof) {
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);			
			}
		}
		if(f1->item==Tornado) {
			pac->itemOn = true;
			pac->missile = 1;
			pac->itemRemainTime = 400;
			play_sound(Chomp2Sound);
		}
		
		if(f1->item==Ghost_mode) {
            pac->itemOn = true;			
			pac->protect = 1;
			pac->itemRemainTime = 150;
			play_sound(CutsceneSound);
		}
	}

	if (f2->itemMode == Displaying && collides_obj(&pac->body, f2->x, f2->y))
	{
		f2->itemMode = Displayed;
		f2->eaten = true;
		f2->eatenAt = ticks_game();
		pac->score += item_points(f2->item);
		
		if(f2->item==Life) {
			pac->livesLeft += 1;
            play_sound(BonusSound);				
		}
		
		if(f2->item==Move_Fast) {
			pac->body.velocity = 120;
			pac->itemRemainTime = 150;
            pac->itemOn = true;			
            play_sound(BoosterSound);	
		}
		
		if(f2->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            pac->itemOn = true;			
            play_sound(Munch_Bsound);			
		}
		
		if(f2->item==Prof) {
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);			
			}
		}
		if(f2->item==Tornado) {
			pac->itemOn = true;
			pac->missile = 1;
			pac->itemRemainTime = 400;
			play_sound(Chomp2Sound);
		}		
		
		if(f2->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
            pac->itemOn = true;
		play_sound(CutsceneSound);			
		}		
	}

	if (f3->itemMode == Displaying && collides_obj(&pac->body, f3->x, f3->y))
	{
		f3->itemMode = Displayed;
		f3->eaten = true;
		f3->eatenAt = ticks_game();
		pac->score += item_points(f3->item);
		
		if(f3->item==Life) {
			pac->livesLeft += 1;
            play_sound(BonusSound);			
		}
		
		if(f3->item==Move_Fast) {
			pac->body.velocity = 120;
			pac->itemRemainTime = 150;
            pac->itemOn = true;
            play_sound(BoosterSound);			
		}
		
		if(f3->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
            pac->itemOn = true;
		}
		
		if(f3->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
			}
		}
		
		if(f3->item==Tornado) {
			pac->itemOn = true;
			pac->missile = 1;
			pac->itemRemainTime = 400;
			play_sound(Chomp2Sound);
		}	
		
		if(f3->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
            pac->itemOn = true;			
		play_sound(CutsceneSound);
		}				
	}
	
	if (f4->itemMode == Displaying && collides_obj(&pac->body, f4->x, f4->y))
	{
		f4->itemMode = Displayed;
		f4->eaten = true;
		f4->eatenAt = ticks_game();
		pac->score += item_points(f4->item);
		
		if(f4->item==Life) {
			pac->livesLeft += 1;
            play_sound(BonusSound);			
		}
			
		if(f4->item==Move_Fast) {
			pac->body.velocity = 120;
			pac->itemRemainTime = 150;
            play_sound(BoosterSound);
            pac->itemOn = true;            			
		}
		
		if(f4->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
            pac->itemOn = true;
		}
		
		if(f4->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
			}
		}
		
		if(f4->item==Tornado) {
			pac->itemOn = true;
			pac->missile = 1;
			pac->itemRemainTime = 400;
			play_sound(Chomp2Sound);
		}
		
		if(f4->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
            pac->itemOn = true;
		play_sound(CutsceneSound);
		}					
	}
	
	if (f5->itemMode == Displaying && collides_obj(&pac->body, f5->x, f5->y))
	{
		f5->itemMode = Displayed;
		f5->eaten = true;
		f5->eatenAt = ticks_game();
		pac->score += item_points(f5->item);
		
		if(f5->item==Life) {
			pac->livesLeft += 1;
            play_sound(BonusSound);			
		}
			
		if(f5->item==Move_Fast) {
			pac->body.velocity = 120;
			pac->itemRemainTime = 150;
            play_sound(BoosterSound);			
            pac->itemOn = true;
		}
		
		if(f5->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
            pac->itemOn = true;
		}
		
		if(f5->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
			}
		}
	
		if(f5->item==Tornado) {
			pac->itemOn = true;
			pac->missile = 1;
			pac->itemRemainTime = 400;
			play_sound(Chomp2Sound);
		}
		
		if(f5->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
            pac->itemOn = true;		
		play_sound(CutsceneSound);	
		}			
	}
	
		if(game->mode != SoloState) {
			Pacman *pac2 = &game->pacman_enemy;
		
		if(pac2->itemRemainTime != 0) pac2->itemRemainTime--;
		else {
			pac2->body.velocity = 80;
			pac2->itemRemainTime = 0;
			pac2->itemOn = false;
			pac2->protect = 0;
			pac2->missile2 = 0;
			missiles_init(game->missiles);		
		}		

		
		if (f1->itemMode == Displaying && collides_obj(&pac2->body, f1->x, f1->y))
		{
			f1->itemMode = Displayed;
			f1->eaten = true;
			f1->eatenAt = ticks_game();
			pac2->score += item_points(f1->item);
			
			if(f1->item==Life) {
				pac2->livesLeft += 1;
				play_sound(BonusSound);
			}
			if(f1->item==Move_Fast) {
				pac2->body.velocity = 120;
				pac2->itemRemainTime = 150;			
				pac2->itemOn = true;
				play_sound(BoosterSound);
			}
			
			if(f1->item==Move_Slow) {
				pac2->body.velocity = 60;
				pac2->itemRemainTime = 150;			
	            pac2->itemOn = true;
	            play_sound(Munch_Bsound);
			}	
				
			if(f1->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
			}
		}

			if(f1->item==Tornado) {
			pac2->itemOn = true;
			pac2->missile2 = 1;
			pac2->itemRemainTime = 400;
			play_sound(Chomp2Sound);
			}
			
			if(f1->item==Ghost_mode) {
			pac2->protect = 1;
			pac2->itemRemainTime = 150;
            pac2->itemOn = true;	
            play_sound(CutsceneSound);
		}
	}

		if (f2->itemMode == Displaying && collides_obj(&pac2->body, f2->x, f2->y))
		{
			f2->itemMode = Displayed;
			f2->eaten = true;
			f2->eatenAt = ticks_game();
			pac2->score += item_points(f2->item);
				
			if(f1->item==Life) {
				pac2->livesLeft += 1;
				play_sound(BonusSound);
			}
			if(f2->item==Move_Fast) {
				pac2->body.velocity = 120;
				pac2->itemRemainTime = 150;			
				pac2->itemOn = true;
				play_sound(BoosterSound);
			}
			
			if(f2->item==Move_Slow) {
				pac2->body.velocity = 60;
				pac2->itemRemainTime = 150;			
	            pac2->itemOn = true;
	            play_sound(Munch_Bsound);
			}	
				
			if(f2->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
				}
			}

			if(f2->item==Tornado) {
			pac2->itemOn = true;
			pac2->missile2 = 1;
			pac2->itemRemainTime = 400;
			play_sound(Chomp2Sound);
			}
			
			if(f2->item==Ghost_mode) {
			pac2->protect = 1;
			pac2->itemRemainTime = 150;
            pac2->itemOn = true;	
            play_sound(CutsceneSound);
			}
		}					
		

		if (f3->itemMode == Displaying && collides_obj(&pac2->body, f3->x, f3->y))
		{
			f3->itemMode = Displayed;
			f3->eaten = true;
			f3->eatenAt = ticks_game();
			pac2->score += item_points(f3->item);
				
			if(f3->item==Life) {
				pac2->livesLeft += 1;
				play_sound(BonusSound);
			}
			if(f3->item==Move_Fast) {
				pac2->body.velocity = 120;
				pac2->itemRemainTime = 150;			
				pac2->itemOn = true;
				play_sound(BoosterSound);
			}
			
			if(f3->item==Move_Slow) {
				pac2->body.velocity = 60;
				pac2->itemRemainTime = 150;			
	            pac2->itemOn = true;
	            play_sound(Munch_Bsound);
			}	
				
			if(f3->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
				}
			}

			if(f3->item==Tornado) {
			pac2->itemOn = true;
			pac2->missile2 = 1;
			pac2->itemRemainTime = 400;
			play_sound(Chomp2Sound);
			}
			
			if(f3->item==Ghost_mode) {
			pac2->protect = 1;
			pac2->itemRemainTime = 150;
            pac2->itemOn = true;	
            play_sound(CutsceneSound);
			}	
		}
		
		if (f4->itemMode == Displaying && collides_obj(&pac2->body, f4->x, f4->y))
		{
			f4->itemMode = Displayed;
			f4->eaten = true;
			f4->eatenAt = ticks_game();
			pac2->score += item_points(f4->item);
					
			if(f4->item==Life) {
				pac2->livesLeft += 1;
				play_sound(BonusSound);
			}
			if(f4->item==Move_Fast) {
				pac2->body.velocity = 120;
				pac2->itemRemainTime = 150;			
				pac2->itemOn = true;
				play_sound(BoosterSound);
			}
			
			if(f4->item==Move_Slow) {
				pac2->body.velocity = 60;
				pac2->itemRemainTime = 150;			
	            pac2->itemOn = true;
	            play_sound(Munch_Bsound);
			}	
				
			if(f4->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
				}
			}

			if(f4->item==Tornado) {
			pac2->itemOn = true;
			pac2->missile2 = 1;
			pac2->itemRemainTime = 400;
			play_sound(Chomp2Sound);
			}
			
			if(f4->item==Ghost_mode) {
			pac2->protect = 1;
			pac2->itemRemainTime = 150;
            pac2->itemOn = true;	
            play_sound(CutsceneSound);
			}	
		}
		
		if (f5->itemMode == Displaying && collides_obj(&pac2->body, f5->x, f5->y))
		{
			f5->itemMode = Displayed;
			f5->eaten = true;
			f5->eatenAt = ticks_game();
			pac2->score += item_points(f5->item);
			
			if(f5->item==Life) {
				pac2->livesLeft += 1;
				play_sound(BonusSound);
			}
			if(f5->item==Move_Fast) {
				pac2->body.velocity = 120;
				pac2->itemRemainTime = 150;			
				pac2->itemOn = true;
				play_sound(BoosterSound);
			}
			
			if(f5->item==Move_Slow) {
				pac2->body.velocity = 60;
				pac2->itemRemainTime = 150;			
	            pac2->itemOn = true;
	            play_sound(Munch_Bsound);
			}	
				
			if(f5->item==Prof){
			for (int i = 0; i < 4; i++) {
				game->ghosts[i].body.velocity = 1;
				game->ghosts[i].remain = 1000;
				play_sound(Chomp1Sound);
				}
			}

			if(f5->item==Tornado) {
			pac2->itemOn = true;
			pac2->missile2 = 1;
			pac2->itemRemainTime = 400;
			play_sound(Chomp2Sound);
			}
			
			if(f5->item==Ghost_mode) {
			pac2->protect = 1;
			pac2->itemRemainTime = 150;
            pac2->itemOn = true;	
            play_sound(CutsceneSound);
			}
		}
	}
}

static void process_pellets(PacmanGame *game)
{
	int j = 0;
	//if pacman and pellet collide
	//give pacman that many points
	//set pellet to not be active
	//decrease num of alive pellets
	
	PelletHolder *holder = &game->pelletHolder[game->stageLevel];
	

	for (int i = 0; i < holder->totalNum; i++)
	{
		Pellet *p = &holder->pellets[i];

		//skip if we've eaten this one already
		if (p->eaten) continue;
		bool flag = false;
		if (collides_obj(&game->pacman.body, p->x, p->y))
		{
			flag = true;
			holder->numLeft--;
			// printf("numLeft : %d\n", holder->numLeft);

			p->eaten = true;
			game->pacman.score += pellet_points(p);
			if(pellet_check(p)) {
				game->pacman.godMode = true;
				game->pacman.originDt = ticks_game();
				game->pacman_enemy.godMode = true;
				game->pacman_enemy.originDt = ticks_game();
				for(j = 0; j< 4; j++) {
					if(game->ghosts[j].isDead == 2)
						game->ghosts[j].isDead = 0;
				}
			}

			//play eat sound
			play_sound(SmallSound);
			//eating a small pellet makes pacman not move for 1 frame
			//eating a large pellet makes pacman not move for 3 frames
			game->pacman.missedFrames = pellet_nop_frames(p);
			game->pacman_enemy.missedFrames = pellet_nop_frames(p);
			//can only ever eat 1 pellet in a frame, so return
			// return;
		}
		if (collides_obj(&game->pacman_enemy.body, p->x, p->y) && flag == false)
		{

			holder->numLeft--;
			// printf("numLeft : %d\n", holder->numLeft);
			p->eaten = true;
			game->pacman_enemy.score += pellet_points(p);
			if(pellet_check(p)) {
				game->pacman.godMode = true;
				game->pacman.originDt = ticks_game();
				game->pacman_enemy.godMode = true;
				game->pacman_enemy.originDt = ticks_game();
				for(j = 0; j< 4; j++) {
					if(game->ghosts[j].isDead == 2)
						game->ghosts[j].isDead = 0;
				}
			}

			//play eat sound
			
			play_sound(SmallSound);
			//eating a small pellet makes pacman not move for 1 frame
			//eating a large pellet makes pacman not move for 3 frames
			game->pacman.missedFrames = pellet_nop_frames(p);
			game->pacman_enemy.missedFrames = pellet_nop_frames(p);

			//can only ever eat 1 pellet in a frame, so return
			// return;
		}
	}
	
	//maybe next time, poor pacman
}

static bool check_pacghost_collision(PacmanGame *game)
{
	int numOfGhosts = 4;
	if(game->stageLevel == BOSS_STAGE) numOfGhosts = 5;
	for (int i = 0; i < numOfGhosts; i++)
	{
		Ghost *g = &game->ghosts[i];
		Pacman *pac = &game->pacman;
		
		
		// switch(g->ghostType) {
		// case Blinky : printf("red : %d \n", g->isDead); break;
		// case Inky   : printf("blue : %d \n", g->isDead); break;
		// case Clyde  : printf("orange : %d \n", g->isDead); break;
		// case Pinky  : printf("pink : %d \n", g->isDead); break;
		// case Boss  : printf("boss : %d \n", g->isDead); break;
		// }
		

		
		if(pac->protect == 0 && pac->livesLeft != -1) {
			if (collides(&game->pacman.body, &g->body)) {
				if(game->pacman.godMode == false){
					death_player = One;
					game->death_player = One;
					return true;
				}
				else {
					if(g->isDead == 2) { death_player = One; return true;}
					play_sound(SirenSound);
					g->isDead = 1;
					death_send(g);
				}
			}

		}
		

		pac = &game->pacman_enemy;
		if(pac->protect == 0 && pac->livesLeft != -1) {
			if(game->mode != SoloState){
				if (collides(&game->pacman_enemy.body, &g->body)) {
					if(game->pacman_enemy.godMode == false){
						death_player = Two;
						game->death_player = Two;
						return true;
					}
					else {
						if(g->isDead == 2) { death_player = Two; return true;}
						g->isDead = 1;
						death_send(g);
					}
				}
			}
		}
	}
	return false;
}

static bool check_ghomissile_collision(PacmanGame *game)
{
	for (int i = 0; i < 2; i++)
	{
		for(int j= 0; j<4; j++) {
			Missile *m = &game->missiles[i];
			Ghost *g = &game->ghosts[j];
		
		/*
		switch(g->ghostType) {
		case Blinky : printf("red : %d \n", g->isDead); break;
		case Inky   : printf("blue : %d \n", g->isDead); break;
		case Clyde  : printf("orange : %d \n", g->isDead); break;
		case Pinky  : printf("pink : %d \n", g->isDead); break;
		}
		*/
		
		if (collides(&game->ghosts[j].body, &m->body)) {
			game->ghosts[j].body.velocity = 0;
			game->ghosts[j].remain = 150;
			game->missiles[i].isDead = 1;
		}
		if (game->ghosts[j].remain != 0) game->ghosts[j].remain--;
		else {
			game->ghosts[j].body.velocity = 70;
			game->ghosts[j].remain = 0;
		}
	}
}
	return false;
}

void gamestart_init(PacmanGame *game, int mode)
{
	printf("gamestart_init : %d\n", mode);
	// play mode 저장
	if(mode == SoloState) game->mode = SoloState;
	else if(mode == MultiState) game->mode = MultiState;
	else game->mode = RemoteState;
	
	level_init(game);

	// mode가 1(multi) 라면 상대방 팩맨도 생성한다.
	pacman_init(&game->pacman);
	if(game->mode != SoloState) pacman_init(&game->pacman_enemy);
	
	//we need to reset all fruit
	//fuit_init();
	game->highscore = 0; //TODO maybe load this in from a file..?
	game->currentLevel = 1;
	game->stageLevel = 0;

	//invalidate the state so it doesn't effect the enter_state function
	game->gameState = -1;
	enter_state(game, GameBeginState);
}

void level_init(PacmanGame *game)
{
	//reset pacmans position
	pacman_level_init(&game->pacman);
	if(game->mode != SoloState) pacman_level_init(&game->pacman_enemy);
	
	//reset pellets
	printf("level_init : %d\n", game->stageLevel);
	pellets_init(&game->pelletHolder[game->stageLevel]);
	missiles_init(game->missiles);
	//reset ghosts
	ghosts_init(game->ghosts);

	//reset fruit
	reset_item(&game->gameItem1[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem2[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem3[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem4[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem5[game->stageLevel], &game->board[game->stageLevel]);

}

void pacdeath_init(PacmanGame *game)
{
	pacman_level_init(&game->pacman);
	if(game->mode != SoloState) pacman_level_init(&game->pacman_enemy);
	ghosts_init(game->ghosts);
	missiles_init(game->missiles);
	reset_item(&game->gameItem1[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem2[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem3[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem4[game->stageLevel], &game->board[game->stageLevel]);
	reset_item(&game->gameItem5[game->stageLevel], &game->board[game->stageLevel]);

}

//TODO: make this method based on a state, not a conditional
//or make the menu system the same. Just make it consistant
bool is_game_over(PacmanGame *game, int tick)
{
	//unsigned dt = ticks_game() - game->ticksSinceModeChange;
	unsigned dt = tick - game->ticksSinceModeChange;
	
	return dt > 2000 && game->gameState == GameoverState;
}

int int_length(int x)
{
    if (x >= 1000000000) return 10;
    if (x >= 100000000)  return 9;
    if (x >= 10000000)   return 8;
    if (x >= 1000000)    return 7;
    if (x >= 100000)     return 6;
    if (x >= 10000)      return 5;
    if (x >= 1000)       return 4;
    if (x >= 100)        return 3;
    if (x >= 10)         return 2;
    return 1;
}

static bool resolve_telesquare(PhysicsBody *body)
{
	//TODO: chuck this back in the board class somehow

	if (body->y != 14) return false;

	if (body->x == -1) { body->x = 27; return true; }
	if (body->x == 28) { body->x =  0; return true; }

	return false;
}
