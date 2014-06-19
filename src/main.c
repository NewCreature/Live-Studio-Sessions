#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/resource.h"
#include "t3f/view.h"

#include "rtk/midi.h"

#include "instance.h"
#include "state.h"
#include "song_list.h"
#include "song_audio.h"
#include "song.h"
#include "player.h"
#include "game.h"
#include "resources.h"
#include "controller.h"

/* main logic routine */
void app_logic(void * data)
{
	lss_state_logic((APP_INSTANCE *)data);
}

/* main rendering routine */
void app_render(void * data)
{
	lss_state_render((APP_INSTANCE *)data);
}

static bool lss_setup_default_controllers(APP_INSTANCE * app)
{
	app->controller[0].controller = t3f_create_controller(8);
	if(!app->controller[0].controller)
	{
		return false;
	}
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_GREEN].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_GREEN].button = ALLEGRO_KEY_F1;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_YELLOW].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_YELLOW].button = ALLEGRO_KEY_F2;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_RED].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_RED].button = ALLEGRO_KEY_F3;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_BLUE].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_BLUE].button = ALLEGRO_KEY_F4;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_ORANGE].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_ORANGE].button = ALLEGRO_KEY_F5;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].button = ALLEGRO_KEY_LSHIFT;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].button = ALLEGRO_KEY_ENTER;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STAR_POWER].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STAR_POWER].button = ALLEGRO_KEY_SPACE;
	return true;
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int f;
	ALLEGRO_PATH * included_songs_path;
	ALLEGRO_PATH * songs_path;
	const char * val;
	
	/* initialize T3F */
	if(!t3f_initialize("Live Studio Sessions", 960, 540, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	if(!lss_setup_default_controllers(app))
	{
		printf("Could not set up controllers!\n");
		return false;
	}
	if(!lss_load_global_resources(&app->resources))
	{
		printf("Could not load global resources!\n");
		return false;
	}
	
	/* create song database */
	included_songs_path = al_create_path("data/songs");
	if(!included_songs_path)
	{
		return false;
	}
	if(argc > 1)
	{
		songs_path = al_create_path(argv[1]);
		al_set_config_value(t3f_config, "Live Studio Sessions", "Library Path", argv[1]);
	}
	else
	{
		val = al_get_config_value(t3f_config, "Live Studio Sessions", "Library Path");
		if(val)
		{
			songs_path = al_create_path(val);
		}
		else
		{
			printf("Could not get library path!\n");
			return false;
		}
	}
	f = lss_song_list_count_files(al_path_cstr(included_songs_path, '/'), 0);
	f += lss_song_list_count_files(al_path_cstr(songs_path, '/'), 0);
	printf("Found %d songs!\n", f);
	app->song_list = lss_create_song_list(t3f_get_filename(t3f_data_path, "song_list.cache"), f);
	if(!app->song_list)
	{
		printf("Could create song list!\n");
		return false;
	}
	lss_song_list_add_files(app->song_list, included_songs_path, 0);
	lss_song_list_add_files(app->song_list, songs_path, 0);
//	app_load_song(app);
//	app->current_event = 0;
//	app->current_event_tick = (app->midi->track[1]->event[app->current_event]->pos_sec + app->offset) * 60.0;
	app->state = LSS_STATE_SONG_SELECT;
	app->game.av_delay = 15;
	app->selected_song = 0;
	app->game.player[0].selected_track = 0;
	app->game.player[0].selected_difficulty = 0;
	return true;
}

void app_exit(APP_INSTANCE * app)
{
	lss_free_global_resources(&app->resources);
}

int main(int argc, char * argv[])
{
	APP_INSTANCE app;
	
	if(app_initialize(&app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	app_exit(&app);
	return 0;
}
