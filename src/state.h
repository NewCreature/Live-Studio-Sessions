#ifndef LSS_STATE_H
#define LSS_STATE_H

#include "instance.h"

#define LSS_STATE_TITLE                  0
#define LSS_STATE_SONG_SELECT            1
#define LSS_STATE_SONG_SELECT_TRACK      2
#define LSS_STATE_SONG_SELECT_DIFFICULTY 3
#define LSS_STATE_GAME                   4
#define LSS_STATE_GAME_RESULTS           5
#define LSS_STATE_AV_SETUP               6

void lss_state_logic(APP_INSTANCE * app);
void lss_state_render(APP_INSTANCE * app);

#endif
