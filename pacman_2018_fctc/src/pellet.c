#include "pellet.h"
#include "game.h"

void pellets_init(PelletHolder *pelletHolder)
{
	for (int i = 0; i < pelletHolder->pelletNumOfCurrentMap; i++)
	{
		pelletHolder->pellets[i].eaten = false;
	}

	pelletHolder->numLeft = pelletHolder->pelletNumOfCurrentMap;
	pelletHolder->totalNum = pelletHolder->pelletNumOfCurrentMap;
}

int pellet_points(Pellet *pellet)
{
	switch (pellet->type)
	{
		case SmallPellet: return 10;
		case LargePellet: return 50;
	}

	printf("error\n");
	exit(1);
}

int pellet_nop_frames(Pellet *pellet)
{
	switch (pellet->type)
	{
		case SmallPellet: return 1;
		case LargePellet: return 3;
	}

	printf("error\n");
	exit(1);
}

bool pellet_check(Pellet *pellet) {
	if(pellet->type == SmallPellet) {
		return false;
	} else {
		return true;
	}
}
