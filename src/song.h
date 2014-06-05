#ifndef LSS_SONG_H
#define LSS_SONG_H

#include "t3f/t3f.h"

#include "rtk/midi.h"

#define LSS_SONG_MAX_TRACKS       16
#define LSS_SONG_MAX_DIFFICULTIES  8

typedef struct
{
	
	int val;
	int tick;
	int play_tick;
	int length;
	bool active;
	bool in_chord;
	bool playing;
	bool visible;
	bool hopo;
	
} LSS_SONG_NOTE;

typedef struct
{
	
	int type;
	LSS_SONG_NOTE ** note;
	int notes;
	int note_count;
	
} LSS_SONG_TRACK;

typedef struct
{
	
	RTK_MIDI * source_midi;
	ALLEGRO_CONFIG * tags;
	
	LSS_SONG_TRACK track[LSS_SONG_MAX_TRACKS][LSS_SONG_MAX_DIFFICULTIES];
	double offset;

} LSS_SONG;

LSS_SONG * lss_load_song(ALLEGRO_PATH * pp);
void lss_destroy_song(LSS_SONG * sp);

#endif
