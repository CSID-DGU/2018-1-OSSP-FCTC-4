#pragma once

//28 across, 36 down, 16 pixels each square.
//28 across, 40 down, 16 pixels each square.
#define SCREEN_TITLE "Tphakumahn - ver.2018FCTC"
#define SCREEN_WIDTH 448
#define SCREEN_HEIGHT 640 //origin : 576

//Defines the main states the program can be in.
typedef enum
{
	Menu,
	Game,
	Intermission
} ProgramState;

typedef enum
{
	SoloState,
	MultiState,
	RemoteState
} ModeState;

//Returns the number of credits the user currently has.
int num_credits(void);
