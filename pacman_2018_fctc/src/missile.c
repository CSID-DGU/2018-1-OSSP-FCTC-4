#include "missile.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void missiles_init(Missile missiles[2])
{
	missiles[0].missileType = M_One;
	missiles[1].missileType = M_Two;

	for (int i = 0; i < 2; i++)
	{
		reset_missile(&missiles[i], missiles[i].missileType);
	}
	
	srand(time(NULL));
}

void reset_missile(Missile *missile, MissileType type)
{
	int x, y;
	int ox, oy;
	Direction dir;
	Direction next;
	M_MovementMode mode;
	//
	switch (type)
	{
		case M_One: { x = 5; y = 0; ox = -10; oy =  0; mode = M_Chase; dir = Left; next = Left; break; }
		case M_Two: { x = 20; y = 30; ox = -10; oy =  0; mode = M_Chase; dir = Up;   next = Down; break; }

		//testing
		//case Inky:  { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		//case Clyde: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		//case Pinky: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }

		default: printf("error ghost\naborting\n"); exit(1);
	}

	missile->body = (PhysicsBody) { x, y, ox, oy, dir, next, 100, 0, 0};
	missile->body.xOffsetInternal = 0;
	missile->body.yOffsetInternal = 0;
	missile->body.velocity = 80;

	missile->m_movementMode = mode;
	//ghost->movementMode = mode;
	missile->transDirection = Left;
	missile->nextDirection = Left;
	missile->isDead = false;
}

void msend_to_home(Missile *missile, MissileType type)
{
	int targetX;
	int targetY;

	switch (type)
	{
		case M_One: { targetX = 25; targetY =  -2; break; }
		case M_Two:   { targetX = 27; targetY =  31; break; }
		default:
			printf("error ghost\naborting\n");
			exit(1);
	}

	missile->targetX = targetX;
	missile->targetY = targetY;
}

void mdeath_send(Missile *missile)
{
	int targetX;
	int targetY;

	targetX = 13;
	targetY = 11;

	missile->targetX = targetX;
	missile->targetY = targetY;

}

typedef struct
{
	Direction dir;
	int distance;
} Targetcalc;

typedef struct
{
	int x;
	int y;
} Boardoffset;

Direction m_next_direction(Missile *missile, Board *board)
{
	//ghost has just entered a new tile
	//we look ahead to their next tile
	//then we look at all the surrounding valid tiles
	//we get the distance from each of those to the ghosts target tile
	//we then select the one with the smallest distance
	//if there is a tie, we break it in order Up, Left, Down, Right

	Targetcalc targets[4] =  { {Up, INT_MAX}, {Left, INT_MAX}, {Down, INT_MAX}, {Right, INT_MAX}};
	Boardoffset offsets[4] = { {0, -1}, {-1, 0}, {0, 1}, {1, 0} };

	int x, y;
	// 고스트의 다음 방향(nextDirection)에 따라서 x, y 값을 결정한다.
	// Up(0, -1), Down(0, 1), Left(-1, 0), Right(1, 0)
	dir_xy(missile->nextDirection, &x, &y); 

	//calculate the distances between the squares (or if it is even valid)
	for (int i = 0; i < 2; i++)
	{
		int testX = missile->body.x + x + offsets[i].x;
		int testY = missile->body.y + y + offsets[i].y;

		//allow for when ghost is going through teleporter
		if (testX ==  0) testX = 26;
		if (testX == 27) testX = 1;

		//make sure the square is a valid walkable square
		// square가 걸을 수 있는 square인지 확인한다.
		if (!(is_valid_square(board, testX, testY) || is_tele_square(testX, testY))) continue;

		//a further condition is that ghosts cannot enter certain squares moving upwards


		int dx = testX - missile->targetX;
		int dy = testY - missile->targetY;

		//really the square root, but don't take to keep precision
		targets[i].distance = (dx * dx + dy * dy);
	}

	//a ghost can never turn around, so exclude the opposite direction to what they are facing
	Direction reverseDir = dir_opposite(missile->nextDirection);

	Targetcalc shortest = {Right, INT_MAX};

	//iterate backwards so that we get the tie-breaking logic for free
	for (int i = 1; i >= 0; i--)
	{
		if (targets[i].dir == reverseDir) continue;

		if (targets[i].distance <= shortest.distance)
		{
			shortest.distance = targets[i].distance;
			shortest.dir = targets[i].dir;
		}
	}

	return shortest.dir;
}

void execute_missile_logic(int curLvl, Missile *targetMissile, MissileType type, Missile *oneMissile, Ghost *ghost)
{
	switch (type)
	{
		case M_One:  execute_one_logic(curLvl, targetMissile, ghost);         break;
		case M_Two:  execute_two_logic(curLvl, targetMissile, ghost);         break;
	}
}

void execute_one_logic(int curLvl, Missile *oneMissile, Ghost *ghost)
{
	int tempX, tempY;
	switch(curLvl){
		case 1: // Stage 1
			tempX = rand() % 12;
			tempY = rand() % 8;
			oneMissile->body.velocity = 80;
			break;
		case 2: 
			// Red's AI is to set his target position to pacmans
			tempX = rand() % 12;
			tempY = rand() % 8;
			oneMissile->body.velocity = 80;
			break;
		case 3:
			// Red's AI is to set his target position to pacmans
			tempX = rand() % 12;
			tempY = rand() % 8;
			oneMissile->body.velocity = 80;
			break;
		case 4:
			// Red's AI is to set his target position to pacmans
			tempX = rand() % 12;
			tempY = rand() % 8;
			oneMissile->body.velocity = 80;
			break;
		case 5:
			// Red's AI is to set his target position to pacmans
			tempX = rand() % 12;
			tempY = rand() % 8;
			oneMissile->body.velocity = 80;
			break;
	}
	oneMissile->targetX = tempX;
	oneMissile->targetY = tempY;

	// Red's Ai is random x, y
	// int rNum = rand() % 26;
	// int rNum2 = rand() % 30;

}

void execute_two_logic(int curLvl, Missile *twoMissile, Ghost *ghost)
{
	// Pinks's AI is to set his target position to pacmans, plus a few more in the distance
	int targetOffsetX = 1;
	int targetOffsetY = 1;

	int tempX, tempY;
	switch(curLvl){
		case 1: // Stage 1
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = ghost->body.x + targetOffsetX;
			tempY = ghost->body.y + targetOffsetY;
			twoMissile->body.velocity = 80;
			break;
		case 2:
			//use dir_xy_buggy to get 4 up AND 4 left, as per bug in original game
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = ghost->body.x + targetOffsetX;
			tempY = ghost->body.y + targetOffsetY;
			twoMissile->body.velocity = 80;
			break;
		case 3:
			// Pink's AI is to set his target position to pacmans
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = ghost->body.x + targetOffsetX;
			tempY = ghost->body.y + targetOffsetY;
			twoMissile->body.velocity = 80;
			break;
		case 4:
			// Pink's AI is to set his target position to pacmans
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = ghost->body.x + targetOffsetX;
			tempY = ghost->body.y + targetOffsetY;
			twoMissile->body.velocity = 80;
			break;
		case 5:
			// Pink's AI is to set his target position to pacmans
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = ghost->body.x + targetOffsetX;
			tempY = ghost->body.y + targetOffsetY;
			twoMissile->body.velocity = 80;
			break;
	}

	twoMissile->targetX = tempX;
	twoMissile->targetY = tempY;

}

int missile_speed_normal(int level)
{
	return 80;
}
