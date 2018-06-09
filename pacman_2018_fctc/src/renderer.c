#include "renderer.h"

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "animation.h"
#include "imageloader.h"
#include "text.h"
#include "window.h"
#include "game.h"

//draws an image at a board coordinate
void draw_image_coord(SDL_Surface *surface, int x, int y);
void draw_image_coord_offset(SDL_Surface *surface, int x, int y, int xOffset, int yOffset);

//Offset the board is off from the top of the screen
//Needed because some items are stored in board (x,y) coords and need to be rendered at an offset
#define Y_OFFSET 4
static int offset = 4 * 16;

void draw_image_coord(SDL_Surface *surface, int x, int y)
{
	draw_image_coord_offset(surface, x, y, 0, 0);
}

void draw_image_coord_offset(SDL_Surface *surface, int x, int y, int xOffset, int yOffset)
{
	int pixPerTile = 16;
	apply_surface(pixPerTile * x + xOffset, pixPerTile * y + yOffset, surface);
}

//
//
// Vanity screen rendering
//
//

void draw_vanity_ghostline(GhostDisplayRow *row, int y, bool drawDescription, bool drawName)
{
	draw_image_coord(ghost_image(row->type, Right, 0), 2, y);

	if (drawDescription)
	{
		set_text_color(row->color);
		draw_text_coord(get_screen(), row->description, 5, y);
	}

	if (drawName)
	{
		draw_text_coord(get_screen(), row->name, 16, y);
	}
}

void draw_vanity_charnickname(void)
{
	set_text_color(WhiteText);
	draw_text_coord(get_screen(), "CHARACTER / NICKNAME", 5, 11);
}

void draw_vanity_text(const char *text, int x, int y)
{
	set_text_color(WhiteText);
	draw_text_coord(get_screen(), text, x, y);
}

void draw_vanity_corporate_info(void)
{
	set_text_color(PinkText);
	draw_text_coord(get_screen(), "@      MIDWAY MFG.CO.", 4, 32);
	draw_numtext_coord(get_screen(), "  1980", 4, 32);
}

void draw_vanity_pellet_info(bool flashing)
{
	//TODO: use this properly
	if (flashing) return;

	set_text_color(WhiteText);

	draw_image_coord(small_pellet_image(), 10, 26);
	draw_numtext_coord(get_screen(), "10", 12, 26);
	draw_image_coord(pts_white_image(), 15, 26);

	draw_image_coord(large_pellet_image(), 10, 28);
	draw_numtext_coord(get_screen(), "50", 12, 28);
	draw_image_coord(pts_white_image(), 15, 28);
}

void draw_vanity_animation(int dt)
{
	//TODO do this later
	if (dt)
	{

	}
}

//
//
// Instructions screen renderering
//
//

void draw_instrc_info(void)
{
	set_text_color(OrangeText);
	draw_text_coord(get_screen(), "PUSH START BUTTON", 6, 16);

	set_text_color(CyanText);
	draw_text_coord(get_screen(), "  PLAYER ONLY", 8, 20);
	draw_numtext_coord(get_screen(), "1", 8, 20);

	set_text_color(PeachText);
	draw_text_coord(get_screen(), "BONUS PAC-MAN FOR", 1, 24);
	draw_numtext_coord(get_screen(), "                  10000", 1, 24);
	draw_image_coord(pts_peach_image(), 25, 24);
}

void draw_instrc_corporate_info(void)
{
	set_text_color(PinkText);
	draw_text_coord(get_screen(), "@      MIDWAY MFG.CO.", 4, 28);
	draw_numtext_coord(get_screen(), "  1980", 4, 28);
}

//
//
// Common menu rendering
//
//

void draw_common_oneup(bool flashing, int score)
{
	set_text_color(WhiteText);
	draw_numtext_coord(get_screen(), "1", 3, 1);
	draw_text_coord(get_screen(), "UP", 4, 1);

	if (flashing && animation_get_frame(265, 2)) return;

	char scoreStr[256];
	sprintf(scoreStr, "%01i", score);
	draw_text_coord(get_screen(), scoreStr, 6 - int_length(score), 2);
}

void draw_common_twoup(bool flashing, int score)
{
	set_text_color(WhiteText);
	draw_numtext_coord(get_screen(), "2", 22, 1);
	draw_text_coord(get_screen(), "UP", 23, 1);

	if (flashing && animation_get_frame(265, 2)) return;

	char scoreStr[256];
	sprintf(scoreStr, "%01i", score);
	draw_text_coord(get_screen(), scoreStr, 25 - int_length(score), 2);
}

void draw_common_highscore(int highscore)
{
	set_text_color(WhiteText);
	draw_text_coord(get_screen(), "HIGH SCORE", 9, 1);

	//game doesn't render highscore if it is 0 for some reason. Emulate this behaviour
	if (highscore == 0) return;

	char scoreStr[256];
	sprintf(scoreStr, "%01i", highscore);
	draw_text_coord(get_screen(), scoreStr, 16 - int_length(highscore), 2);
}


void draw_stage(int curLvl){
	set_text_color(WhiteText);
	draw_text_coord(get_screen(), "STAGE", 11, 0);

	char levelStr[10];
	sprintf(levelStr, "%01i", curLvl);
	draw_text_coord(get_screen(), levelStr, 16, 0);
}

void draw_common_indicator(int mode, int x, int y)
{
	set_text_color(WhiteText);
	if(mode == 0) draw_text_coord(get_screen(), "@", x, y+3);
	else if(mode == 1) draw_text_coord(get_screen(), "@", x, y+5);
	else draw_text_coord(get_screen(), "@", x, y+7);
}

void draw_credits(int numCredits)
{
	if (numCredits < 0)
	{
		printf("negative number credits: %d\naborting\b", numCredits);
		exit(1);
	}

	char scoreStr[256];
	sprintf(scoreStr, "   %i", numCredits);

	//draw credit + number of credits at the appropriate place
	set_text_color(WhiteText);
	draw_text_coord(get_screen(), "CREDIT", 2, 37);

	draw_numtext_coord(get_screen(), scoreStr, 8 - int_length(numCredits), 37);
}

//
// Game-only word rendering
//

void draw_game_playerone_start(void)
{
	set_text_color(CyanText);
	draw_text_coord(get_screen(), "PLAYER ONE", 9, 15);
}

void draw_game_playertwo_start(void)
{
	set_text_color(OrangeText);
	draw_text_coord(get_screen(), "PLAYER TWO", 9, 17);
}

void draw_game_ready(void)
{
	set_text_color(YellowText);
	draw_text_coord(get_screen(), "READY!", 11, 21);
}

void draw_game_gameover(void)
{
	set_text_color(RedText);
	draw_text_coord(get_screen(), "GAME  OVER", 9, 21);
}

void draw_game_clear(void)
{
	set_text_color(RedText);
	draw_text_coord(get_screen(), "GAME CLEAR ! ! !",7,21);
	set_text_color(YellowText);
	draw_text_coord(get_screen(), "CREATED BY FCTC",7,23);	
	draw_text_coord(get_screen(), "INO  JI SU",9,27);
	draw_text_coord(get_screen(), "JOON SEO  HYEON JONG",4,25);
	draw_text_coord(get_screen(), "SN",25,32);
}


//
//
// Fruit renderering
//
//


//Draws the fruit in the middle of the level.
void draw_item_game(int currentLevel, GameItem *gameItem)
{
	Item item = item_for_level(currentLevel);
	SDL_Surface *image = get_item_image(item);

	//TODO: maybe this offset isn't the same for all fruit. Investigate
	draw_image_coord_offset(image, gameItem->x, gameItem->y + 3, -5, 8);
}

//
//
// Pellet renderering
//
//

void draw_pellet_small(Pellet *p)
{
	draw_image_coord(p->image, p->x, p->y);
}

void draw_pellet_power(Pellet *p, bool flashing)
{
	//do this properly
	if (flashing) return;

	draw_image_coord(p->image, p->x, p->y);
}

void draw_small_pellets(PelletHolder *ph)
{
	for (int i = 0; i < NUM_PELLETS; i++)
	{
		Pellet p = ph->pellets[i];

		if (p.type != SmallPellet) continue;
		if (p.eaten) continue;

		apply_surface(p.x * 16, offset + p.y * 16, p.image);
	}
}

void draw_large_pellets(PelletHolder *ph, bool flashing)
{
	//actual tick rate is 10 * (1 / 60), or 10 frames
	//this comes out to 166.666 ms
	int frame = animation_get_frame(167, 2);

	for (int i = 0; i < NUM_PELLETS; i++)
	{
		Pellet p = ph->pellets[i];

		if (p.type != LargePellet) continue;
		if (p.eaten) continue;
		if (flashing && frame == 1) continue;

		apply_surface(p.x * 16, offset + p.y * 16, p.image);
	}
}

//
//
// Pacman renderering
//
//

void draw_pacman(Pacman *pacman)
{
	int frame;

	Direction aniDir;

	if (pacman->movementType == Stuck)
	{
		//if left/ down, he needs full open frame
		//if up/ right, he uses semi-open frame
		aniDir = pacman->lastAttemptedMoveDirection;
		if (aniDir == Left || aniDir == Down)
		{
			frame = 2;
		}
		else
		{
			frame = 1;
		}
	}
	else
	{
		aniDir = pacman->body.curDir;
		frame = animation_get_frame(50, 4);
	}

	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 4;

	if(!pacman->itemOn) {
		draw_image_coord_offset(pacman_ani_image(aniDir, frame), pacman->body.x, pacman->body.y, xOffset, yOffset);
	} else {
		draw_image_coord_offset(pacman_ani_item_image(aniDir, frame), pacman->body.x, pacman->body.y, xOffset, yOffset);
	}
}

void draw_pacman_static(Pacman *pacman)
{
	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 6;

	draw_image_coord_offset(pacman_image(), pacman->body.x, pacman->body.y, xOffset, yOffset);
}

void draw_pacman_death(Pacman *pacman, unsigned int dt)
{
	//hangs on first image for 200ms
	//cycles through rest of images at constant rate
	//hangs on "plop" image for a while

	unsigned int hang1 = 200;
	unsigned int perFrame = 140;
	unsigned int hang2 = 200;

	int numFrames = 11;

	SDL_Surface *image;

	if (dt < hang1)
	{
		image = pacman_death_image(0);
	}
	else if (dt < (hang1 + numFrames * perFrame))
	{
		int i = animation_get_frame_dt(dt - hang1, perFrame, numFrames);

		image = pacman_death_image(i);
	}
	else if (dt < (hang1 + numFrames * perFrame + hang2))
	{
		//draw last frame
		image = pacman_death_image(10);
	}
	else
	{
		//draw nothing
		return;
	}

	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 6;

	draw_image_coord_offset(image, pacman->body.x, pacman->body.y, xOffset, yOffset);
}

void draw_pacman_lives(int numLives)
{
	int x = 2 * 16;
	int y = 35 * 16;

	for (int i = 0; i < numLives; i++)
	{
		apply_surface(x, y, pacman_life_image());

		x += 16 * 2;
	}
}

//
//
// Pacman player2 renderering
//
//

void draw_pacman2(Pacman *pacman)
{
	int frame;

	Direction aniDir;

	if (pacman->movementType == Stuck)
	{
		//if left/ down, he needs full open frame
		//if up/ right, he uses semi-open frame
		aniDir = pacman->lastAttemptedMoveDirection;
		if (aniDir == Left || aniDir == Down)
		{
			frame = 2;
		}
		else
		{
			frame = 1;
		}
	}
	else
	{
		aniDir = pacman->body.curDir;
		frame = animation_get_frame(50, 4);
	}

	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 4;

	if(!pacman->itemOn) {
		draw_image_coord_offset(pacman2_ani_image(aniDir, frame), pacman->body.x, pacman->body.y, xOffset, yOffset);
	} else {
		draw_image_coord_offset(pacman2_ani_item_image(aniDir, frame), pacman->body.x, pacman->body.y, xOffset, yOffset);
	}
}

void draw_pacman2_static(Pacman *pacman)
{
	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 6;

	draw_image_coord_offset(pacman2_image(), pacman->body.x, pacman->body.y, xOffset, yOffset);
}

void draw_pacman2_death(Pacman *pacman, unsigned int dt)
{
	//hangs on first image for 200ms
	//cycles through rest of images at constant rate
	//hangs on "plop" image for a while

	unsigned int hang1 = 200;
	unsigned int perFrame = 140;
	unsigned int hang2 = 200;

	int numFrames = 11;

	SDL_Surface *image;

	if (dt < hang1)
	{
		image = pacman2_death_image(0);
	}
	else if (dt < (hang1 + numFrames * perFrame))
	{
		int i = animation_get_frame_dt(dt - hang1, perFrame, numFrames);

		image = pacman2_death_image(i);
	}
	else if (dt < (hang1 + numFrames * perFrame + hang2))
	{
		//draw last frame
		image = pacman2_death_image(10);
	}
	else
	{
		//draw nothing
		return;
	}

	int xOffset = pacman->body.xOffset - 4;
	int yOffset = offset + pacman->body.yOffset - 6;

	draw_image_coord_offset(image, pacman->body.x, pacman->body.y, xOffset, yOffset);
}

void draw_pacman2_lives(int numLives)
{
	int x = 18 * 16;
	int y = 35 * 16;

	for (int i = 0; i < numLives; i++)
	{
		apply_surface(x, y, pacman2_life_image());

		x += 16 * 2;
	}
}
//
// Ghost rendering
//

void draw_ghost(Ghost *ghost)
{
	SDL_Surface *image = ghost_image(ghost->ghostType, ghost->body.curDir, 0);

	int x = ghost->body.x;
	int y = ghost->body.y + Y_OFFSET;

	int xOffset = ghost->body.xOffset - 6;
	int yOffset = ghost->body.yOffset - 6;

	draw_image_coord_offset(image, x, y, xOffset, yOffset);
}

void draw_missile(Missile *missile)
{
	SDL_Surface *image = missile_image(missile->missileType, missile->body.curDir, 0);

	int x = missile->body.x;
	int y = missile->body.y + Y_OFFSET;

	int xOffset = missile->body.xOffset - 6;
	int yOffset = missile->body.yOffset - 6;

	draw_image_coord_offset(image, x, y, xOffset, yOffset);
}

bool draw_scared_ghost(Ghost *ghost, unsigned int dt)
{
	//hangs on first image for 200ms
	//cycles through rest of images at constant rate
	//hangs on "plop" image for a while

	unsigned int perFrame = 1000;

	int numFrames = 10;

	SDL_Surface *image;

	if(dt < numFrames * perFrame) {
		if((dt/500) % 2 == 0){
			image = scared_ghost_image(0);
		} else {
			image = scared_ghost_image(1);
		}
	} else {
		return false;
	}

	int x = ghost->body.x;
	int y = ghost->body.y + Y_OFFSET;

	int xOffset = ghost->body.xOffset - 6;
	int yOffset = ghost->body.yOffset - 6;

	draw_image_coord_offset(image, x, y, xOffset, yOffset);
	return true;
}

void draw_eyes(Ghost *ghost) {
	//ghost dead
	SDL_Surface *image = ghost_eye_image(ghost->body.curDir);

	int x = ghost->body.x;
	int y = ghost->body.y + Y_OFFSET;

	int xOffset = ghost->body.xOffset - 6;
	int yOffset = ghost->body.yOffset - 6;

	draw_image_coord_offset(image, x, y, xOffset, yOffset);
}

//
// Points rendering
//

void draw_item_pts(GameItem *gameItem)
{
	Item f = gameItem->item;
	SDL_Surface *image = get_itemshow_image(f);

	draw_image_coord(image, gameItem->x, gameItem->y + 3);
}

void draw_ghost_pts(GameItem *gameItem)
{
	Item f = gameItem->item;
	SDL_Surface *image = get_itemshow_image(f);

	draw_image_coord(image, gameItem->x, gameItem->y + 3);
}

//
//
// Board rendering
//
//

void draw_board(Board *board)
{
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_LENGTH; x++)
		{
			apply_surface(x * 16, offset + y * 16, (board->boardSquares[x][y]).image);
		}
	}
}

void draw_board_flash(Board *board)
{
	int animate = animation_get_frame(250, 2);

	//TODO: make this draw the different colored board instead of just not drawing
	if (animate) return;

	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		for (int x = 0; x < BOARD_LENGTH; x++)
		{
			apply_surface(x * 16, offset + y * 16, (board->boardSquares[x][y]).image);
		}
	}
}
