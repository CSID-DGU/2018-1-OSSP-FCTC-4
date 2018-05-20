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
static void process_fruit(PacmanGame *game);
static void process_ghosts(PacmanGame *game);
static void process_pellets(PacmanGame *game);

static bool check_pacghost_collision(PacmanGame *game);     //return true if pacman collided with any ghosts
static void enter_state(PacmanGame *game, GameState state); //transitions to/ from a state
static bool resolve_telesquare(PhysicsBody *body);          //wraps the body around if they've gone tele square

static Player death_player;

void game_tick(PacmanGame *game)
{
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
			process_player(&game->pacman, &game->board, One);
			if(game->mode == MultiState) process_player(&game->pacman_enemy, &game->board, Two);
			process_ghosts(game);

			process_item(game);
			process_pellets(game);
			
			if (game->pacman.score > game->highscore) game->highscore = game->pacman.score;
			if (game->mode == MultiState && (game->pacman_enemy.score > game->highscore) ) game->highscore = game->pacman_enemy.score;

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
	}

	//
	// State Transitions - refer to gameflow for descriptions
	//

	bool allPelletsEaten = game->pelletHolder.numLeft == 0;
	bool collidedWithGhost = check_pacghost_collision(game);
	
	int lives = game->pacman.livesLeft;
	int player2_lives = -1;
	if(game->mode == MultiState) player2_lives = game->pacman_enemy.livesLeft;
	
	switch (game->gameState)
	{
		case GameBeginState:
			if (dt > 2200) enter_state(game, LevelBeginState);

			break;
		case LevelBeginState:
			if (dt > 1800) enter_state(game, GamePlayState);
			game->pacman.godMode = false;
			if(game->mode == MultiState) game->pacman_enemy.godMode = false;

			break;
		case GamePlayState:

			//TODO: remove this hacks
			if (key_held(SDLK_k)) enter_state(game, DeathState);

			else if (allPelletsEaten) enter_state(game, WinState);
			else if (collidedWithGhost) enter_state(game, DeathState);

			break;
		case WinState:
			//if (transitionLevel) //do transition here
			if (dt > 4000) enter_state(game, LevelBeginState);

			break;
		case DeathState:
			if (dt > 4000)
			{
				if (lives == 0 || player2_lives == 0) enter_state(game, GameoverState);
				else enter_state(game, LevelBeginState);
			}

			break;
		case GameoverState:
			if (dt > 2000)
			{
				//TODO: go back to main menu

			}
			break;
	}
}

void game_render(PacmanGame *game)
{
	unsigned dt = ticks_game() - game->ticksSinceModeChange;
	static unsigned godDt = 0;
	static bool godChange = false;

	//common stuff that is rendered in every mode:
	// 1up + score, highscore, base board, lives, small pellets, fruit indicators
	draw_common_oneup(true, game->pacman.score);
	if(game->mode == MultiState) draw_common_twoup(true, game->pacman_enemy.score);
	
	draw_common_highscore(game->highscore);

	draw_pacman_lives(game->pacman.livesLeft);
	if(game->mode == MultiState) draw_pacman2_lives(game->pacman_enemy.livesLeft);

	draw_small_pellets(&game->pelletHolder);

	//in gameover state big pellets don't render
	//in gamebegin + levelbegin big pellets don't flash
	//in all other states they flash at normal rate

	switch (game->gameState)
	{
		case GameBeginState:
			draw_game_playerone_start();
			draw_game_ready();

			draw_large_pellets(&game->pelletHolder, false);
			draw_board(&game->board);
			break;
		case LevelBeginState:
			draw_game_ready();

			//we also draw pacman and ghosts (they are idle currently though)
			draw_pacman_static(&game->pacman);
			if(game->mode == MultiState) draw_pacman2_static(&game->pacman_enemy);
			
			for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);

			draw_large_pellets(&game->pelletHolder, false);
			draw_board(&game->board);
			break;
		case GamePlayState:
			//stage 표시
			draw_stage(game->currentLevel);
			draw_large_pellets(&game->pelletHolder, true);
			draw_board(&game->board);

			if (game->gameItem1.itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem1);
			if (game->gameItem2.itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem2);
			if (game->gameItem3.itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem3);
			if (game->gameItem4.itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem4);
			if (game->gameItem5.itemMode == Displaying) draw_item_game(game->currentLevel, &game->gameItem5);

			if (game->gameItem1.eaten && ticks_game() - game->gameItem1.eatenAt < 2000) draw_item_pts(&game->gameItem1);
			if (game->gameItem2.eaten && ticks_game() - game->gameItem2.eatenAt < 2000) draw_item_pts(&game->gameItem2);
			if (game->gameItem3.eaten && ticks_game() - game->gameItem3.eatenAt < 2000) draw_item_pts(&game->gameItem3);
			if (game->gameItem4.eaten && ticks_game() - game->gameItem4.eatenAt < 2000) draw_item_pts(&game->gameItem4);
			if (game->gameItem5.eaten && ticks_game() - game->gameItem5.eatenAt < 2000) draw_item_pts(&game->gameItem5);


			draw_pacman(&game->pacman);

			if(game->pacman.godMode == false) {
				for (int i = 0; i < 4; i++) {
					if(game->ghosts[i].isDead == 1) {
						draw_eyes(&game->ghosts[i]);
					} else
						draw_ghost(&game->ghosts[i]);
				}

			} else {
				if(godChange == false) {
					game->pacman.originDt = ticks_game();
					godChange = true;
				}
				godDt = ticks_game() - game->pacman.originDt;
				for (int i = 0; i < 4; i++) {
					if(game->ghosts[i].isDead == 1) {
						draw_eyes(&game->ghosts[i]);
					} else if(draw_scared_ghost(&game->ghosts[i], godDt)){
						// nothing
						if(game->ghosts[i].isDead == 2) {
							draw_ghost(&game->ghosts[i]);
						}
					} else {
						game->pacman.godMode = false;
						godChange = false;
						if(game->ghosts[i].isDead == 2)
							game->ghosts[i].isDead = 0;
					}
				}
			}
			
			if(game->mode == MultiState) {
					draw_pacman2(&game->pacman_enemy);
					
					if(game->pacman_enemy.godMode == false) {
						for (int i = 0; i < 4; i++) {
							if(game->ghosts[i].isDead == 1) {
								draw_eyes(&game->ghosts[i]);
							} else
								draw_ghost(&game->ghosts[i]);
						}

					} else {
						if(godChange == false) {
							game->pacman_enemy.originDt = ticks_game();
							godChange = true;
						}
						godDt = ticks_game() - game->pacman_enemy.originDt;
						for (int i = 0; i < 4; i++) {
							if(game->ghosts[i].isDead == 1) {
								draw_eyes(&game->ghosts[i]);
							} else if(draw_scared_ghost(&game->ghosts[i], godDt)){
								// nothing
								if(game->ghosts[i].isDead == 2) {
									draw_ghost(&game->ghosts[i]);
								}
							} else {
								game->pacman_enemy.godMode = false;
								godChange = false;
								if(game->ghosts[i].isDead == 2)
									game->ghosts[i].isDead = 0;
							}
						}
					}
			}
			break;
		case WinState:
			draw_pacman_static(&game->pacman);
			if(game->mode == MultiState) draw_pacman2_static(&game->pacman_enemy);

			if (dt < 2000)
			{
				for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);
				draw_board(&game->board);
			}
			else
			{
				//stop rendering the pen, and do the flash animation
				draw_board_flash(&game->board);
			}

			break;
		case DeathState:
			//draw everything the same for 1ish second
			if (dt < 1000)
			{
				//draw everything normally

				//TODO: this actually draws the last frame pacman was on when he died
				draw_pacman_static(&game->pacman);
				if(game->mode == MultiState) draw_pacman2_static(&game->pacman_enemy);

				for (int i = 0; i < 4; i++) draw_ghost(&game->ghosts[i]);
			}
			else
			{
				//draw the death animation
				draw_pacman_death(&game->pacman, dt - 1000);
				if(game->mode == MultiState) draw_pacman2_death(&game->pacman_enemy, dt - 1000);
			}

			draw_large_pellets(&game->pelletHolder, true);
			draw_board(&game->board);
			break;
		case GameoverState:
			draw_game_gameover();
			draw_board(&game->board);
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
			if(game->mode == MultiState) game->pacman_enemy.livesLeft--;

			break;
		case WinState:
			game->currentLevel++;
			game->gameState = LevelBeginState;
			level_init(game);
			break;
		case DeathState:
			// Player died and is starting a new game, subtract a life
			if (state == LevelBeginState)
			{
				if(death_player == Two) game->pacman_enemy.livesLeft--;
				else game->pacman.livesLeft--;
				
				pacdeath_init(game);
			}
		default: ; //do nothing
	}

	//process entering a state
	switch (state)
	{
		case GameBeginState:
			play_sound(LevelStartSound);

			break;
		case LevelBeginState:
			
			break;
		case GamePlayState:
			break;
		case WinState:

			break;
		case DeathState:
			break;
		case GameoverState:
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
	
	if(pacman->usd == 1)
		dir_xy_usd(dir, &x, &y);
	else
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

	if(pacman->usd==1){
		dir_xy_usd(pacman->body.curDir, &curDirX, &curDirY);
		dir_xy_usd(pacman->body.nextDir, &nextDirX, &nextDirY);
	}
	
	else{
		dir_xy(pacman->body.curDir, &curDirX, &curDirY);
		dir_xy(pacman->body.nextDir, &nextDirX, &nextDirY);
	}
	
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
	for (int i = 0; i < 4; i++)
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

			g->nextDirection = next_direction(g, &game->board);
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

static void process_item(PacmanGame *game)
{
	int pelletsEaten = game->pelletHolder.totalNum - game->pelletHolder.numLeft;

	GameItem *f1 = &game->gameItem1;
	GameItem *f2 = &game->gameItem2;
	GameItem *f3 = &game->gameItem3;
	GameItem *f4 = &game->gameItem4;
	GameItem *f5 = &game->gameItem5;

	int curLvl = game->currentLevel;

	if (pelletsEaten >= 30 && f1->itemMode == NotDisplaying)
	{
		f1->itemMode = Displaying;
		regen_item(f1, curLvl);
	}
	else if (pelletsEaten == 60 && f2->itemMode == NotDisplaying)
	{
		f2->itemMode = Displaying;
		regen_item(f2, curLvl);
	}
	else if (pelletsEaten == 90 && f3->itemMode == NotDisplaying)
	{
		f3->itemMode = Displaying;
		regen_item(f3, curLvl);
	}
	else if (pelletsEaten == 120 && f4->itemMode == NotDisplaying)
	{
		f4->itemMode = Displaying;
		regen_item(f4, curLvl);
	}
	else if (pelletsEaten == 150 && f5->itemMode == NotDisplaying)
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
		pac->usd = 0;
		pac->protect = 0;
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
            play_sound(BoosterSound);			
		}
		
		if(f1->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);			
		}
		
		if(f1->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
		
		if(f1->item==Move_USD) {
			pac->usd = 1;
			pac->itemRemainTime = 150;
		}
		
		if(f1->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
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
            play_sound(BoosterSound);	
		}
		
		if(f2->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);			
		}
		
		if(f2->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
		
		if(f2->item==Move_USD) {
			pac->usd = 1;
			pac->itemRemainTime = 150;
		}		
		
		if(f2->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
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
            play_sound(BoosterSound);			
		}
		
		if(f3->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
		}
		
		if(f3->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
		
		if(f3->item==Move_USD) {
			pac->usd = 1;
			pac->itemRemainTime = 150;
		}	
		
		if(f3->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
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
		}
		
		if(f4->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
		}
		
		if(f4->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
		
		if(f4->item==Move_USD) {
			pac->usd = 1;
			pac->itemRemainTime = 150;
		}
		
		if(f4->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
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
		}
		
		if(f5->item==Move_Slow){
			pac->body.velocity = 60;
			pac->itemRemainTime = 150;
            play_sound(Munch_Bsound);
		}
		
		if(f5->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
	
		if(f5->item==Move_USD) {
			pac->usd = 1;
			pac->itemRemainTime = 150;
		}
		
		if(f5->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
		}			
	}
	
		if(game->mode == MultiState) {
			pac = &game->pacman_enemy;
		
		if(pac->itemRemainTime != 0) pac->itemRemainTime--;
		else {
			pac->body.velocity = 80;
			pac->itemRemainTime = 0;
			pac->usd = 0;
			pac->protect = 0;
		}		
		
		if (f1->itemMode == Displaying && collides_obj(&pac->body, f1->x, f1->y))
		{
			f1->itemMode = Displayed;
			f1->eaten = true;
			f1->eatenAt = ticks_game();
			pac->score += item_points(f1->item);
			
			if(f1->item==Life)
				pac->livesLeft += 1;
			
			if(f1->item==Move_Fast) {
				pac->body.velocity = 120;
				pac->itemRemainTime = 150;			
			}
			
			if(f1->item==Move_Slow) {
				pac->body.velocity = 60;
				pac->itemRemainTime = 150;			
			}	
				
			if(f1->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;

			if(f1->item==Move_USD) {
				pac->usd = 1;
				pac->itemRemainTime = 150;
			}
			
			if(f1->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
		}
		}

		if (f2->itemMode == Displaying && collides_obj(&pac->body, f2->x, f2->y))
		{
			f2->itemMode = Displayed;
			f2->eaten = true;
			f2->eatenAt = ticks_game();
			pac->score += item_points(f2->item);
			
			if(f2->item==Life)
				pac->livesLeft += 1;
				
			if(f2->item==Move_Fast) {
				pac->body.velocity = 120;	
				pac->itemRemainTime = 150;			
			}
			
			if(f2->item==Move_Slow) {
				pac->body.velocity = 60;
				pac->itemRemainTime = 150;			
			}

			if(f2->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;

			if(f2->item==Move_USD) {
				pac->usd = 1;
				pac->itemRemainTime = 150;
			}
			
			if(f2->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
		}					
		}

		if (f3->itemMode == Displaying && collides_obj(&pac->body, f3->x, f3->y))
		{
			f3->itemMode = Displayed;
			f3->eaten = true;
			f3->eatenAt = ticks_game();
			pac->score += item_points(f3->item);
			
			if(f3->item==Life)
				pac->livesLeft += 1;
				
			if(f3->item==Move_Fast) {
				pac->body.velocity = 120;
				pac->itemRemainTime = 150;			
			}

			if(f3->item==Move_Slow) {
				pac->body.velocity = 60;
				pac->itemRemainTime = 150;			
			}
			
			if(f3->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
		
			if(f3->item==Move_USD) {
				pac->usd = 1;
				pac->itemRemainTime = 150;
			}
			
			if(f3->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
		}					
		}
		
		if (f4->itemMode == Displaying && collides_obj(&pac->body, f4->x, f4->y))
		{
			f4->itemMode = Displayed;
			f4->eaten = true;
			f4->eatenAt = ticks_game();
			pac->score += item_points(f4->item);
			
			if(f4->item==Life)
				pac->livesLeft += 1;
				
			if(f4->item==Move_Fast) {
				pac->body.velocity = 120;
				pac->itemRemainTime = 150;			
			}
			
			if(f4->item==Move_Slow) {
				pac->body.velocity = 60;
				pac->itemRemainTime = 150;			
			}
			
			if(f4->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
			
			if(f4->item==Move_USD) {
				pac->usd = 1;
				pac->itemRemainTime = 150;
			}	
			
			if(f4->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
		}				
		}
		
		if (f5->itemMode == Displaying && collides_obj(&pac->body, f5->x, f5->y))
		{
			f5->itemMode = Displayed;
			f5->eaten = true;
			f5->eatenAt = ticks_game();
			pac->score += item_points(f5->item);
			
			if(f5->item==Life)
				pac->livesLeft += 1;
				
			if(f5->item==Move_Fast) {
				pac->body.velocity = 120;
				pac->itemRemainTime = 150;			
			}
			
			if(f5->item==Move_Slow) {
				pac->body.velocity = 60;
				pac->itemRemainTime = 150;			
			}

			if(f5->item==Prof)
			for (int i = 0; i < 4; i++) game->ghosts[i].body.velocity = 1;
			
			if(f5->item==Move_USD) {
				pac->usd = 1;
				pac->itemRemainTime = 150;
			}
			
			if(f5->item==Ghost_mode) {
			pac->protect = 1;
			pac->itemRemainTime = 150;
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
	
	PelletHolder *holder = &game->pelletHolder;

	for (int i = 0; i < holder->totalNum; i++)
	{
		Pellet *p = &holder->pellets[i];

		//skip if we've eaten this one already
		if (p->eaten) continue;

		if (collides_obj(&game->pacman.body, p->x, p->y))
		{
			holder->numLeft--;

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

			//can only ever eat 1 pellet in a frame, so return
			return;
		}
		if (collides_obj(&game->pacman_enemy.body, p->x, p->y))
		{
			holder->numLeft--;

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

			//eating a small pellet makes pacman not move for 1 frame
			//eating a large pellet makes pacman not move for 3 frames
			game->pacman_enemy.missedFrames = pellet_nop_frames(p);

			//can only ever eat 1 pellet in a frame, so return
			return;
		}
	}
	
	//maybe next time, poor pacman
}

static bool check_pacghost_collision(PacmanGame *game)
{
	for (int i = 0; i < 4; i++)
	{
		Ghost *g = &game->ghosts[i];
		Pacman *pac = &game->pacman;
		
		/*
		switch(g->ghostType) {
		case Blinky : printf("red : %d \n", g->isDead); break;
		case Inky   : printf("blue : %d \n", g->isDead); break;
		case Clyde  : printf("orange : %d \n", g->isDead); break;
		case Pinky  : printf("pink : %d \n", g->isDead); break;
		}
		*/
		
		if(pac->protect == 0) {
		if (collides(&game->pacman.body, &g->body)) {
			if(game->pacman.godMode == false){
				play_sound(DieSound);
				death_player = One;
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
		
		if(game->mode == MultiState){
				if (collides(&game->pacman_enemy.body, &g->body)) {
					if(game->pacman_enemy.godMode == false){
						death_player = Two;
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

	return false;
}

void gamestart_init(PacmanGame *game, int mode)
{
	// play mode 저장
	if(mode == SoloState) game->mode = SoloState;
	else game->mode = MultiState;
	
	level_init(game);

	// mode가 1(multi) 라면 상대방 팩맨도 생성한다.
	pacman_init(&game->pacman);
	if(game->mode == MultiState) pacman_init(&game->pacman_enemy);
	
	//we need to reset all fruit
	//fuit_init();
	game->highscore = 0; //TODO maybe load this in from a file..?
	game->currentLevel = 1;

	//invalidate the state so it doesn't effect the enter_state function
	game->gameState = -1;
	enter_state(game, GameBeginState);
}

void level_init(PacmanGame *game)
{
	//reset pacmans position
	pacman_level_init(&game->pacman);
	if(game->mode == MultiState) pacman_level_init(&game->pacman_enemy);
	
	//reset pellets
	pellets_init(&game->pelletHolder);

	//reset ghosts
	ghosts_init(game->ghosts);

	//reset fruit
	reset_item(&game->gameItem1, &game->board);
	reset_item(&game->gameItem2, &game->board);
	reset_item(&game->gameItem3, &game->board);
	reset_item(&game->gameItem4, &game->board);
	reset_item(&game->gameItem5, &game->board);

}

void pacdeath_init(PacmanGame *game)
{
	pacman_level_init(&game->pacman);
	if(game->mode == MultiState) pacman_level_init(&game->pacman_enemy);
	ghosts_init(game->ghosts);

	reset_item(&game->gameItem1, &game->board);
	reset_item(&game->gameItem2, &game->board);
	reset_item(&game->gameItem3, &game->board);
	reset_item(&game->gameItem4, &game->board);
	reset_item(&game->gameItem5, &game->board);

}

//TODO: make this method based on a state, not a conditional
//or make the menu system the same. Just make it consistant
bool is_game_over(PacmanGame *game)
{
	unsigned dt = ticks_game() - game->ticksSinceModeChange;

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
