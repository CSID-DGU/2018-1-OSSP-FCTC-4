#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_imageFilter.h>
#include <SDL/SDL_rotozoom.h>

#include "imageloader.h"

#define DIR "images/"

#define PNT_DIR "show_item/"

#define MAP_DIR "map/"
#define PEN_DIR "pen/"

#define ITEM_DIR "item/"

#define ENTITIES_DIR "entities/"

#define GHOST_DIR "ghosts/"
#define MISSILE_DIR "missiles/"

#define RED_DIR "red/"
#define PINK_DIR "pink/"
#define CYAN_DIR "cyan/"
#define ORANGE_DIR "orange/"
#define BOSS_DIR "boss/"

#define EYES_DIR "eyes/"
#define SCARED_DIR "scared/"

#define PAC_DIR "pacman/"
#define PAC2_DIR "pacman_player2/"
#define PAC_DEATH_DIR "death/"

#define CHAR_DIR "realchar/"

#define LETTERS_DIR "letters/"
#define NUMS_DIR "nums/"
#define SPEC_DIR "special/"

//loads an image from filename and returns it as an SDL_Surface
SDL_Surface *load_image(const char *filename);

//
//hallway sprites
//
SDL_Surface *double_corner_imgs[4];
SDL_Surface *single_corner_imgs[4];
SDL_Surface *tleft_imgs[4];
SDL_Surface *tright_imgs[4];
SDL_Surface *middle_imgs[4];
SDL_Surface *hallway_imgs[4];

//
//pen sprites
//

SDL_Surface *pen_imgs[4];
SDL_Surface *penSide_imgs[4];
SDL_Surface *penGate;

//
//misc sprites
//

SDL_Surface *smallPellet;
SDL_Surface *largePellet;

SDL_Surface *ptsWhiteImage;
SDL_Surface *ptsPeachImage;

SDL_Surface *itemPoints[NUM_ITEM];
SDL_Surface *ghostPoints[4];

//
//pacman sprites
//
SDL_Surface *pacman;
SDL_Surface *pacman_item;
SDL_Surface *aniPacman[4][3];
SDL_Surface *aniPacmanItem[4][3];
SDL_Surface *deathPacman[11];
SDL_Surface *pacmanLifeIcon;

//
//pacman sprites
//

SDL_Surface *pacman2;
SDL_Surface *pacman2_item;
SDL_Surface *aniPacman2[4][3];
SDL_Surface *aniPacmanItem2[4][3];
SDL_Surface *deathPacman2[11];
SDL_Surface *pacmanLifeIcon2;

//
//ghost sprites
//

const int NUM_OF_GHOSTS = 5;
//ghosts have 4 colors * 2 images for main animatino
SDL_Surface *ghosts[5][4][2];

SDL_Surface *missiles[2][4][2];

//2 images for ghost
SDL_Surface *ghostScared[2];
//4 images for eyes
SDL_Surface *ghostEyes[4];

//
//character/ text sprites
//
SDL_Surface *charImages[NUM_CHAR_IMAGES];
SDL_Surface *numImages[NUM_NUM_IMAGES];
SDL_Surface *specImages[NUM_SPEC_IMAGES];

//
//fruit images
//
SDL_Surface *movefastImage;
SDL_Surface *moveslowImage;
SDL_Surface *tornadoImage;
SDL_Surface *ghostmodeImage;
SDL_Surface *lifeImage;
SDL_Surface *profImage;

void load_board_images(void);
void load_pacman_images(void);
void load_pacman_player2_images(void);
void load_ghost_images(void);
void load_missile_images(void);

void load_misc_images(void);
void load_char_images(void);
void load_item_images(void);

void dispose_board_images(void);
void dispose_pacman_images(void);
void dispose_pacman_player2_images(void);
void dispose_ghost_images(void);
void dispose_missile_images(void);
void dispose_misc_images(void);
void dispose_char_images(void);
void dispose_item_images(void);

void load_images(void)
{
	load_board_images();
	load_pacman_images();
	load_pacman_player2_images();
	load_ghost_images();
	load_missile_images();
	load_misc_images();
	load_char_images();
	load_item_images();
}

void dispose_images(void)
{
	dispose_board_images();
	dispose_pacman_images();
	dispose_pacman_player2_images();
	dispose_misc_images();
	dispose_char_images();
	dispose_item_images();
}

void load_diags(SDL_Surface *images[4], const char *file)
{
	char dirStr[256];
	const char *extensions[] = {"UL", "UR", "BL", "BR"};

	for (int i = 0; i < 4; i++)
	{
		sprintf(dirStr, "%s%s%s", file, extensions[i], ".png");
		images[i] = load_image(dirStr);
	}
}

void load_dirs(SDL_Surface *images[4], const char *file)
{
	char dirStr[256];
	const char *extensions[] = {"U", "B", "L", "R"};

	for (int i = 0; i < 4; i++)
	{
		sprintf(dirStr, "%s%s%s", file, extensions[i], ".png");
		images[i] = load_image(dirStr);
	}
}

void load_board_images(void)
{
	load_diags(double_corner_imgs, DIR MAP_DIR "doublecorner");
	load_diags(single_corner_imgs, DIR MAP_DIR "singlecorner");
	load_diags(tleft_imgs, DIR MAP_DIR "tleft");
	load_diags(tright_imgs, DIR MAP_DIR "tright");
	load_dirs(middle_imgs, DIR MAP_DIR "middle");
	load_dirs(hallway_imgs, DIR MAP_DIR "hallway");

	load_diags(pen_imgs, DIR MAP_DIR PEN_DIR "pen");
	load_dirs(penSide_imgs, DIR MAP_DIR PEN_DIR "pen");
	penGate = load_image(DIR MAP_DIR PEN_DIR "pengate.png");
}

void dispose_board_images(void)
{
	for (int i = 0; i < 4; i++)
	{
		SDL_FreeSurface(double_corner_imgs[i]);
		SDL_FreeSurface(single_corner_imgs[i]);
		SDL_FreeSurface(tleft_imgs[i]);
		SDL_FreeSurface(tright_imgs[i]);
		SDL_FreeSurface(middle_imgs[i]);
		SDL_FreeSurface(pen_imgs[i]);
		SDL_FreeSurface(penSide_imgs[i]);
	}

	SDL_FreeSurface(penGate);
}

void load_pacman_images(void)
{
	pacman = load_image(DIR ENTITIES_DIR PAC_DIR "pacman.png");
	pacman_item = load_image(DIR ENTITIES_DIR PAC_DIR "pacman_item.png");

	pacmanLifeIcon = load_image(DIR ENTITIES_DIR PAC_DIR "pac_life_icon.png");

	aniPacman[0][0] = load_image(DIR ENTITIES_DIR PAC_DIR "l0.png");
	aniPacman[0][1] = load_image(DIR ENTITIES_DIR PAC_DIR "l1.png");
	aniPacman[0][2] = aniPacman[0][0];

	aniPacman[1][0] = load_image(DIR ENTITIES_DIR PAC_DIR "u0.png");
	aniPacman[1][1] = load_image(DIR ENTITIES_DIR PAC_DIR "u1.png");
	aniPacman[1][2] = aniPacman[1][0];

	aniPacman[2][0] = load_image(DIR ENTITIES_DIR PAC_DIR "r0.png");
	aniPacman[2][1] = load_image(DIR ENTITIES_DIR PAC_DIR "r1.png");
	aniPacman[2][2] = aniPacman[2][0];

	aniPacman[3][0] = load_image(DIR ENTITIES_DIR PAC_DIR "d0.png");
	aniPacman[3][1] = load_image(DIR ENTITIES_DIR PAC_DIR "d1.png");
	aniPacman[3][2] = aniPacman[3][0];

	//Boost
	aniPacmanItem[0][0] = load_image(DIR ENTITIES_DIR PAC_DIR "l02.png");
	aniPacmanItem[0][1] = load_image(DIR ENTITIES_DIR PAC_DIR "l12.png");
	aniPacmanItem[0][2] = aniPacmanItem[0][0];

	aniPacmanItem[1][0] = load_image(DIR ENTITIES_DIR PAC_DIR "u02.png");
	aniPacmanItem[1][1] = load_image(DIR ENTITIES_DIR PAC_DIR "u12.png");
	aniPacmanItem[1][2] = aniPacmanItem[1][0];

	aniPacmanItem[2][0] = load_image(DIR ENTITIES_DIR PAC_DIR "r02.png");
	aniPacmanItem[2][1] = load_image(DIR ENTITIES_DIR PAC_DIR "r12.png");
	aniPacmanItem[2][2] = aniPacmanItem[2][0];

	aniPacmanItem[3][0] = load_image(DIR ENTITIES_DIR PAC_DIR "d02.png");
	aniPacmanItem[3][1] = load_image(DIR ENTITIES_DIR PAC_DIR "d12.png");
	aniPacmanItem[3][2] = aniPacmanItem[3][0];


	char dirStr[256];

	for (int i = 0; i < 11; i++)
	{
		sprintf(dirStr, "%s%s%d%s", DIR ENTITIES_DIR PAC_DIR PAC_DEATH_DIR, "death", i, ".png");

		deathPacman[i] = load_image(dirStr);
	}
}

void load_pacman_player2_images(void)
{
	pacman2 = load_image(DIR ENTITIES_DIR PAC2_DIR "pacman_2.png");
	pacman2_item = load_image(DIR ENTITIES_DIR PAC2_DIR "pacman2_item.png");

	pacmanLifeIcon2 = load_image(DIR ENTITIES_DIR PAC2_DIR "pac_life_icon_2.png");

	aniPacman2[0][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "l0_2.png");
	aniPacman2[0][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "l1_2.png");
	aniPacman2[0][2] = aniPacman2[0][0];

	aniPacman2[1][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "u0_2.png");
	aniPacman2[1][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "u1_2.png");
	aniPacman2[1][2] = aniPacman2[1][0];

	aniPacman2[2][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "r0_2.png");
	aniPacman2[2][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "r1_2.png");
	aniPacman2[2][2] = aniPacman2[2][0];

	aniPacman2[3][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "d0_2.png");
	aniPacman2[3][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "d1_2.png");
	aniPacman2[3][2] = aniPacman2[3][0];

	//Boost
	aniPacmanItem2[0][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "l02_2.png");
	aniPacmanItem2[0][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "l12_2.png");
	aniPacmanItem2[0][2] = aniPacmanItem2[0][0];

	aniPacmanItem2[1][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "u02_2.png");
	aniPacmanItem2[1][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "u12_2.png");
	aniPacmanItem2[1][2] = aniPacmanItem2[1][0];

	aniPacmanItem2[2][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "r02_2.png");
	aniPacmanItem2[2][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "r12_2.png");
	aniPacmanItem2[2][2] = aniPacmanItem2[2][0];

	aniPacmanItem2[3][0] = load_image(DIR ENTITIES_DIR PAC2_DIR "d02_2.png");
	aniPacmanItem2[3][1] = load_image(DIR ENTITIES_DIR PAC2_DIR "d12_2.png");
	aniPacmanItem2[3][2] = aniPacmanItem2[3][0];


	char dirStr[256];

	for (int i = 0; i < 11; i++)
	{
		sprintf(dirStr, "%s%s%d%s", DIR ENTITIES_DIR PAC2_DIR PAC_DEATH_DIR, "death", i, "_2.png");

		deathPacman2[i] = load_image(dirStr);
	}
}

void dispose_pacman_images(void)
{
	SDL_FreeSurface(pacman);

	SDL_FreeSurface(pacmanLifeIcon);

	SDL_FreeSurface(aniPacman[0][0]);
	SDL_FreeSurface(aniPacman[0][1]);

	SDL_FreeSurface(aniPacman[1][0]);
	SDL_FreeSurface(aniPacman[1][1]);

	SDL_FreeSurface(aniPacman[2][0]);
	SDL_FreeSurface(aniPacman[2][1]);

	SDL_FreeSurface(aniPacman[3][0]);
	SDL_FreeSurface(aniPacman[3][1]);

	//Boost

	SDL_FreeSurface(aniPacmanItem[0][0]);
	SDL_FreeSurface(aniPacmanItem[0][1]);

	SDL_FreeSurface(aniPacmanItem[1][0]);
	SDL_FreeSurface(aniPacmanItem[1][1]);

	SDL_FreeSurface(aniPacmanItem[2][0]);
	SDL_FreeSurface(aniPacmanItem[2][1]);

	SDL_FreeSurface(aniPacmanItem[3][0]);
	SDL_FreeSurface(aniPacmanItem[3][1]);

	for (int i = 0; i < 11; i++)
	{
		SDL_FreeSurface(deathPacman[i]);
	}
}

void dispose_pacman_player2_images(void)
{
	SDL_FreeSurface(pacman2);

	SDL_FreeSurface(pacmanLifeIcon2);

	SDL_FreeSurface(aniPacman2[0][0]);
	SDL_FreeSurface(aniPacman2[0][1]);

	SDL_FreeSurface(aniPacman2[1][0]);
	SDL_FreeSurface(aniPacman2[1][1]);

	SDL_FreeSurface(aniPacman2[2][0]);
	SDL_FreeSurface(aniPacman2[2][1]);

	SDL_FreeSurface(aniPacman2[3][0]);
	SDL_FreeSurface(aniPacman2[3][1]);

	//Boost

	SDL_FreeSurface(aniPacmanItem2[0][0]);
	SDL_FreeSurface(aniPacmanItem2[0][1]);

	SDL_FreeSurface(aniPacmanItem2[1][0]);
	SDL_FreeSurface(aniPacmanItem2[1][1]);

	SDL_FreeSurface(aniPacmanItem2[2][0]);
	SDL_FreeSurface(aniPacmanItem2[2][1]);

	SDL_FreeSurface(aniPacmanItem2[3][0]);
	SDL_FreeSurface(aniPacmanItem2[3][1]);

	for (int i = 0; i < 11; i++)
	{
		SDL_FreeSurface(deathPacman2[i]);
	}
}

const char *red_dir = DIR ENTITIES_DIR GHOST_DIR RED_DIR;
const char *pink_dir = DIR ENTITIES_DIR GHOST_DIR PINK_DIR;
const char *cyan_dir = DIR ENTITIES_DIR GHOST_DIR CYAN_DIR;
const char *orange_dir = DIR ENTITIES_DIR GHOST_DIR ORANGE_DIR;
const char *boss_dir = DIR ENTITIES_DIR GHOST_DIR BOSS_DIR;


const char *one_dir = DIR ENTITIES_DIR MISSILE_DIR;
const char *two_dir = DIR ENTITIES_DIR MISSILE_DIR;

const char *get_ghost_dir(GhostType type)
{
	switch (type)
	{
		case Blinky: return red_dir;
		case Inky:   return cyan_dir;
		case Pinky:  return pink_dir;
		case Clyde:  return orange_dir;
		case Boss :  return boss_dir;
	}

	return NULL;
}

const char *get_missile_dir(MissileType type)
{
		switch (type)
	{
		case M_One: return one_dir;
		case M_Two: return two_dir;
	}
	return NULL;
}

void load_ghost_images(void)
{
	char dirStr[256];

	for (int col = 0; col < NUM_OF_GHOSTS; col++)
	{
		for (int i = 0; i < 2; i++)
		{
			sprintf(dirStr, "%s%s%d%s", get_ghost_dir(col), "u", i, ".png");
			ghosts[col][0][i] = load_image(dirStr);

			sprintf(dirStr, "%s%s%d%s", get_ghost_dir(col), "d", i, ".png");
			ghosts[col][1][i] = load_image(dirStr);

			sprintf(dirStr, "%s%s%d%s", get_ghost_dir(col), "l", i, ".png");
			ghosts[col][2][i] = load_image(dirStr);

			sprintf(dirStr, "%s%s%d%s", get_ghost_dir(col), "r", i, ".png");
			ghosts[col][3][i] = load_image(dirStr);
		}
	}

	ghostScared[0] = load_image(DIR ENTITIES_DIR GHOST_DIR SCARED_DIR "scared0.png");
	ghostScared[1] = load_image(DIR ENTITIES_DIR GHOST_DIR SCARED_DIR "scared1.png");

	ghostEyes[0] = load_image(DIR ENTITIES_DIR GHOST_DIR EYES_DIR "u.png");
	ghostEyes[1] = load_image(DIR ENTITIES_DIR GHOST_DIR EYES_DIR "d.png");
	ghostEyes[2] = load_image(DIR ENTITIES_DIR GHOST_DIR EYES_DIR "l.png");
	ghostEyes[3] = load_image(DIR ENTITIES_DIR GHOST_DIR EYES_DIR "r.png");
}

void load_missile_images(void)
{
	char dirStr[256];

	for (int col = 0; col < 2; col++)
	{
		for (int i = 0; i < 2; i++)
		{
			sprintf(dirStr, "%s%s%d%s", get_missile_dir(col), "u", i, ".png");
			missiles[col][0][i] = load_image(dirStr);

			sprintf(dirStr, "%s%s%d%s", get_missile_dir(col), "d", i, ".png");
			missiles[col][1][i] = load_image(dirStr);
			
			sprintf(dirStr, "%s%s%d%s", get_missile_dir(col), "l", i, ".png");
			missiles[col][2][i] = load_image(dirStr);

			sprintf(dirStr, "%s%s%d%s", get_missile_dir(col), "r", i, ".png");
			missiles[col][3][i] = load_image(dirStr);			
		}
	}
}


void dispose_ghost_images(void)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			SDL_FreeSurface(ghosts[i][j][0]);
			SDL_FreeSurface(ghosts[i][j][1]);
		}
	}

	SDL_FreeSurface(ghostScared[0]);
	SDL_FreeSurface(ghostScared[1]);

	SDL_FreeSurface(ghostEyes[0]);
	SDL_FreeSurface(ghostEyes[1]);
	SDL_FreeSurface(ghostEyes[2]);
	SDL_FreeSurface(ghostEyes[3]);
}


void dispose_missile_images(void)
{
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			SDL_FreeSurface(missiles[i][j][0]);
			SDL_FreeSurface(missiles[i][j][1]);
		}
	}
}

void load_item_images(void)
{
	movefastImage     	= load_image(DIR ITEM_DIR "box.png");
	moveslowImage 		= load_image(DIR ITEM_DIR "box.png");
	tornadoImage      	= load_image(DIR ITEM_DIR "box.png");
	ghostmodeImage      = load_image(DIR ITEM_DIR "box.png");
	lifeImage 			= load_image(DIR ITEM_DIR "box.png");
	profImage			= load_image(DIR ITEM_DIR "box.png");
}

void dispose_item_images(void)
{
	SDL_FreeSurface(movefastImage);
	SDL_FreeSurface(moveslowImage);
	SDL_FreeSurface(tornadoImage);
	SDL_FreeSurface(ghostmodeImage);
	SDL_FreeSurface(lifeImage);
	SDL_FreeSurface(profImage);
}

void load_misc_images(void)
{
	smallPellet = load_image(DIR "smallpellet.png");
	largePellet = load_image(DIR "largepellet.png");

	ptsWhiteImage = load_image(DIR "ptswhite.png");
	ptsPeachImage = load_image(DIR "ptspeach.png");

	char dirStr[256];

	const char *itemPntStr[] = {"Fast", "Slow", "Tornado", "Ghostmode", "Life", "Prof" };
	const char *ghostPntStr[] = {"200", "400", "800", "1600"};

	for (int i = 0; i < NUM_ITEM; i++)
	{
		sprintf(dirStr, "%s%s%s%s", DIR PNT_DIR, "item", itemPntStr[i], ".png");
		itemPoints[i] = load_image(dirStr);
	}

	for (int i = 0; i < 4; i++)
	{
		sprintf(dirStr, "%s%s%s%s", DIR PNT_DIR, "ghost", ghostPntStr[i], ".png");
		ghostPoints[i] = load_image(dirStr);
	}
}

void dispose_misc_images(void)
{
	SDL_FreeSurface(smallPellet);
	SDL_FreeSurface(largePellet);

	SDL_FreeSurface(ptsWhiteImage);
	SDL_FreeSurface(ptsPeachImage);

	for (int i = 0; i < NUM_ITEM; i++)
	{
		SDL_FreeSurface(itemPoints[i]);
	}

	for (int i = 0; i < 4; i++)
	{
		SDL_FreeSurface(ghostPoints[i]);
	}
}

void load_char_images(void)
{
	char dirStr[256];

	for (int i = 0; i < 26; i++)
	{
		char c = 'a';
		sprintf(dirStr, "%s%c%s", DIR CHAR_DIR LETTERS_DIR, (char) (c +i), ".png");

		charImages[i] = load_image(dirStr);
	}

	for (int i = 0; i < 10; i++)
	{
		char c = '0';
		sprintf(dirStr, "%s%c%s", DIR CHAR_DIR NUMS_DIR, (char) (c +i), ".png");

		numImages[i] = load_image(dirStr);
	}

	specImages[0] = load_image(DIR CHAR_DIR SPEC_DIR "space.png");
	specImages[1] = load_image(DIR CHAR_DIR SPEC_DIR "dot.png");
	specImages[2] = load_image(DIR CHAR_DIR SPEC_DIR "dash.png");
	specImages[3] = load_image(DIR CHAR_DIR SPEC_DIR "copyright.png");
	specImages[4] = load_image(DIR CHAR_DIR SPEC_DIR "exclamation.png");
	specImages[5] = load_image(DIR CHAR_DIR SPEC_DIR "forwardslash.png");
	specImages[6] = load_image(DIR CHAR_DIR SPEC_DIR "speechmark.png");
}

void dispose_char_images(void)
{
	for (int i = 0; i < 26; i++)
	{
		SDL_FreeSurface(charImages[i]);
	}

	for (int i = 0; i < 10; i++)
	{
		SDL_FreeSurface(numImages[i]);
	}

	SDL_FreeSurface(specImages[0]);
	SDL_FreeSurface(specImages[1]);
	SDL_FreeSurface(specImages[2]);
	SDL_FreeSurface(specImages[3]);
	SDL_FreeSurface(specImages[4]);
	SDL_FreeSurface(specImages[5]);
	SDL_FreeSurface(specImages[6]);
}

SDL_Surface *load_image(const char *filename)
{
	printf("loading image: %s\n", filename);
	//The image that's loaded
	SDL_Surface* loadedImage = NULL;

	//The optimized image that will be used
	SDL_Surface* optimizedImage = NULL;

	//Load the image using SDL_image
	loadedImage = IMG_Load(filename);

	//If the image loaded
	if(loadedImage != NULL)
	{
		//Create an optimized image
		optimizedImage = SDL_DisplayFormatAlpha(loadedImage);

		//Free the old image
		SDL_FreeSurface(loadedImage);
	}
	else
	{
		printf("Error opening image %s\n", filename);
		printf("Aborting.\n");
		exit(1);
	}

	//Return the optimized image
	return optimizedImage;
}

SDL_Surface* get_char_image(char c)
{
	if ('A' <= c && c <= 'Z') return charImages[c - 'A'];
	else return NULL;
}

SDL_Surface** get_char_images(void)
{
	return charImages;
}

SDL_Surface *get_num_image(char c)
{
	if ('0' <= c && c <= '9') return numImages[c - '0'];
	else return NULL;
}

SDL_Surface** get_num_images(void)
{
	return numImages;
}

SDL_Surface* get_spec_image(char c)
{
	switch (c)
	{
		case ' ': return specImages[0];
		case '.': return specImages[1];
		case '-': return specImages[2];
		case '@': return specImages[3];
		case '!': return specImages[4];
		case '/': return specImages[5];
		case '"': return specImages[6];
	}

	return NULL;
}

SDL_Surface** get_spec_images(void)
{
	return specImages;
}

SDL_Surface* double_corner_image(DiagDirection direction)
{
	return double_corner_imgs[direction];
}

SDL_Surface* single_corner_image(DiagDirection direction)
{
	return single_corner_imgs[direction];
}

SDL_Surface* middle_image(Direction direction)
{
	return middle_imgs[direction];
}

SDL_Surface* hallway_image(Direction direction)
{
	return hallway_imgs[direction];
}

SDL_Surface* tleft_image(DiagDirection direction)
{
	return tleft_imgs[direction];
}

SDL_Surface* tright_image(DiagDirection direction)
{
	return tright_imgs[direction];
}

SDL_Surface* pen_corner_image(DiagDirection direction)
{
	return pen_imgs[direction];
}

SDL_Surface* pen_side_image(Direction direction)
{
	return penSide_imgs[direction];
}

SDL_Surface* pen_gate_image(void)
{
	return penGate;
}

SDL_Surface* small_pellet_image(void)
{
	return smallPellet;
}

SDL_Surface* large_pellet_image(void)
{
	return largePellet;
}

SDL_Surface* pts_white_image(void)
{
	return ptsWhiteImage;
}

SDL_Surface* pts_peach_image(void)
{
	return ptsPeachImage;
}

SDL_Surface* pacman_image(void)
{
	return pacman;
}

SDL_Surface *pacman_ani_image(Direction dir, int frame)
{
	if (frame < 0 || frame > 4)
	{
		printf("invalid pacman frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (frame == 0) return pacman;

	switch(dir)
	{
		case Left:  return aniPacman[0][frame - 1];
		case Up:    return aniPacman[1][frame - 1];
		case Right: return aniPacman[2][frame - 1];
		case Down:  return aniPacman[3][frame - 1];
	}

	printf("incorrect enum value\naborting\n");
	exit(1);
}

SDL_Surface *pacman_ani_item_image(Direction dir, int frame)
{
	if (frame < 0 || frame > 4)
	{
		printf("invalid pacman frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (frame == 0) return pacman_item;

	switch(dir)
	{
		case Left:  return aniPacmanItem[0][frame - 1];
		case Up:    return aniPacmanItem[1][frame - 1];
		case Right: return aniPacmanItem[2][frame - 1];
		case Down:  return aniPacmanItem[3][frame - 1];
	}

	printf("incorrect enum value\naborting\n");
	exit(1);
}

SDL_Surface* pacman_death_image(int i)
{
	return deathPacman[i];
}

SDL_Surface* pacman_life_image(void)
{
	return pacmanLifeIcon;
}

SDL_Surface* pacman2_image(void)
{
	return pacman2;
}

SDL_Surface *pacman2_ani_image(Direction dir, int frame)
{
	if (frame < 0 || frame > 4)
	{
		printf("invalid pacman frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (frame == 0) return pacman2;

	switch(dir)
	{
		case Left:  return aniPacman2[0][frame - 1];
		case Up:    return aniPacman2[1][frame - 1];
		case Right: return aniPacman2[2][frame - 1];
		case Down:  return aniPacman2[3][frame - 1];
	}

	printf("incorrect enum value\naborting\n");
	exit(1);
}

SDL_Surface *pacman2_ani_item_image(Direction dir, int frame)
{
	if (frame < 0 || frame > 4)
	{
		printf("invalid pacman frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (frame == 0) return pacman2_item;

	switch(dir)
	{
		case Left:  return aniPacmanItem2[0][frame - 1];
		case Up:    return aniPacmanItem2[1][frame - 1];
		case Right: return aniPacmanItem2[2][frame - 1];
		case Down:  return aniPacmanItem2[3][frame - 1];
	}

	printf("incorrect enum value\naborting\n");
	exit(1);
}

SDL_Surface* pacman2_death_image(int i)
{
	return deathPacman2[i];
}

SDL_Surface* pacman2_life_image(void)
{
	return pacmanLifeIcon2;
}

SDL_Surface* ghost_image(GhostType type, Direction dir, int frame)
{
	if (frame < 0 || frame > 1)
	{
		printf("invalid ghost frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (dir < 0 || dir > 4)
	{
		printf("invalid ghost direction: %d\n", dir);
		printf("aborting\n");
		exit(1);
	}

	if (frame < 0 || frame > 1)
	{
		printf("invalid ghost frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	return ghosts[type][dir][frame];
}


SDL_Surface* missile_image(MissileType type, Direction dir, int frame)
{
	if (frame < 0 || frame > 1)
	{
		printf("invalid ghost frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	if (frame < 0 || frame > 1)
	{
		printf("invalid ghost frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	return missiles[type][dir][frame];
}

SDL_Surface* scared_ghost_image(int frame)
{
	if (frame < 0 || frame > 1)
	{
		printf("invalid scared ghost frame: %d\n", frame);
		printf("aborting\n");
		exit(1);
	}

	return ghostScared[frame];
}

SDL_Surface* ghost_eye_image(Direction dir)
{
	if (dir < 0 || dir > 4)
	{
		printf("invalid ghost eye direction: %d\n", dir);
		printf("aborting\n");
		exit(1);
	}

	return ghostEyes[dir];
}

SDL_Surface* get_item_image(Item item)
{
	switch (item)
	{
		case Move_Fast:     return movefastImage;
		case Move_Slow: 	return moveslowImage;
		case Tornado:      return tornadoImage;
		case Ghost_mode:    return ghostmodeImage;
		case Life:			return lifeImage;
		case Prof:			return profImage;
	}

	printf("incorrect enum value\naborting\n");
	exit(1);
}

SDL_Surface* get_itemshow_image(Item item)
{
	switch (item)
	{
		case Move_Fast:     return itemPoints[0];
		case Move_Slow: 	return itemPoints[1];
		case Tornado:      return itemPoints[2];
		case Ghost_mode:    return itemPoints[3];
		case Life:			return itemPoints[4];
		case Prof:			return itemPoints[5];
	}

	printf("incorrect itemimage enum value\naborting\n");
	exit(1);
}

SDL_Surface* get_ghost_score_image(int i)
{
	if (i < 0 || i > 3)
	{
		printf("incorrect ghost score num: %d\nAborting\n", i);
		exit(1);
	}

	return ghostPoints[i];
}
