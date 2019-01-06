#ifndef LSS_STATE_GAME_H
#define LSS_STATE_GAME_H

#include "t3f/t3f.h"

#include "struct_game.h"

#include "resources.h"

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path, LSS_RESOURCES * rp);
void lss_game_exit(LSS_GAME * gp);
void lss_game_logic(LSS_GAME * gp);
void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp);

#endif
