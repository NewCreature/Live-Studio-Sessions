#ifndef LSS_SONG_H
#define LSS_SONG_H

#include "t3f/t3f.h"

#include "rtk/midi.h"

#define LSS_SONG_MAX_TRACKS       16
#define LSS_SONG_MAX_DIFFICULTIES  8

#define LSS_SONG_NOTE_HIT_LEVEL_NONE    0
#define LSS_SONG_NOTE_HIT_LEVEL_MISS    1
#define LSS_SONG_NOTE_HIT_LEVEL_BAD     2
#define LSS_SONG_NOTE_HIT_LEVEL_OKAY    3
#define LSS_SONG_NOTE_HIT_LEVEL_GOOD    4
#define LSS_SONG_NOTE_HIT_LEVEL_PERFECT 5

#define LSS_SONG_PLACEMENT_SCALE       12 // 1 tick is equal to 12 z-units

typedef struct
{

	/* static data (don't change after loading) */
	int val;
	int tick;
	int length;
	bool active;
	bool in_chord;
	bool hopo;
	float start_z, end_z;

	/* dynamic data (keep track of stuff during gameplay) */
	int play_tick;
	bool playing;
	bool visible;
	int hit_level;
	bool hidden;

} LSS_SONG_NOTE;

typedef struct
{

	int type;
	LSS_SONG_NOTE ** note;
	int notes;
	int note_count;
	int stream;

} LSS_SONG_TRACK;

typedef struct
{

	double BPM;
	int tick;
	float z;

} LSS_SONG_BEAT;

typedef struct
{

	RTK_MIDI * source_midi;
	ALLEGRO_CONFIG * tags;

	LSS_SONG_TRACK track[LSS_SONG_MAX_TRACKS][LSS_SONG_MAX_DIFFICULTIES];
	double offset;

	LSS_SONG_BEAT ** beat;
	int beats;

} LSS_SONG;

LSS_SONG * lss_load_song(ALLEGRO_PATH * pp);
void lss_destroy_song(LSS_SONG * sp);
bool lss_song_mark_beats(LSS_SONG * sp, double total_length);
void lss_song_hide_prior_notes(LSS_SONG * sp, int track, int difficulty, int note);

#endif
