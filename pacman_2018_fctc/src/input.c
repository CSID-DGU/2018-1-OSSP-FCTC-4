#include "input.h"

//does a check to ensure the given keycode is within the bounds of valid keys
//stops the program with an error if it is out of bounds
static void check_keycode(int keycode);

//gets the frame this direction was pressed.
//Uses both wasd and arrow keys.
static int frame_for_direction(Direction dir);
static int frame_for_direction_player2(Direction dir);

static bool keysHeld[MAX_KEYS] = {false};
static bool keysHeld_player2[MAX_KEYS] = {false};

static unsigned int keysPressedFrame[MAX_KEYS] = {0};
static unsigned int keysReleasedFrame[MAX_KEYS] = {0};

static unsigned int keysPressedFrame_player2[MAX_KEYS] = {0};
static unsigned int keysReleasedFrame_player2[MAX_KEYS] = {0};

static unsigned int curKeyFrame = 1;

bool* player2_keysHeld_state(bool *enemy_key){
	for(int i=0; i<MAX_KEYS; i++) enemy_key[i] = keysHeld_player2[i];
	return keysHeld_player2;
}

unsigned int* player2_keyPressed_state(void){
	
	return keysPressedFrame_player2;
}

void store_enemy_keysHeld(bool* enemy_key) {
	for(int i=0; i<MAX_KEYS; i++) keysHeld_player2[i] = enemy_key[i];
}

void store_enemy_keyPressed(unsigned int* enemy_key) {
	for(int i=0; i<MAX_KEYS; i++) keysPressedFrame_player2[i] = enemy_key[i];
}

void print_enemy_key(void) {
	printf("Held1: %d %d %d %d\n", keysHeld_player2[SDLK_UP], keysHeld_player2[SDLK_DOWN], keysHeld_player2[SDLK_LEFT], keysHeld_player2[SDLK_DOWN]);
	printf("Held2: %d %d %d %d\n", keysHeld_player2[SDLK_w], keysHeld_player2[SDLK_s], keysHeld_player2[SDLK_a], keysHeld_player2[SDLK_d]);
	printf("Rele1: %d %d %d %d\n", keysPressedFrame_player2[SDLK_UP], keysPressedFrame_player2[SDLK_DOWN], keysPressedFrame_player2[SDLK_LEFT], keysPressedFrame_player2[SDLK_DOWN]);
	printf("Rele2: %d %d %d %d\n", keysPressedFrame_player2[SDLK_w], keysPressedFrame_player2[SDLK_s], keysPressedFrame_player2[SDLK_a], keysPressedFrame_player2[SDLK_d]);
}

void keyevents_finished(void)
{
	curKeyFrame++;
}

void handle_keydown(int keycode)
{
	check_keycode(keycode);

	if (!keysHeld[keycode]) keysPressedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = true;
}

void handle_keyup(int keycode)
{
	check_keycode(keycode);

	if (keysHeld[keycode]) keysReleasedFrame[keycode] = curKeyFrame;

	keysHeld[keycode] = false;
}

void handle_keydown_player2(int keycode)
{
	check_keycode(keycode);

	if (!keysHeld_player2[keycode]) keysPressedFrame_player2[keycode] = curKeyFrame;

	keysHeld_player2[keycode] = true;
}

void handle_keyup_player2(int keycode)
{
	check_keycode(keycode);

	if (keysHeld_player2[keycode]) keysReleasedFrame_player2[keycode] = curKeyFrame;

	keysHeld_player2[keycode] = false;
}

bool dir_key_held(Direction direction)
{
	switch (direction)
	{
		case Up:    return keysHeld[SDLK_UP];
		case Down:  return keysHeld[SDLK_DOWN];
		case Left:  return keysHeld[SDLK_LEFT];
		case Right: return keysHeld[SDLK_RIGHT];
	}

	printf("should never reach here\n");
	exit(1);
}

bool dir_key_held_player2(Direction direction)
{
	switch (direction)
	{
		case Up:    return keysHeld_player2[SDLK_w];
		case Down:  return keysHeld_player2[SDLK_s];
		case Left:  return keysHeld_player2[SDLK_a];
		case Right: return keysHeld_player2[SDLK_d];
	}

	printf("should never reach here\n");
	exit(1);
}

bool key_who_player(void)
{
	if(keysHeld[SDLK_UP] || keysHeld[SDLK_DOWN] || keysHeld[SDLK_LEFT] || keysHeld[SDLK_RIGHT]) return true;
	
	return false;
}

bool key_who_player2(void)
{
	if(keysHeld_player2[SDLK_w] || keysHeld_player2[SDLK_s] || keysHeld_player2[SDLK_a] || keysHeld_player2[SDLK_d]) return true;
	
	return false;
}

bool key_is_player(int keycode)
{
	if(keycode == SDLK_w || keycode == SDLK_s || keycode == SDLK_a || keycode == SDLK_d) return true;
	
	return false;
}

bool key_is_player2(int keycode)
{
	if(keycode == SDLK_UP || keycode == SDLK_DOWN || keycode == SDLK_LEFT || keycode == SDLK_RIGHT) return true;
	
	return false;
}

bool dir_pressed_now(Direction *dir)
{
	int highestPushed = 0;

	Direction dirs[4] = {Up, Left, Down, Right};

	for (int i = 3; i >= 0; i--)
	{
		if (!dir_key_held(dirs[i])) continue;
		
		int x = frame_for_direction(dirs[i]);
		
		if (x > highestPushed)
		{
			*dir = dirs[i];
			highestPushed = x;
		}
	}

	return highestPushed != 0;
}

bool dir_pressed_now_player2(Direction *dir)
{
	
	int highestPushed = 0;

	Direction dirs[4] = {Up, Left, Down, Right};

	for (int i = 3; i >= 0; i--)
	{
		if (!dir_key_held_player2(dirs[i])) continue;
		
		int x = frame_for_direction_player2(dirs[i]);
		
		if (x > highestPushed)
		{
			*dir = dirs[i];
			highestPushed = x;
		}
	}

	return highestPushed != 0;
}

bool key_held(int keycode)
{
	check_keycode(keycode);

	return keysHeld[keycode];
}

bool key_pressed(int keycode)
{
	check_keycode(keycode);

	return keysPressedFrame[keycode] == (curKeyFrame - 1);
}

bool key_released(int keycode)
{
	check_keycode(keycode);

	return keysReleasedFrame[keycode] == (curKeyFrame - 1);
}

bool key_held_player2(int keycode)
{
	check_keycode(keycode);

	return keysHeld_player2[keycode];
}

bool key_pressed_player2(int keycode)
{
	check_keycode(keycode);

	return keysPressedFrame_player2[keycode] == (curKeyFrame - 1);
}

bool key_released_player2(int keycode)
{
	check_keycode(keycode);

	return keysReleasedFrame_player2[keycode] == (curKeyFrame - 1);
}


#define max(a, b) (a) > (b) ? (a) : (b)
#define min(a, b) max((b), (a))

static int frame_for_direction(Direction dir)
{	
	switch (dir)
	{
		case Up:    return keysPressedFrame[SDLK_UP];
		case Down:  return keysPressedFrame[SDLK_DOWN];
		case Left:  return keysPressedFrame[SDLK_LEFT];
		case Right: return keysPressedFrame[SDLK_RIGHT];
	}

	printf("should never reach here\n");
	exit(1);
}

static int frame_for_direction_player2(Direction dir)
{	
	switch (dir)
	{
		case Up:    return keysPressedFrame_player2[SDLK_w];
		case Down:  return keysPressedFrame_player2[SDLK_s];
		case Left:  return keysPressedFrame_player2[SDLK_a];
		case Right: return keysPressedFrame_player2[SDLK_d];
	}

	printf("should never reach here\n");
	exit(1);
}

static void check_keycode(int keycode)
{
	if (keycode >= MAX_KEYS)
	{
		printf("Keycode %d is out of range.\n", keycode);
		printf("Aborting\n");
		exit(1);
	}
}
