#pragma once

#include "board.h"
#include "direction.h"
#include "ghost.h"

typedef enum
{
	M_One,
	M_Two
} MissileType;

typedef enum
{
	M_InPen,      //in this at the start, as they bob up and down inside the pen
	M_LeavingPen, //in this as they leave the pen, before being out in the maze proper
	M_Chase      //ghosts use their normal AI strategy, usually trying to get pacman
} M_MovementMode;

typedef struct
{
	PhysicsBody body;
	int targetX;
	int targetY;
	Direction transDirection; //direction ghost should face for middle of current tile to middle of next tile
	Direction nextDirection;  //direction ghost should face when reaching the center of the next tilea
	MissileType missileType;
	M_MovementMode m_movementMode;
	int isDead; // 0 = false, 1 = true, 2 = rebirth
} Missile;

void missiles_init(Missile missiles[2]);

void reset_missile(Missile *missile, MissileType type);

void execute_missile_logic(int curLvl, Missile *targetMissile, MissileType type, Missile *oneMissile, Ghost *ghost);

//gets the next direction the ghost will travel in based on their target square
Direction m_next_direction(Missile *missile, Board *board);

void m_send_to_home(Missile *missile, MissileType type);
void m_death_send(Missile *missile);

void execute_one_logic(int curLvl, Missile *oneMissile, Ghost *ghost);
void execute_two_logic(int curLvl, Missile *twoMissile, Ghost *ghost);

int missile_speed_normal(int level);

