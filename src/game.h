#ifndef LSS_GAME_H
#define LSS_GAME_H

#include "t3f/t3f.h"
#include "t3f/primitives.h"

#include "controller.h"
#include "song.h"
#include "song_audio.h"
#include "resources.h"
#include "profile.h"

#define LSS_MAX_PLAYERS 8

#define LSS_GAME_NOTE_BASE_POINTS          50
#define LSS_GAME_NOTE_SUSTAIN_BASE_POINTS   5

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

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path);
void lss_game_exit(LSS_GAME * gp);
void lss_game_logic(LSS_GAME * gp);
void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp);

#endif
