#include "menu.h"

#include <stdbool.h>

#include <SDL/SDL_keysym.h>

#include "input.h"
#include "ghost.h"
#include "renderer.h"

//time till ghost-rows start appearing
#define GHOST_START 1000

//time between each ghost-row appearance
#define GHOST_BETWEEN 2000

static void draw_vanity_screen(MenuSystem *menuSystem);
static void draw_remote_choice_screen(MenuSystem *menuSystem);
static void draw_remote_server_screen(MenuSystem *menuSystem);
static void draw_remote_client_connect_screen(MenuSystem *menuSystem);
static void draw_info_screen(MenuSystem *menuSystem);

static void draw_ghost_line(GhostDisplayRow *row, int y, unsigned int dt);
static void draw_player_info(void);
static void draw_mode_choice(MenuSystem *menuSystem);

static GhostDisplayRow enemyRows[4] = {
	{Blinky, "-SHADOW",  "\"BLINKY\"", RedText},
	{Pinky,  "-SPEEDY",  "\"PINKY\"",  PinkText},
	{Inky,   "-BASHFUL", "\"INKY\"",   CyanText},
	{Clyde,  "-POKEY",   "\"CLYDE\"",  OrangeText}
};

void menu_init(MenuSystem *menuSystem)
{
	//set to be in solo play
	menuSystem->mode = SoloState;
	menuSystem->role = Nothing;
	menuSystem->action = Nothing;
	menuSystem->ticksSinceModeChange = SDL_GetTicks();
	menuSystem->severIP = (char*)malloc(sizeof(char)*20);
	for(int i=0; i<20; i++) menuSystem->severIP[i] = NULL;
}

void menu_tick(MenuSystem *menuSystem)
{
	bool startNew = key_held(SDLK_KP_ENTER) || key_held(SDLK_RETURN);

	if (startNew)
	{
		if(menuSystem->mode == RemoteState) menuSystem->action = ReadyConnect;
		else menuSystem->action = GoToGame;
	}
	
}

void remote_tick(MenuSystem *menuSystem)
{
	bool startNew = key_held(SDLK_KP_ENTER) || key_held(SDLK_RETURN);

	if (startNew)
	{
		if(menuSystem->role == Server) menuSystem->action = ServerWait;
		else if(menuSystem->role == Client) menuSystem->action = ConnectClient;
	}
	
	//if(menuSystem->role == None) menuSystem->role = Server;
}

void menu_render(MenuSystem *menuSystem)
{
	if (num_credits() == 0) draw_vanity_screen(menuSystem);
	else draw_info_screen(menuSystem);
}

void remote_render(MenuSystem *menuSystem)
{
	
	if (menuSystem->action == ServerWait) draw_remote_server_screen(menuSystem);
	else if (menuSystem->action == ConnectClient) draw_remote_client_connect_screen(menuSystem);
	else if (menuSystem->action == ReadyConnect) draw_remote_choice_screen(menuSystem);
}

static void draw_vanity_screen(MenuSystem *menuSystem)
{
	unsigned int dt = SDL_GetTicks() - menuSystem->ticksSinceModeChange;

	draw_player_info();
	draw_vanity_charnickname();
	draw_mode_choice(menuSystem);

	for (int i = 0; i < 4; i++)
	{
		unsigned int current = GHOST_START + i * GHOST_BETWEEN;
		if (dt < current) break;

		GhostDisplayRow r = enemyRows[i];
		draw_ghost_line(&r, 13 + 3 * i, dt - current);
	}

	if (dt > 9500) draw_vanity_pellet_info(false);
	if (dt > 10500) draw_vanity_corporate_info();
	if (dt > 11500) draw_vanity_animation(dt - 11500);
}

static void draw_remote_choice_screen(MenuSystem *menuSystem)
{
	draw_player_info();
	
	if(menuSystem->role == None) draw_common_indicator(Server, 4, 10);
	else draw_common_indicator(menuSystem->role, 4, 10);
	draw_vanity_text("CONNECT SERVER", 7, 15);
	draw_vanity_text("CONNECT CLIENT", 7, 17);
}

static void draw_remote_server_screen(MenuSystem *menuSystem)
{
	unsigned int dt = SDL_GetTicks() - menuSystem->ticksSinceModeChange;

	draw_player_info();
	
	draw_vanity_text("WAITING", 9, 17);
	if (dt%1900 > 400) draw_vanity_text(".", 18, 17);
	if (dt%1900 > 900) draw_vanity_text(".", 19, 17);
	if (dt%1900 > 1400) draw_vanity_text(".", 20, 17);
}

static void draw_remote_client_connect_screen(MenuSystem *menuSystem)
{
	unsigned int dt = SDL_GetTicks() - menuSystem->ticksSinceModeChange;

	draw_player_info();
	
	draw_vanity_text("WRITE SERVER IP", 6, 15);
	if (dt%800 > 400) draw_vanity_text("- ", 4, 17);
	draw_vanity_text(menuSystem->severIP, 6, 17);
}

static void draw_info_screen(MenuSystem *menuSystem)
{
	draw_player_info();
	draw_mode_choice(menuSystem);

	draw_instrc_info();
	draw_instrc_corporate_info();
}

static void draw_player_info(void)
{
	draw_common_oneup(false, 0);
	draw_common_twoup(false, 0);
	draw_common_highscore(0);

	draw_credits(num_credits());
}

static void draw_mode_choice(MenuSystem *menuSystem)
{
	draw_common_indicator(menuSystem->mode, 6, 0);
	draw_vanity_text("PLAY ONE", 9, 3);
	draw_vanity_text("PLAY TWO", 9, 5);
	draw_vanity_text("PLAY MULTI", 9, 7);
}

static void draw_ghost_line(GhostDisplayRow *row,  int y, unsigned int dt)
{
	bool drawDescription = dt > 1000;
	bool drawName = dt > 1500;

	draw_vanity_ghostline(row, y, drawDescription, drawName);
}
