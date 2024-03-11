#ifndef LSS_RESOURCES_H
#define LSS_RESOURCES_H

#include "t3f/t3f.h"

#define LSS_MAX_FONTS  2
#define LSS_FONT_SMALL 0
#define LSS_FONT_LARGE 1

#define LSS_MAX_BITMAPS     16
#define LSS_BITMAP_TITLE_BG  0
#define LSS_BITMAP_PLATINUM  1

typedef struct
{

	T3F_FONT * font[LSS_MAX_FONTS];
	ALLEGRO_BITMAP * bitmap[LSS_MAX_BITMAPS];

} LSS_RESOURCES;

bool lss_load_global_resources(LSS_RESOURCES * rp);
void lss_free_global_resources(LSS_RESOURCES * rp);

#endif
