#ifndef LSS_TITLE_H
#define LSS_TITLE_H

#include "t3f/gui.h"

#include "instance.h"

bool lss_title_initialize(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp);
void lss_title_exit(LSS_TITLE_DATA * dp);
void lss_title_logic(LSS_TITLE_DATA * dp, APP_INSTANCE * app);
void lss_title_render(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp);

#endif
