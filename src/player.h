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
	int streak;
	int life;
	int miss_streak;
	
	int hit_notes;

	int next_note[32];
	int next_notes;

	/* list of currently playing notes */
	int playing_note[32];
	int playing_notes;
	
	/* list of notes that can be hit */
	int hittable_note[32];
	int hittable_notes;

} LSS_PLAYER;

#endif
