#include "t3net/t3net.h"

#include "modules/obfuscate.h"

#include "instance.h"
#include "state.h"
#include "state_game.h"
#include "state_game_results.h"
#include "state_title.h"
#include "state_av_setup.h"
#include "state_song_list.h"

void lss_state_logic(APP_INSTANCE * app)
{
	char buf[2][64];

	switch(app->state)
	{
		case LSS_STATE_TITLE:
		{
			lss_title_logic(&app->title, app);
			break;
		}
		case LSS_STATE_SONG_SELECT:
		{
			lss_state_song_list_song_select_logic(app);
			break;
		}
		case LSS_STATE_SONG_SELECT_TRACK:
		{
			lss_state_song_list_track_select_logic(app);
			break;
		}
		case LSS_STATE_SONG_SELECT_DIFFICULTY:
		{
			lss_state_song_list_difficulty_select_logic(app);
			break;
		}
		case LSS_STATE_GAME:
		{
			lss_game_logic(&app->game);
			if(app->game.done)
			{
				if(app->game.player[0].score > app->game.player[0].high_score)
				{
					app->game.player[0].high_score = app->game.player[0].score;
					sprintf(buf[0], "high_score_%d_%d", app->game.player[0].selected_track, app->game.player[0].selected_difficulty);
					sprintf(buf[1], "%d", lss_obfuscate_value(app->game.player[0].high_score));
					al_set_config_value(app->game.player[0].profile->config, app->game.song_id, buf[0], buf[1]);
				}
				sprintf(buf[0], "%d%d", app->game.player[0].selected_track, app->game.player[0].selected_difficulty);
				sprintf(buf[1], "%s", app->game.player[0].profile->name);
				if(app->game.player[0].score > 0)
				{
					if(!t3net_upload_score("http://www.t3-i.com/leaderboards/poll.php", "live_studio_sessions", "0.1", buf[0], app->song_list->entry[app->selected_song]->id, buf[1], lss_obfuscate_value(app->game.player[0].score)))
					{
						printf("failed to upload score\n");
					}
				}
				app->leaderboard = t3net_get_leaderboard("http://www.t3-i.com/leaderboards/query_2.php", "live_studio_sessions", "0.1", buf[0], app->song_list->entry[app->selected_song]->id, 10, 0);
				if(!app->leaderboard)
				{
					printf("Failed to download leaderboard\n");
				}
				app->state = LSS_STATE_GAME_RESULTS;
			}
			break;
		}
		case LSS_STATE_GAME_RESULTS:
		{
			lss_game_results_logic(app);
			lss_read_controller(&app->controller[0]);
			if(t3f_key[ALLEGRO_KEY_ESCAPE] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
			{
				app->state = LSS_STATE_SONG_SELECT;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			break;
		}
		case LSS_STATE_AV_SETUP:
		{
			lss_state_av_setup_logic(app);
			if(app->game.done)
			{
				app->state = LSS_STATE_TITLE;
			}
			break;
		}
	}
}

void lss_state_render(APP_INSTANCE * app)
{
	al_clear_to_color(t3f_color_black);
	switch(app->state)
	{
		case LSS_STATE_TITLE:
		{
			lss_title_render(&app->title, &app->resources);
			break;
		}
		case LSS_STATE_SONG_SELECT:
		{
			lss_state_song_list_song_select_render(app);
			break;
		}
		case LSS_STATE_SONG_SELECT_TRACK:
		{
			lss_state_song_list_track_select_render(app);
			break;
		}
		case LSS_STATE_SONG_SELECT_DIFFICULTY:
		{
			lss_state_song_list_difficulty_select_render(app);
			break;
		}
		case LSS_STATE_GAME:
		{
			lss_game_render(&app->game, &app->resources);
			break;
		}
		case LSS_STATE_GAME_RESULTS:
		{
			lss_game_results_render(app);
			break;
		}
		case LSS_STATE_AV_SETUP:
		{
			lss_state_av_setup_render(app);
			break;
		}
	}
}
