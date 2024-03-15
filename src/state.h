#ifndef LSS_STATE_H
#define LSS_STATE_H

#include "instance.h"

#define LSS_STATE_TITLE                  0
#define LSS_STATE_GAME_SETUP             1
#define LSS_STATE_GAME                   2
#define LSS_STATE_GAME_RESULTS           3
#define LSS_STATE_AV_SETUP               4
#define LSS_STATE_TITLE_LOGO             5

#define LSS_GAME_SETUP_STATE_SONG_LIST 0
#define LSS_GAME_SETUP_STATE_SETTINGS  1

#define LSS_PLAYER_SETUP_GAME_TYPE_SELECT  0
#define LSS_PLAYER_SETUP_TRACK_SELECT      1
#define LSS_PLAYER_SETUP_DIFFICULTY_SELECT 2
#define LSS_PLAYER_SETUP_READY             3

void lss_state_logic(APP_INSTANCE * app);
void lss_state_render(APP_INSTANCE * app);

#endif
