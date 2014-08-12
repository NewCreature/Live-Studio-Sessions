#ifndef LSS_TITLE_H
#define LSS_TITLE_H

#include "t3f/gui.h"

#include "modules/song_list.h"

#include "instance.h"

void lss_select_menu(LSS_TITLE_DATA * tp, int menu);
bool lss_create_profiles_menu(APP_INSTANCE * app);
bool lss_create_controller_menu(APP_INSTANCE * app);
bool lss_title_initialize(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp, LSS_SONG_LIST * lp);
void lss_title_exit(LSS_TITLE_DATA * dp);
void lss_title_logic(LSS_TITLE_DATA * dp, APP_INSTANCE * app);
void lss_title_render(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp);

#endif
