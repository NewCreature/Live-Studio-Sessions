#ifndef LSS_STATE_SONG_LIST_H
#define LSS_STATE_SONG_LIST_H

#include "instance.h"

void lss_state_song_list_initialize(APP_INSTANCE * app);
void lss_state_song_list_song_select_logic(APP_INSTANCE * app);
void lss_state_song_list_song_select_render(APP_INSTANCE * app);
void lss_state_song_list_track_select_logic(APP_INSTANCE * app);
void lss_state_song_list_track_select_render(APP_INSTANCE * app);
void lss_state_song_list_difficulty_select_logic(APP_INSTANCE * app);
void lss_state_song_list_difficulty_select_render(APP_INSTANCE * app);

#endif
