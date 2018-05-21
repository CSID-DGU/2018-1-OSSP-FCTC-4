#pragma once

#include "main.h"
#include "socket.h"
//Defines what action we should take at a particular time while on the menu.
typedef enum
{
	Nothing,
	GoToGame,
	ReadyConnect,
	ServerWait,
	ConnectClient
} MenuAction;

//Defines the menu system.
typedef struct
{
	char* severIP;
	RemoteRole role;
	ModeState mode;
	MenuAction action;
	unsigned int ticksSinceModeChange;
} MenuSystem;

//Performs a single tick on the menu system.
void menu_tick(MenuSystem *menuSystem);
void remote_tick(MenuSystem *menuSystem, Socket_value *socket_info);

//Renders the menu system in its current state.
void menu_render(MenuSystem *menuSystem);
void remote_render(MenuSystem *menuSystem);

void menu_init(MenuSystem *menuSystem);
