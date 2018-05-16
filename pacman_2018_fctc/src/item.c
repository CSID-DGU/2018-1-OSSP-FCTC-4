#include "item.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL/SDL.h>

#include "item.h"

int item_points(Item item)
{
	switch (item)
	{
		case Move_Fast:     return 500;
		case Move_Slow: 	return 500;
		case Move_USD:      return 500;
		case Misile:      	return 500;
		case Life:			return 500;
		case Prof:			return 1000000;
	}

	printf("invalid fruit\naborting\n");
	exit(1);
}

Item item_for_level(int level)
{
	if (level < 1)
	{
		printf("invalid level: %d\n", level);
		printf("aborting\n");
		exit(1);
	}
	
	else 
	{
		int ran = 0;
		
		ran = rand()%122;
		

			return Move_USD;

	}
}

void reset_item(GameItem *gameItem, Board *board)
{
	//srand((unsigned)time(NULL));
	int randX = 0;
	int randY = 0;
	do {
			randX = rand() % 26;
			randY = rand() % 30;
	} while( (is_valid_square(board, randX, randY) || is_tele_square(randX, randY) ) == false);

	gameItem->item = Move_Fast;
	gameItem->itemMode = NotDisplaying;
	gameItem->startedAt = 0;
	gameItem->displayTime = 0;

	gameItem->eaten = false;
	gameItem->eatenAt = 0;

	gameItem->x = randX;
	gameItem->y = randY;
}

void regen_item(GameItem *gameItem, int level)
{
	gameItem->item = item_for_level(level);
	gameItem->displayTime = rand_item_visible_time();
	gameItem->startedAt = ticks_game();
}

int rand_item_visible_time(void)
{
	// chosen by a random number generator
	// guarenteed to be random
	return 9500;
}
