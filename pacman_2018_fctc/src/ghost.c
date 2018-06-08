#include "ghost.h"

#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void ghosts_init(Ghost ghosts[5])
{
	ghosts[0].ghostType = Blinky;
	ghosts[1].ghostType = Inky;
	ghosts[2].ghostType = Pinky;
	ghosts[3].ghostType = Clyde;
	ghosts[4].ghostType = Boss;

	for (int i = 0; i < 5; i++)
	{
		reset_ghost(&ghosts[i], ghosts[i].ghostType);
	}

	srand(time(NULL));
}

void reset_ghost(Ghost *ghost, GhostType type)
{
	int x, y;
	int ox, oy;
	Direction dir;
	Direction next;
	MovementMode mode;
	//
	switch (type)
	{
		case Blinky: { x = 14; y = 11; ox = -8; oy =  0; mode = Chase; dir = Left; next = Left; break; }
		case Inky: { x = 12; y = 14; ox = -8; oy =  0; mode = Chase; dir = Up;   next = Down; break; }
		case Clyde: { x = 16; y = 14; ox = -8; oy =  0; mode = Chase; dir = Up;   next = Down; break; }
		case Pinky: { x = 14; y = 14; ox = -8; oy =  0; mode = Chase; dir = Down; next = Down; break; }
		case Boss: { x = 14; y = 5; ox = -8; oy =  0; mode = Chase; dir = Down; next = Down; break; }

		//testing
		//case Inky:  { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		//case Clyde: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }
		//case Pinky: { x = 14; y = 11; ox = -8; oy =  0; mode = Scatter; dir = Left; next = Left; break; }

		default: printf("error ghost\naborting\n"); exit(1);
	}

	ghost->body = (PhysicsBody) { x, y, ox, oy, dir, next, 100, 0, 0};
	ghost->body.xOffsetInternal = 0;
	ghost->body.yOffsetInternal = 0;
	ghost->body.velocity = 80;

	ghost->movementMode = mode;
	//ghost->movementMode = mode;
	ghost->transDirection = Left;
	ghost->nextDirection = Left;
	ghost->isDead = false;
}

void send_to_home(Ghost *ghost, GhostType type)
{
	int targetX;
	int targetY;

	switch (type)
	{
		case Blinky: { targetX = 25; targetY =  -2; break; }
		case Inky:   { targetX = 27; targetY =  31; break; }
		case Clyde:  { targetX =  0; targetY =  31; break; }
		case Pinky:  { targetX =  2; targetY =  -2; break; }
		default:
			printf("error ghost\naborting\n");
			exit(1);
	}

	ghost->targetX = targetX;
	ghost->targetY = targetY;
}

void death_send(Ghost *ghost)
{
	int targetX;
	int targetY;

	targetX = 13;
	targetY = 11;

	ghost->targetX = targetX;
	ghost->targetY = targetY;

	if((ghost->body.x) == 13 && (ghost->body.y == 11)) {
		ghost->isDead = 2;
	}
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

Direction next_direction(Ghost *ghost, Board *board)
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
	dir_xy(ghost->nextDirection, &x, &y); 

	//calculate the distances between the squares (or if it is even valid)
	for (int i = 0; i < 4; i++)
	{
		int testX = ghost->body.x + x + offsets[i].x;
		int testY = ghost->body.y + y + offsets[i].y;

		//allow for when ghost is going through teleporter
		if (testX ==  0) testX = 26;
		if (testX == 27) testX = 1;

		//make sure the square is a valid walkable square
		// square가 걸을 수 있는 square인지 확인한다.
		if (!(is_valid_square(board, testX, testY) || is_tele_square(testX, testY))) continue;

		//a further condition is that ghosts cannot enter certain squares moving upwards
		if (targets[i].dir == Up && is_ghost_noup_square(testX, testY)) continue;

		int dx = testX - ghost->targetX;
		int dy = testY - ghost->targetY;

		//really the square root, but don't take to keep precision
		targets[i].distance = (dx * dx + dy * dy);
	}

	//a ghost can never turn around, so exclude the opposite direction to what they are facing
	Direction reverseDir = dir_opposite(ghost->nextDirection);

	Targetcalc shortest = {Right, INT_MAX};

	//iterate backwards so that we get the tie-breaking logic for free
	for (int i = 3; i >= 0; i--)
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

void execute_ghost_logic(int curLvl, Ghost *targetGhost, GhostType type, Ghost *redGhost, Pacman *pacman)
{
	if (targetGhost->movementMode == Scatter)
	{
		send_to_home(targetGhost, type);
		return;
	}

	switch (type)
	{
		case Blinky: execute_red_logic(curLvl, targetGhost, pacman);            break;
		case Inky:   execute_blue_logic(curLvl, targetGhost, redGhost, pacman); break;
		case Clyde:  execute_orange_logic(curLvl, targetGhost, pacman);         break;
		case Pinky:  execute_pink_logic(curLvl, targetGhost, pacman);           break;
		case Boss:   execute_boss_logic(curLvl, targetGhost, pacman);           break;
	}
}

void execute_boss_logic(int curLvl, Ghost *bossGhost, Pacman *pacman)
{
	int tempX, tempY;
	// switch(curLvl){
	// 	case 1: // Stage 1
	// 		// tempX = rand() % 12;
	// 		tempX = pacman->body.x;
	// 		tempY = rand() % 8;
	// 		break;
	// 	case 2: 
	// 		// Red's AI is to set his target position to pacmans
	// 		tempX = pacman->body.x;
	// 		tempY = pacman->body.y;
	// 		bossGhost->body.velocity = 85;
	// 		break;
	// 	case 3:
	// 		// Red's AI is to set his target position to pacmans
	// 		tempX = pacman->body.x;
	// 		tempY = pacman->body.y;
	// 		bossGhost->body.velocity = 85;
	// 		break;
	// 	case 4:
	// 		// Red's AI is to set his target position to pacmans
	// 		tempX = pacman->body.x;
	// 		tempY = pacman->body.y;
	// 		bossGhost->body.velocity = 90;
	// 		break;
	// 	case 5:
	// 		// Red's AI is to set his target position to pacmans
	// 		tempX = pacman->body.x;
	// 		tempY = pacman->body.y;
	// 		bossGhost->body.velocity = 95;
	// 		break;
	// }
	tempX = pacman->body.x;
	tempY = rand() % 8;
	bossGhost->targetX = tempX;
	bossGhost->targetY = tempY;

	// Red's Ai is random x, y
	// int rNum = rand() % 26;
	// int rNum2 = rand() % 30;

	if(bossGhost->isDead == 1) {death_send(bossGhost);}
}

void execute_red_logic(int curLvl, Ghost *redGhost, Pacman *pacman)
{
	int tempX, tempY;
	switch(curLvl){
		case 1: // Stage 1
			tempX = rand() % 12;
			tempY = rand() % 8;
			break;
		case 2: 
			// Red's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			redGhost->body.velocity = 80;
			break;
		case 3:
			// Red's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			redGhost->body.velocity = 85;
			break;
		case 4:
			// Red's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			redGhost->body.velocity = 90;
			break;
		case 5:
			// Red's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			redGhost->body.velocity = 95;
			break;
	}
	redGhost->targetX = tempX;
	redGhost->targetY = tempY;

	// Red's Ai is random x, y
	// int rNum = rand() % 26;
	// int rNum2 = rand() % 30;

	if(redGhost->isDead == 1) {death_send(redGhost);}
}

void execute_pink_logic(int curLvl, Ghost *pinkGhost, Pacman *pacman)
{
	// Pinks's AI is to set his target position to pacmans, plus a few more in the distance
	int targetOffsetX = 1;
	int targetOffsetY = 1;

	int tempX, tempY;
	switch(curLvl){
		case 1: // Stage 1
			tempX = rand() % 12 + 14;
			tempY = rand() % 8;
			break;
		case 2:
			//use dir_xy_buggy to get 4 up AND 4 left, as per bug in original game
			dir_xy_buggy(pacman->body.curDir, &targetOffsetX, &targetOffsetY);
			targetOffsetX *= 4;
			targetOffsetY *= 4;
			tempX = pacman->body.x + targetOffsetX;
			tempY = pacman->body.y + targetOffsetY;
			break;
		case 3:
			// Pink's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			pinkGhost->body.velocity = 85;
			break;
		case 4:
			// Pink's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			pinkGhost->body.velocity = 90;
			break;
		case 5:
			// Pink's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			pinkGhost->body.velocity = 95;
			break;
	}

	pinkGhost->targetX = tempX;
	pinkGhost->targetY = tempY;

	if(pinkGhost->isDead == 1) {death_send(pinkGhost);}
}

void execute_orange_logic(int curLvl, Ghost *orangeGhost, Pacman *pacman)
{
	int tempX, tempY;
	int dx, dy, distance;
	switch(curLvl){
		case 1: // Stage 1
			tempX = rand() % 12;
			tempY = rand() % 8 + 23;
			break;
		case 2:
			tempX = rand() % 12;
			tempY = rand() % 8 + 23;
			break;
		case 3:
			tempX = rand() % 24;
			tempY = rand() % 16 + 23;
			break;
			// // Orange's logic is in two parts:
			// // If Pacmans distance is 8 or more squares away, his target is pacman
			// // If Pacman is less than 8 squares, his target is his home
			// dx = orangeGhost->body.x - pacman->body.x;
			// dy = orangeGhost->body.y - pacman->body.y;

			// //really the square root, but can compare with square of other number
			// distance = dx * dx + dy * dy;
			// if (distance >= (8 * 8))
			// {
			// 	execute_red_logic(curLvl, orangeGhost, pacman);
			// }
			// else
			// {
			// 	send_to_home(orangeGhost, orangeGhost->ghostType);
			// }

			// if (distance >= (8 * 8))
			// {
			// 	execute_red_logic(curLvl, orangeGhost, pacman);
			// }
			// else
			// {
			// 	send_to_home(orangeGhost, orangeGhost->ghostType);
			// }
			// break;
		case 4:
			// Orange's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			orangeGhost->body.velocity = 90;
			break;
		case 5:
			// Orange's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			orangeGhost->body.velocity = 95;
			break;
	}
	orangeGhost->targetX = tempX;
	orangeGhost->targetY = tempY;

	if(orangeGhost->isDead == 1) {death_send(orangeGhost);}
}

void execute_blue_logic(int curLvl, Ghost *blueGhost, Ghost *redGhost, Pacman *pacman)
{
	int tempX, tempY;
	int offsetX, offsetY;
	int tx, ty, rx, ry;
	switch(curLvl){
		case 1: // Stage 1
			tempX = rand() % 12 + 14;
			tempY = rand() % 8 + 23;
			break;
		case 2:
			tempX = rand() % 12 + 14;
			tempY = rand() % 8 + 23;
			break;
		case 3:
			offsetX = 0;
			offsetY = 0;

			//use dir_xy_buggy to get 2 up AND 2 left, as per bug in original game
			dir_xy_buggy(pacman->body.curDir, &offsetX, &offsetY);

			offsetX *= 2;
			offsetY *= 2;

			tx = pacman->body.x + offsetX;
			ty = pacman->body.y + offsetY;

			rx = redGhost->body.x;
			ry = redGhost->body.y;

			tempX = 2 * tx - rx;
			tempY = 2 * ty - ry;

			break;
		case 4:
			// Blue's AI is to set his target position to pacmans
			offsetX = 0;
			offsetY = 0;

			//use dir_xy_buggy to get 2 up AND 2 left, as per bug in original game
			dir_xy_buggy(pacman->body.curDir, &offsetX, &offsetY);

			offsetX *= 2;
			offsetY *= 2;

			tx = pacman->body.x + offsetX;
			ty = pacman->body.y + offsetY;

			rx = redGhost->body.x;
			ry = redGhost->body.y;

			tempX = 2 * tx - rx;
			tempY = 2 * ty - ry;

			break;
			// tempX = pacman->body.x;
			// tempY = pacman->body.y;
			// blueGhost->body.velocity = 90;
			// break;
		case 5:
			// Blue's AI is to set his target position to pacmans
			tempX = pacman->body.x;
			tempY = pacman->body.y;
			blueGhost->body.velocity = 95;
			break;
	}
	blueGhost->targetX = tempX;
	blueGhost->targetY = tempY;

	if(blueGhost->isDead == 1) {death_send(blueGhost);}
}

int ghost_speed_normal(int level)
{
	if (level == 1)  return 75;
	if (level <= 4)  return 85;
	if (level <= 20) return 95;
	return 95;
}
int ghost_speed_fright(int level)
{
	if (level == 1)  return 50;
	if (level <= 4)  return 55;
	if (level <= 20) return 60;
	return 100; //ghosts don't get into frightened mode past this
}
int ghost_speed_tunnel(int level)
{
	if (level == 1)  return 40;
	if (level <= 4)  return 45;
	if (level <= 20) return 50;
	return 50;
}
