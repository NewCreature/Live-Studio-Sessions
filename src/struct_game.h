#ifndef LSS_STRUCT_GAME_H
#define LSS_STRUCT_GAME_H

#include "t3f/primitives.h"

#include "defines_game.h"

#include "song_audio.h"
#include "song.h"
#include "profile.h"
#include "controller.h"

typedef struct
{

	int note[32];
	int notes;

} LSS_PLAYER_NOTE_GROUP;

typedef struct
{

	LSS_PROFILE * profile;
	LSS_CONTROLLER * controller; //pointer to controller used by player
	
	int selected_track;
	int selected_difficulty;
	double song_pos;
	int score, high_score;
	int streak;
	int life;
	int miss_streak;
	
	int hit_notes, missed_notes;

	LSS_PLAYER_NOTE_GROUP next_notes;
	LSS_PLAYER_NOTE_GROUP playing_notes;
	LSS_PLAYER_NOTE_GROUP hittable_notes[16];
	int hittable_notes_groups;

	/* cache some values for optimization purposes */
	int first_visible_note;
	int last_visible_note;
	int first_visible_beat;
	int last_visible_beat;

} LSS_PLAYER;

typedef struct
{

	LSS_SONG * song;
	LSS_SONG_AUDIO * song_audio;
	const char * song_id;

	ALLEGRO_BITMAP * note_texture[10];
	ALLEGRO_BITMAP * studio_image;
	ALLEGRO_BITMAP * fret_board_image;
	ALLEGRO_BITMAP * beat_line_image;
	T3F_ATLAS * atlas;
	T3F_PRIMITIVES_CACHE * primitives;

	LSS_PLAYER player[LSS_MAX_PLAYERS];
	int current_tick;
	bool done;

	int av_delay;
	double offset;
	double board_y;
	double board_speed;

} LSS_GAME;

#endif
