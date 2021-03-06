#ifndef LSS_CONTROLLER_H
#define LSS_CONTROLLER_H

#include "t3f/controller.h"

#define LSS_MAX_CONTROLLERS         16

#define LSS_CONTROLLER_TYPE_GUITAR   0
#define LSS_CONTROLLER_TYPE_KEYBOARD 1
#define LSS_CONTROLLER_TYPE_MIDI     2

#define LSS_CONTROLLER_SOURCE_CONTROLLER 0
#define LSS_CONTROLLER_SOURCE_TOUCH      1

#define LSS_CONTROLLER_BINDING_GUITAR_GREEN      0
#define LSS_CONTROLLER_BINDING_GUITAR_RED        1
#define LSS_CONTROLLER_BINDING_GUITAR_YELLOW     2
#define LSS_CONTROLLER_BINDING_GUITAR_BLUE       3
#define LSS_CONTROLLER_BINDING_GUITAR_ORANGE     4
#define LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN 5
#define LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP   6
#define LSS_CONTROLLER_BINDING_GUITAR_STRUM_FAST 7
//#define LSS_CONTROLLER_BINDING_GUITAR_STAR_POWER 7
#define LSS_CONTROLLER_BINDING_MENU              8

#define LSS_TOUCH_OFFSET_X (320 - 32)
#define LSS_TOUCH_OFFSET_Y (420 + 96 - 48)
#define LSS_TOUCH_SPACE_X  80

typedef struct
{

	T3F_CONTROLLER * controller;
	int type, source;

	/* touch-specific info */
	int fret_button_tick[10];
	bool block_strum;

} LSS_CONTROLLER;

void lss_read_controller(LSS_CONTROLLER * cp);

#endif
