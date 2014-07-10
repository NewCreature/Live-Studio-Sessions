#ifndef LSS_STRUCT_TITLE_H
#define LSS_STRUCT_TITLE_H

#include "t3f/gui.h"

#include "defines_title.h"

typedef struct
{
	
	T3F_GUI * menu[LSS_MAX_MENUS];
	int current_menu;
	int block_count;
	
} LSS_TITLE_DATA;

#endif
