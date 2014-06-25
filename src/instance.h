#ifndef LSS_INSTANCE_H
#define LSS_INSTANCE_H

#include "resources.h"
#include "controller.h"
#include "song_list.h"
#include "profile.h"
#include "game.h"
#include "struct_title.h"
#include "defines_title.h"

/* structure to hold all of our app-specific data */
typedef struct
{
	
	/* global data */
	LSS_RESOURCES resources;
	LSS_CONTROLLER controller[LSS_MAX_CONTROLLERS];

	LSS_SONG_LIST * song_list;
	
	LSS_PROFILES * profiles;
	
	/* menu data */
	int selected_song;
	
	int state;
	
	LSS_TITLE_DATA title;
	LSS_GAME game;
	
} APP_INSTANCE;

#endif
