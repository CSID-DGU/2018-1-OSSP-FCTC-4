#include "main.h"

#include <stdbool.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "socket.h"
#include "board.h"
#include "boardloader.h"
#include "fps.h"
#include "game.h"
#include "input.h"
#include "intermission.h"
#include "imageloader.h"
#include "menu.h"
#include "sound.h"
#include "text.h"
#include "window.h"

//Initializes all resources.
static void resource_init(void);

//Initialize all the internal entities needed for the game at startup.
static void game_init(void);

//Called when a game is about to begin (player hits enter from main menu).
static void startgame_init(void);

//Frees all resources.
static void clean_up(void);

//Performs a loop, updating and rendering at 60hz.
static void main_loop(void);

//Defers to appropriate tick, based on current state.
static void internal_tick(void);

//Defers to appropriate render, based on current state.
static void internal_render(void);

//Processess and deals with all SDL events.
static void process_events(Player player);

//Performs specific actions on various keypresses. Used for testing.
static void key_down_hacks(int keycode);

static ProgramState state;
static MenuSystem menuSystem;
static PacmanGame pacmanGame;

// Socket value
static Socket_value socket_info;

static bool gameRunning = true;
static int numCredits = 0;

int main(void)
{
	resource_init();
	game_init();

	main_loop();

	clean_up();
	
	return 0;
}

static void main_loop(void)
{
	while (gameRunning && !key_held(SDLK_ESCAPE))
	{
		if(state == Game && pacmanGame.mode == MultiState) {
			process_events(Two);
		}
		else if(state == Game && pacmanGame.mode == RemoteState) {
			process_events(Two);
		}
		else process_events(One);
		
		internal_tick();
		internal_render();

		fps_sleep();
	}
}

static void internal_tick(void)
{
	switch (state)
	{
		case Menu:
			menu_tick(&menuSystem);
			if (menuSystem.action == GoToGame)
			{
				state = Game;
				startgame_init();
			}
			else if(menuSystem.action == ReadyConnect){
				state = Remote;
			}

			break;
		case Game:
			if(pacmanGame.mode == RemoteState) {
				/*
				 * 
				 * 서버가 game_tick으로 pacmanGame에 정보저장하고 
				 * 이때, 클라이언트쪽의 팩맨 정보를 받아서 pacman_enemy의 정보로 저장
				 * 이 정보를 클라이언트로 보내는데 클라이언트는 서버의 pacmanGame의 정보중에서
				 * pacman의 정보를 클라이언트의 pacmanGame의 pacman_enemy에 저장
				 * 나머지 펠렛이나 아이템 고스트의 위치에 관한 모든 정보를 저장
				 * 
				 */
				if(menuSystem.role == Server) {
					/*
					unsigned int* enemy_keyPressed = (unsigned int*)malloc(sizeof(unsigned int)*MAX_KEYS);
					recv(socket_info.client_fd, enemy_keyPressed, sizeof(unsigned int)*MAX_KEYS, 0);
					printf("Rele1: %d %d %d %d\n", enemy_keyPressed[SDLK_UP], enemy_keyPressed[SDLK_DOWN], enemy_keyPressed[SDLK_LEFT], enemy_keyPressed[SDLK_DOWN]);
					printf("Rele2: %d %d %d %d\n", enemy_keyPressed[SDLK_w], enemy_keyPressed[SDLK_s], enemy_keyPressed[SDLK_a], enemy_keyPressed[SDLK_d]);
					store_enemy_keyPressed(enemy_keyPressed);
					
					bool* enemy_keysHeld = (bool*)malloc(sizeof(bool)*MAX_KEYS);
					recv(socket_info.client_fd, enemy_keysHeld, MAX_KEYS, 0);
					store_enemy_keysHeld(enemy_keysHeld);
					print_enemy_key();
					*/
					
					game_tick(&pacmanGame);
					send(socket_info.client_fd, &pacmanGame, sizeof(PacmanGame)+1, 0);
					//PacmanGame *d = (PacmanGame*)malloc(sizeof(PacmanGame));
					//int a = recv(socket_info.client_fd, pacmanGame, sizeof(PacmanGame), 0);
					//printf("size: %d\n",a);
					//send(socket_info.client_fd, tmp, MAX_KEYS, 0);
				}
				else if(menuSystem.role == Client) {
					/*
					unsigned int* my_keyPressed;
					my_keyPressed = player2_keyPressed_state();
					printf("Rele1: %d %d %d %d\n", my_keyPressed[SDLK_UP], my_keyPressed[SDLK_DOWN], my_keyPressed[SDLK_LEFT], my_keyPressed[SDLK_DOWN]);
					printf("Rele2: %d %d %d %d\n", my_keyPressed[SDLK_w], my_keyPressed[SDLK_s], my_keyPressed[SDLK_a], my_keyPressed[SDLK_d]);
					send(socket_info.client_fd, my_keyPressed, sizeof(unsigned int)*MAX_KEYS+1, 0);
					
					bool* my_keysHeld;
					my_keysHeld = player2_keysHeld_state();
					send(socket_info.client_fd, my_keysHeld, MAX_KEYS+1, 0);
					*/
					PacmanGame *d = (PacmanGame*)malloc(sizeof(PacmanGame));
					int a = recv(socket_info.client_fd, d, sizeof(PacmanGame), 0);
					pacmanGame = *d;
					printf("size : %d\n",a);
					//game_tick(&pacmanGame);
					
					//recv(socket_info.client_fd, tmp, MAX_KEYS, 0);
					
				}
			}
			else game_tick(&pacmanGame);
			
			if (is_game_over(&pacmanGame))
			{
				menu_init(&menuSystem);
				state = Menu;
			}

			break;
		case Remote:
			if (menuSystem.action == ServerWait) {
				// listen client
				if(connect_server(&socket_info) == -1) printf("Wait...\n");
				else menuSystem.action = GoToGame;
			}
			
			remote_tick(&menuSystem, &socket_info);
			if (menuSystem.action == GoToGame) {
				state = Game;
				startgame_init();
			}
			
			break;
		case Intermission:
			intermission_tick();
			break;
	}
}

static void internal_render(void)
{
	clear_screen(0, 0, 0, 0);

	switch (state)
	{
		case Menu:
			menu_render(&menuSystem);
			break;
		case Game:
			game_render(&pacmanGame);
			break;
		case Remote:
			remote_render(&menuSystem);
			break;
		case Intermission:
			intermission_render();
			break;
	}

	flip_screen();
}

static void game_init(void)
{
	//Load the board here. We only need to do it once
	load_board(&pacmanGame.board, &pacmanGame.pelletHolder, "maps/rea_map");

	//set to be in menu
	state = Menu;
	
	//init the framerate manager
	fps_init(60);

	menu_init(&menuSystem);
}

static void startgame_init(void)
{
	if(menuSystem.role == Server) pacmanGame.role = Server;
	else if(menuSystem.role == Client) pacmanGame.role = Client;
	gamestart_init(&pacmanGame, menuSystem.mode);
}

static void resource_init(void)
{
	init_window(SCREEN_TITLE, SCREEN_WIDTH, SCREEN_HEIGHT);
	load_images();
	load_sounds();
	load_text();

	//TODO: ensure all the resources loaded properly with some nice function calls
}

static void clean_up(void)
{
	dispose_window();
	dispose_images();
	dispose_sounds();
	dispose_text();

	SDL_Quit();
}

static void process_events(Player player)
{
	static SDL_Event event;
	while (SDL_PollEvent(&event))
	{	
		switch (event.type)
		{
			case SDL_QUIT:
				gameRunning = false;

				break;
			case SDL_KEYDOWN:
				if(pacmanGame.role == Server) handle_keydown(event.key.keysym.sym);
				else if (pacmanGame.role == Client) handle_keydown_player2(event.key.keysym.sym);
				else {
					handle_keydown(event.key.keysym.sym);
					if (player == Two) handle_keydown_player2(event.key.keysym.sym);
				}				
			
				key_down_hacks(event.key.keysym.sym);
				
				break;
			case SDL_KEYUP:
				if(pacmanGame.role == Server) handle_keyup(event.key.keysym.sym);	
				else if (pacmanGame.role == Client) handle_keyup_player2(event.key.keysym.sym);
				else {
					handle_keyup(event.key.keysym.sym);	
					if (player == Two) handle_keyup_player2(event.key.keysym.sym);
				}
				
				break;
		}
		
	}

	keyevents_finished();
	
}

static void key_down_hacks(int keycode)
{
	if (keycode == SDLK_RETURN) pacmanGame.currentLevel++;
	if (keycode == SDLK_BACKSPACE) menuSystem.ticksSinceModeChange = SDL_GetTicks();

	static bool rateSwitch = false;

	//TODO: remove this hack and try make it work with the physics body
	if (keycode == SDLK_SPACE && state != Remote) fps_sethz((rateSwitch = !rateSwitch) ? 200 : 60);

	if (keycode == SDLK_m && state != Remote) {
		if(!pacmanGame.pacman.boostOn) {
			pacmanGame.pacman.body.velocity = 100;
			pacmanGame.pacman.boostOn = true;
		} else {
			pacmanGame.pacman.body.velocity = 80;
			pacmanGame.pacman.boostOn = false;
		}
	}
	
	if (keycode == SDLK_b && state != Remote) {
		if(!pacmanGame.pacman_enemy.boostOn) {
			pacmanGame.pacman_enemy.body.velocity = 100;
			pacmanGame.pacman_enemy.boostOn = true;
		} else {
			pacmanGame.pacman_enemy.body.velocity = 80;
			pacmanGame.pacman_enemy.boostOn = false;
		}
	}

	//TODO: move logic into the tick method of the menu
	if (state == Menu && keycode == SDLK_5 && numCredits < 99)
	{
		numCredits++;
	}
	
	if(state == Menu){
		if (keycode == SDLK_DOWN) 
		{
			menuSystem.mode++;
			if(menuSystem.mode > 2) menuSystem.mode = 0;
		}
		
		if (keycode == SDLK_UP)
		{
			menuSystem.mode--;
			if(menuSystem.mode == -1) menuSystem.mode = 2;
		}
	}
	else if(state == Remote){
		
		if (keycode == SDLK_DOWN) 
		{
			if(menuSystem.role == None) menuSystem.role = Server;
			menuSystem.role++;
			if(menuSystem.role > 2) menuSystem.role = 1;
		}
		
		if (keycode == SDLK_UP)
		{
			if(menuSystem.role == None) menuSystem.role = Server;
			menuSystem.role--;
			if(menuSystem.role == 0) menuSystem.role = 2;
		}
	}
	
	if(menuSystem.action == ConnectClient) {
		int len = strlen(menuSystem.severIP);
		
		if ( (keycode == SDLK_BACKSPACE) && (len > 0) ) menuSystem.severIP[len-1] = NULL;
		if ( len < 20 && ( (keycode >= SDLK_0 && keycode <= SDLK_9) || keycode == SDLK_PERIOD) ) menuSystem.severIP[len] = keycode;
	}
	
	if (keycode == SDLK_9 && state != Remote)
	{
		printf("plus\n");
		for (int i = 0; i < 4; i++) pacmanGame.ghosts[i].body.velocity += 5;

		printf("ghost speed: %d\n", pacmanGame.ghosts[0].body.velocity);
	}
	else if (keycode == SDLK_0 && state != Remote)
	{
		printf("minus\n");
		for (int i = 0; i < 4; i++) pacmanGame.ghosts[i].body.velocity -= 5;

		printf("ghost speed: %d\n", pacmanGame.ghosts[0].body.velocity);
	}
}

int num_credits(void)
{
	return numCredits;
}
