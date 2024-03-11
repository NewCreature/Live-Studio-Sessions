#ifndef LSS_STRUCT_TITLE_H
#define LSS_STRUCT_TITLE_H

#include "t3f/gui.h"
#include "t3f/rng.h"

#include "modules/song_audio.h"

#include "defines_title.h"

typedef struct
{

	ALLEGRO_BITMAP * logo_bitmap;
	ALLEGRO_BITMAP * bg_bitmap;
	T3F_GUI * menu[LSS_MAX_MENUS];
	int current_menu;
	int block_count;
	
	LSS_SONG_AUDIO * song_audio;
	T3F_RNG_STATE rng;
	
} LSS_TITLE_DATA;

#endif
