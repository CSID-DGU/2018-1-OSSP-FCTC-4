#pragma once

#include "board.h"
#include "direction.h"
#include "pacman.h"

typedef enum
{
	Blinky, // red ghost
	Inky,   // cyan
	Pinky,  // pink
	Clyde,   // orange
	Boss    // boss
} GhostType;

typedef enum
{
	InPen,      //in this at the start, as they bob up and down inside the pen
	LeavingPen, //in this as they leave the pen, before being out in the maze proper
	Chase,      //ghosts use their normal AI strategy, usually trying to get pacman
	Scatter,    //ghosts go back to their pens during this mode
	Frightened, //after eating pellets ghosts are in this mode. All blue and can be eaten
	Eaten       //in this mode after being eaten by pacman, takes them back to the pen
} MovementMode;

typedef struct
{
	PhysicsBody body;
	int targetX;
	int targetY;
	Direction transDirection; //direction ghost should face for middle of current tile to middle of next tile
	Direction nextDirection;  //direction ghost should face when reaching the center of the next tilea
	GhostType ghostType;
	MovementMode movementMode;
	int isDead; // 0 = false, 1 = true, 2 = rebirth
	int remain;
} Ghost;

void ghosts_init(Ghost ghosts[5]);

void reset_ghost(Ghost *ghost, GhostType type);

void execute_ghost_logic(int curLvl, Ghost *targetGhost, GhostType type, Ghost *redGhost, Pacman *pacman);

//gets the next direction the ghost will travel in based on their target square
Direction next_direction(Ghost *ghost, Board *board);

void send_to_home(Ghost *ghost, GhostType type);
void death_send(Ghost *ghost);

void execute_boss_logic(int curLvl, Ghost *bossGhost, Pacman *pacman);
void execute_red_logic(int curLvl, Ghost *redGhost, Pacman *pacman);
void execute_pink_logic(int curLvl, Ghost *pinkGhost, Pacman *pacman);
void execute_orange_logic(int curLvl, Ghost *orangeGhost, Pacman *pacman);
void execute_blue_logic(int curLvl, Ghost *blueGhost, Ghost *redGhost, Pacman *pacman);

int ghost_speed_normal(int level);
int ghost_speed_fright(int level);
int ghost_speed_tunnel(int level);
