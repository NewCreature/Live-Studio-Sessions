#ifndef LSS_GAME_H
#define LSS_GAME_H

#include "t3f/t3f.h"

#include "song.h"
#include "song_audio.h"
#include "player.h"

typedef struct
{

	LSS_SONG * song;
	LSS_SONG_AUDIO * song_audio;

	ALLEGRO_BITMAP * notes_texture;

	LSS_PLAYER player[LSS_MAX_PLAYERS];
	int current_tick;
	bool done;

	int av_delay;
	double offset;

} LSS_GAME;

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path);
void lss_game_exit(LSS_GAME * gp);
void lss_game_logic(LSS_GAME * gp);
void lss_game_render(LSS_GAME * gp);

#endif
