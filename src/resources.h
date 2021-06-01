#ifndef LSS_RESOURCES_H
#define LSS_RESOURCES_H

#include "t3f/t3f.h"

#define LSS_MAX_FONTS  2
#define LSS_FONT_SMALL 0
#define LSS_FONT_LARGE 1

typedef struct
{

	T3F_FONT * font[LSS_MAX_FONTS];
	ALLEGRO_BITMAP * platinum_bitmap;

} LSS_RESOURCES;

bool lss_load_global_resources(LSS_RESOURCES * rp);
void lss_free_global_resources(LSS_RESOURCES * rp);

#endif
