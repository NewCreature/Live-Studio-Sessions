#ifndef LSS_STRUCT_GAME_H
#define LSS_STRUCT_GAME_H

#include "t3f/primitives.h"

#include "defines_game.h"

#include "modules/song_audio.h"
#include "modules/song.h"
#include "modules/profile.h"
#include "modules/controller.h"

typedef struct
{

	int note[32];
	int notes;
	bool hopo;
	int hopo_points; // points awarded for played HOPO note, subtract these if note is strummed later

} LSS_PLAYER_NOTE_GROUP;

typedef struct
{

	LSS_PROFILE * profile;
	LSS_CONTROLLER * controller; //pointer to controller used by player
	T3F_VIEW * view; // view for this player
	T3F_GUI * menu; // per-player game setup menu
	bool block_menu_strum;

	bool active;

	/* cache song details for menu creation */
	int track[16];
	int tracks;
	int diff[4];
	int track_type[16];
	int setup_state;
	int selected_game_type;
	int selected_track;
	int selected_difficulty;
	double song_pos;
	int score, high_score;
	int streak;
	int life;
	int miss_streak;
	int multiplier;
	bool full_combo;

	LSS_PLAYER_NOTE_GROUP next_notes;
	LSS_PLAYER_NOTE_GROUP playing_notes;
	LSS_PLAYER_NOTE_GROUP hittable_notes[16];
	int hittable_notes_groups;

	/* cache some values for optimization purposes */
	int first_visible_note;
	int last_visible_note;
	int first_visible_beat;
	int last_visible_beat;

	/* cache game results so we only need to calculate them once */
	int stars;
	double accuracy;
	double completion;
	int total_notes, hit_notes, missed_notes, perfect_notes, good_notes, bad_notes;

} LSS_PLAYER;

typedef struct
{

	LSS_SONG * song;
	LSS_SONG_AUDIO * song_audio;
	const char * song_id;
	float backing_audio_gain;
	float playing_audio_gain;
	float not_playing_audio_gain;
	float missed_audio_gain;

	ALLEGRO_BITMAP * note_texture[10];
	ALLEGRO_BITMAP * fret_button_texture[5];
	ALLEGRO_BITMAP * studio_image;
	ALLEGRO_BITMAP * fret_board_image;
	ALLEGRO_BITMAP * fret_buttons_image;
	ALLEGRO_BITMAP * beat_line_image;
	ALLEGRO_BITMAP * fret_button_image;
	ALLEGRO_BITMAP * strum_bar_image;
	T3F_ATLAS * atlas;
	T3F_PRIMITIVES_CACHE * primitives;
	T3F_GUI * pause_menu;
	int setup_state;

	LSS_PLAYER player[LSS_MAX_PLAYERS];
	int current_tick, current_beat;
	int rewind_tick;
	float camera_z, camera_vz;
	float delay_z;
	bool paused;
	bool done;
	bool no_fail;

	int av_delay;
	double offset;
	double board_y;
	double board_speed;

} LSS_GAME;

#endif
