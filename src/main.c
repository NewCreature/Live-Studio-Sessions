#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/debug.h"
#ifndef T3F_ANDROID
	#include <allegro5/allegro_native_dialog.h>
#endif

#include "rtk/midi.h"
#include "rtk/io_allegro.h"
#include "modules/song_list.h"
#include "modules/song_audio.h"
#include "modules/song.h"
#include "modules/controller.h"

#include "instance.h"
#include "resources.h"

#include "state.h"
#include "state_game.h"
#include "state_title.h"

/* main logic routine */
void app_logic(void * data)
{
	bool toggle_fullscreen = false;
	int flags;
	int ret;

	if(t3f_key[ALLEGRO_KEY_COMMAND] && (t3f_key[ALLEGRO_KEY_LSHIFT] || t3f_key[ALLEGRO_KEY_RSHIFT]) && t3f_key[ALLEGRO_KEY_F])
	{
		t3f_key[ALLEGRO_KEY_F] = 0;
		toggle_fullscreen = true;
	}
	else if((t3f_key[ALLEGRO_KEY_LCTRL] || t3f_key[ALLEGRO_KEY_RCTRL]) && t3f_key[ALLEGRO_KEY_F])
	{
		t3f_key[ALLEGRO_KEY_F] = 0;
		toggle_fullscreen = true;
	}
	else if((t3f_key[ALLEGRO_KEY_ALT] || t3f_key[ALLEGRO_KEY_ALTGR]) && t3f_key[ALLEGRO_KEY_ENTER])
	{
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
		toggle_fullscreen = true;
	}
	if(toggle_fullscreen)
	{
		flags = t3f_flags;
		flags ^= T3F_USE_FULLSCREEN;
		ret = t3f_set_gfx_mode(960, 540, flags);
		if(!ret || ret == 2)
		{
			t3f_unload_resources();
			al_destroy_display(t3f_display);
			t3f_display = NULL;
			t3f_set_gfx_mode(960, 540, flags);
			t3f_reload_resources();
		}
	}
	lss_state_logic((APP_INSTANCE *)data);
}

/* main rendering routine */
void app_render(void * data)
{
	lss_state_render((APP_INSTANCE *)data);
}

static bool lss_setup_default_controllers(APP_INSTANCE * app)
{
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_GREEN].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_GREEN].button = ALLEGRO_KEY_F1;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_RED].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_RED].button = ALLEGRO_KEY_F2;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_YELLOW].type = T3F_CONTROLLER_BINDING_KEY;
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_YELLOW].button = ALLEGRO_KEY_F3;
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
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_MENU].button = ALLEGRO_KEY_BACKSPACE;
	return true;
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int f, c;
	char buf[1024];
	ALLEGRO_PATH * included_songs_path;
	ALLEGRO_PATH * free_songs_path;
	ALLEGRO_PATH * songs_path = NULL;
	const char * val;

	/* initialize T3F */
	if(!t3f_initialize("Live Studio Sessions", 960, 540, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	#ifdef T3F_DEBUG
		if(!t3f_open_debug_log("lss_debug.log"))
		{
			printf("Failed to open debug log!\n");
			return false;
		}
	#endif

	#ifndef T3F_ANDROID
		t3f_debug_message("Initializing dialog add-on...\n");
		if(!al_init_native_dialog_addon())
		{
			printf("Error initializing native dialog add-on!\n");
			return false;
		}
	#endif
	t3f_debug_message("Initializing controllers...\n");
	app->controller[0].controller = t3f_create_controller(9);
	if(!app->controller[0].controller)
	{
		printf("Could not set up controllers!\n");
		return false;
	}
	app->controller[0].type = LSS_CONTROLLER_TYPE_GUITAR;
	#ifndef T3F_ANDROID
		app->controller[0].source = LSS_CONTROLLER_SOURCE_CONTROLLER;
		if(!t3f_read_controller_config(t3f_config, "LSS Guitar", app->controller[0].controller))
		{
			lss_setup_default_controllers(app);
		}
	#else
		app->controller[0].source = LSS_CONTROLLER_SOURCE_TOUCH;
	#endif
	t3f_debug_message("Loading resources...\n");
	if(!lss_load_global_resources(&app->resources))
	{
		printf("Could not load global resources!\n");
		return false;
	}

	/* create song database */
	t3f_debug_message("Creating songs database...\n");
	free_songs_path = al_create_path("data/songs");
	if(!free_songs_path)
	{
		return false;
	}
	included_songs_path = al_create_path("data/songs_copyright");
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
	}
	f = lss_song_list_count_files(al_path_cstr(free_songs_path, '/'), 0);
	f += lss_song_list_count_files(al_path_cstr(included_songs_path, '/'), 0);
	if(songs_path)
	{
		f += lss_song_list_count_files(al_path_cstr(songs_path, '/'), 0);
	}
	if(f <= 0)
	{
		printf("No songs found!\n");
		return false;
	}
	c = lss_song_list_count_collections(al_path_cstr(free_songs_path, '/'), 0);
	c += lss_song_list_count_collections(al_path_cstr(included_songs_path, '/'), 0);
	if(songs_path)
	{
		c += lss_song_list_count_collections(al_path_cstr(songs_path, '/'), 0);
	}
	if(c <= 0)
	{
		printf("No collections found!\n");
	}
	app->song_list = lss_create_song_list(t3f_get_filename(t3f_data_path, "song_list.cache", buf, 1024), f, c);
	if(!app->song_list)
	{
		printf("Could create song list!\n");
		return false;
	}
	lss_song_list_add_files(app->song_list, free_songs_path, 0);
	lss_song_list_add_files(app->song_list, included_songs_path, 0);
	if(songs_path)
	{
		lss_song_list_add_files(app->song_list, songs_path, 0);
	}
	lss_song_list_collect_files(app->song_list);
	lss_song_list_sort(app->song_list, 0, NULL);

	t3f_debug_message("Loading profiles...\n");
	app->profiles = lss_load_profiles();
	if(!app->profiles)
	{
		printf("Could not load profiles!\n");
		return false;
	}
//	app_load_song(app);
//	app->current_event = 0;
//	app->current_event_tick = (app->midi->track[1]->event[app->current_event]->pos_sec + app->offset) * 60.0;
	t3f_debug_message("Initializing title screen...\n");
	if(!lss_title_initialize(&app->title, &app->resources, app->song_list))
	{
		return false;
	}
	app->title.block_count = 0;
	app->state = LSS_STATE_TITLE;
	t3f_debug_message("Loading configuration...\n");
	app->game.av_delay = 15;
	val = al_get_config_value(t3f_config, "Live Studio Sessions", "av_delay");
	if(val)
	{
		app->game.av_delay = atoi(val);
	}
	app->selected_song = 0;
	app->selected_track = 1;
	app->selected_difficulty = 3;
	app->leaderboard = NULL;
	app->game.player[0].selected_track = 0;
	app->game.player[0].selected_difficulty = 0;
	lss_select_menu(&app->title, LSS_MENU_MAIN);

	rtk_io_set_allegro_driver();

	t3f_debug_message("Done...\n");
	return true;
}

void app_exit(APP_INSTANCE * app)
{
	lss_title_exit(&app->title);
	lss_save_profiles(app->profiles);
	lss_destroy_profiles(app->profiles);
	lss_free_global_resources(&app->resources);
	lss_destroy_song_list(app->song_list);
	#ifdef T3F_DEBUG
		t3f_close_debug_log();
	#endif
}

int main(int argc, char * argv[])
{
	APP_INSTANCE * app;

	app = lss_create_instance();
	if(!app)
	{
		printf("Error: could not create instance!\n");
		return -1;
	}
	if(app_initialize(app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	if(app->state == LSS_STATE_GAME)
	{
		lss_game_exit(&app->game);
	}
	app_exit(app);
	lss_destroy_instance(app);
	return 0;
}
