#include "t3f/t3f.h"
#include "instance.h"
#include "state.h"
#include "views.h"

void lss_player_join_logic(APP_INSTANCE * app)
{
	int i, j;
	bool new_player = false;

	for(i = 0; i < LSS_MAX_PLAYERS; i++)
	{
		if(!app->game.player[i].active)
		{
			for(j = 0; j < app->controller[i].input->elements; j++)
			{
				if(app->controller[i].input->element[j].pressed)
				{
					app->game.player[i].controller = &app->controller[i];
					app->game.player[i].active = true;
					app->game.player[i].setup_state = LSS_PLAYER_SETUP_GAME_TYPE_SELECT;
					new_player = true;
				}
			}
		}
	}
	if(new_player)
	{
		lss_update_views(&app->game);
	}
}
