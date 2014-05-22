#ifndef LSS_RESOURCES_H
#define LSS_RESOURCES_H

#include "t3f/t3f.h"

typedef struct
{

	ALLEGRO_FONT * font;

} LSS_RESOURCES;

bool lss_load_global_resources(LSS_RESOURCES * rp);
void lss_free_global_resources(LSS_RESOURCES * rp);

#endif
