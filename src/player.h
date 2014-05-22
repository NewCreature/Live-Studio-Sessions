#ifndef LSS_PLAYER_H
#define LSS_PLAYER_H

#include "controller.h"

#define LSS_MAX_PLAYERS 8

typedef struct
{

	LSS_CONTROLLER * controller; //pointer to controller used by player
	
	int selected_track;
	int selected_difficulty;
	int score;

} LSS_PLAYER;

#endif
