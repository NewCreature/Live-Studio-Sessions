#ifndef LSS_PLAYER_H
#define LSS_PLAYER_H

#include "game.h"

void lss_initialize_player(LSS_GAME * gp, int player);
void lss_player_logic(LSS_GAME * gp, int player);
void lss_player_render_board(LSS_GAME * gp, int player);

#endif
