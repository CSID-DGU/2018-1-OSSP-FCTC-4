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
// Remote Play 모드에서 정보를 받아옴
static void copy_pacmanGame_info(PacmanGame *game);

//Initializes all resources.
static void resource_init(void);

//Initialize all the internal entities needed for the game at startup.
static void game_init(void);

//Called when a game is about to begin (player hits enter from main menu).
static void startgame_init(void);

//Frees all resources.
static void clean_up(void);

//Performs a loop, updating and rendering at 60hz.
static void main_loop(PacmanGame *game);

//Defers to appropriate tick, based on current state.
static void internal_tick(PacmanGame *game);

//Defers to appropriate render, based on current state.
static void internal_render(void);

//Processess and deals with all SDL events.
static void process_events(Player player);

//Performs specific actions on various keypresses. Used for testing.
static void key_down_hacks(int keycode);

static ProgramState state;
static MenuSystem menuSystem;
static PacmanGame pacmanGame;
static PacmanGame *pac;
// Socket value
static Socket_value *socket_info;

static bool gameRunning = true;
static int numCredits = 0;

int main(PacmanGame *game)
{
	resource_init();
	game_init();

	main_loop(game);

	clean_up();
	
	return 0;
}

static void main_loop(PacmanGame *game)
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
		
		internal_tick(game);
		internal_render();

		fps_sleep();
	}
}

static void copy_pacmanGame_info(PacmanGame *game){
	pacmanGame.death_player = pac->death_player;
	pacmanGame.tick = pac->tick;
	pacmanGame.gameState = pac->gameState;
	pacmanGame.ticksSinceModeChange = pac->ticksSinceModeChange;
	pacmanGame.highscore = pac->highscore;
	pacmanGame.currentLevel = pac->currentLevel;
	pacmanGame.mode = pac->mode;
	
	pacmanGame.pacman = pac->pacman;
	pacmanGame.pacman_enemy = pac->pacman_enemy;
	
	for(int i=0; i<4; i++) {
		pacmanGame.ghosts[i] = pac->ghosts[i];
	}
	
	pacmanGame.gameItem1[game->stageLevel] = pac->gameItem1[game->stageLevel];
	pacmanGame.gameItem2[game->stageLevel] = pac->gameItem2[game->stageLevel];
	pacmanGame.gameItem3[game->stageLevel] = pac->gameItem3[game->stageLevel];
	pacmanGame.gameItem4[game->stageLevel] = pac->gameItem4[game->stageLevel];
	pacmanGame.gameItem5[game->stageLevel] = pac->gameItem5[game->stageLevel];
					
	pacmanGame.pelletHolder[game->stageLevel].numLeft = pac->pelletHolder[game->stageLevel].numLeft;
	pacmanGame.pelletHolder[game->stageLevel].totalNum = pac->pelletHolder[game->stageLevel].totalNum;
					
	for(int i=0; i<NUM_PELLETS; i++){
		pacmanGame.pelletHolder[game->stageLevel].pellets[i].x = pac->pelletHolder[game->stageLevel].pellets[i].x;
		pacmanGame.pelletHolder[game->stageLevel].pellets[i].y = pac->pelletHolder[game->stageLevel].pellets[i].y;
		pacmanGame.pelletHolder[game->stageLevel].pellets[i].eaten = pac->pelletHolder[game->stageLevel].pellets[i].eaten;
		pacmanGame.pelletHolder[game->stageLevel].pellets[i].type = pac->pelletHolder[game->stageLevel].pellets[i].type;
		if(pacmanGame.pelletHolder[game->stageLevel].pellets[i].type == LargePellet) pacmanGame.pelletHolder[game->stageLevel].pellets[i].image = large_pellet_image();
		else pacmanGame.pelletHolder[game->stageLevel].pellets[i].image = small_pellet_image();
	}
	
}

static void internal_tick(PacmanGame *game)
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
				menuSystem.role = Server;
				socket_info = (Socket_value*)malloc(sizeof(Socket_value));
			}

			break;
		case Game:
			if(pacmanGame.mode == RemoteState) {
				if(menuSystem.role == Server) {
					
					KeyState key_info;
					recv(socket_info->client_fd, (char*)&key_info, sizeof(KeyState), MSG_WAITALL);
					store_enemy_keysinfo(&key_info);
					
					game_tick(&pacmanGame);
					pacmanGame.tick = ticks_game();
					send(socket_info->client_fd, (char*)&pacmanGame, sizeof(PacmanGame),0);
				}
				else if(menuSystem.role == Client) {
					
					KeyState key_info;
					keyinfo_store(&key_info);
					send(socket_info->client_fd, (char*)&key_info, sizeof(KeyState),0);
					
					pac = (PacmanGame*)malloc(sizeof(PacmanGame));
					recv(socket_info->client_fd, (char*)pac, sizeof(PacmanGame), MSG_WAITALL);
					
					copy_pacmanGame_info(game);
				}
				
				int flag = 0;
				if (is_game_over(&pacmanGame, pacmanGame.tick))
				{
					menu_init(&menuSystem);
					state = Menu;
					pacmanGame.role = None;
					flag = 1;
				}
				if(flag == 1) {
					printf("socket reset!\n");
					close(socket_info->client_fd);
					if(menuSystem.role == Server) close(socket_info->server_fd);
					free(socket_info);
				}
			}
			else {
				game_tick(&pacmanGame);
				
				if (is_game_over(&pacmanGame, ticks_game()))
				{
					menu_init(&menuSystem);
					state = Menu;
				}
			}
			
			

			break;
		case Remote:
			if (menuSystem.action == ServerWait) {
				// listen client
				
				if(connect_server(socket_info) == -1) printf("Wait...\n");
				else menuSystem.action = GoToGame;
			}
			
			remote_tick(&menuSystem, socket_info);
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
			if(menuSystem.role == Client) game_render(&pacmanGame,pacmanGame.tick);
			else game_render(&pacmanGame,ticks_game());
			
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
	char * mapList[4] = {"maps/boss_map", "maps/stage2_map", "maps/map1test", "maps/stage1_map"};
	int i;
	//Load the board here. We only need to do it once
	for(i=0; i<STAGE_COUNT-1; i++){
		load_board(&pacmanGame.board[i], &pacmanGame.pelletHolder[i], mapList[i]);
	}
	// load_board(&pacmanGame.board[1], &pacmanGame.pelletHolder[1], "maps/boss_map");
	// load_board(&pacmanGame.board[2], &pacmanGame.pelletHolder[2], "maps/stage2_map");
	// load_board(&pacmanGame.board[0], &pacmanGame.pelletHolder[0], "maps/map1test");
	// load_board(&pacmanGame.board[3], &pacmanGame.pelletHolder[3], "maps/stage1_map");

	//set to be in menu
	state = Menu;
	
	play_sound(IntrobgmSound);
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
	if (keycode == SDLK_SPACE) fps_sethz((rateSwitch = !rateSwitch) ? 200 : 60);
	//if (keycode == SDLK_SPACE && state != Remote) fps_sethz((rateSwitch = !rateSwitch) ? 200 : 60);

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
			menuSystem.role++;
			if(menuSystem.role > 2) menuSystem.role = 1;
		}
		
		if (keycode == SDLK_UP)
		{
			menuSystem.role--;
			if(menuSystem.role == 0) menuSystem.role = 2;
		}
	}
	
	if(menuSystem.action == ConnectClient) {
		int len = strlen(menuSystem.severIP);
		
		if ( (keycode == SDLK_BACKSPACE) && (len > 0) ) menuSystem.severIP[len-1] = NULL;
		if ( len < 20 && ( (keycode >= SDLK_0 && keycode <= SDLK_9) || keycode == SDLK_PERIOD) ) menuSystem.severIP[len] = keycode;
	}
	//if (keycode == SDLK_9 && state != Remote)
	if (keycode == SDLK_9)
	{
		printf("plus\n");
		for (int i = 0; i < 4; i++) pacmanGame.ghosts[i].body.velocity += 5;

		printf("ghost speed: %d\n", pacmanGame.ghosts[0].body.velocity);
	}
	//else if (keycode == SDLK_0 && state != Remote)
	else if (keycode == SDLK_0)
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
