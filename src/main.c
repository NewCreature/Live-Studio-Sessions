#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/resource.h"
#include "t3f/view.h"

#include "rtk/midi.h"

#include "song_list.h"
#include "song_audio.h"
#include "song.h"
#include "player.h"
#include "game.h"
#include "resources.h"
#include "controller.h"

/* structure to hold all of our app-specific data */
typedef struct
{
	
	/* global data */
	LSS_RESOURCES resources;
	LSS_CONTROLLER controller[LSS_MAX_CONTROLLERS];

	LSS_SONG_LIST * song_list;
	
	/* menu data */
	int selected_song;
	
	int state;
	
	LSS_GAME game;
	
} APP_INSTANCE;

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(app->state)
	{
		case 0:
		{
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->selected_song--;
				if(app->selected_song < 0)
				{
					app->selected_song = app->song_list->entries - 1;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->selected_song++;
				if(app->selected_song >= app->song_list->entries)
				{
					app->selected_song = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->game.song = lss_load_song(app->song_list->entry[app->selected_song]->path);
				app->state = 1;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case 1:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				lss_destroy_song(app->game.song);
				app->state = 0;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->game.player[0].selected_track--;
				if(app->game.player[0].selected_track < 0)
				{
					app->game.player[0].selected_track = app->game.song->source_midi->tracks - 1;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->game.player[0].selected_track++;
				if(app->game.player[0].selected_track >= app->game.song->source_midi->tracks)
				{
					app->game.player[0].selected_track = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->state = 2;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case 2:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				app->state = 1;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->game.player[0].selected_difficulty--;
				if(app->game.player[0].selected_difficulty < 0)
				{
					app->game.player[0].selected_difficulty = 3;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->game.player[0].selected_difficulty++;
				if(app->game.player[0].selected_difficulty > 3)
				{
					app->game.player[0].selected_difficulty = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->game.player[0].controller = &app->controller[0];
				lss_game_initialize(&app->game, app->song_list->entry[app->selected_song]->path);
				app->state = 3;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case 3:
		{
			lss_game_logic(&app->game);
			if(app->game.done)
			{
				app->state = 0;
			}
			break;
		}
	}
}

/* main rendering routine */
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;
	ALLEGRO_COLOR color;
	char * difficulty_text[4] = {"Easy", "Medium", "Hard", "Expert"};

	al_clear_to_color(t3f_color_black);
	al_hold_bitmap_drawing(true);
	switch(app->state)
	{
		case 0:
		{
			for(i = 0; i < 20 && i + app->selected_song < app->song_list->entries; i++)
			{
				if(i == 0)
				{
					color = t3f_color_white;
				}
				else
				{
					color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
				}
				al_draw_textf(app->resources.font, color, 0, i * 20, 0, "%s", al_path_cstr(app->song_list->entry[app->selected_song + i]->path, '/'));
			}
			break;
		}
		case 1:
		{
			for(i = 0; i < app->game.song->source_midi->tracks; i++)
			{
				if(i == app->game.player[0].selected_track)
				{
					color = t3f_color_white;
				}
				else
				{
					color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
				}
				al_draw_textf(app->resources.font, color, 0, i * 20, 0, "%s", app->game.song->source_midi->track[i]->name);
			}
			break;
		}
		case 2:
		{
			for(i = 0; i < 4; i++)
			{
				if(i == app->game.player[0].selected_difficulty)
				{
					color = t3f_color_white;
				}
				else
				{
					color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
				}
				al_draw_textf(app->resources.font, color, 0, i * 20, 0, "%s (%d)", difficulty_text[i], app->game.song->track[app->game.player[0].selected_track][i].note_count);
			}
			break;
		}
		case 3:
		{
			lss_game_render(&app->game);
			break;
		}
	}
	al_hold_bitmap_drawing(false);
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
	app->controller[0].controller->binding[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].button = ALLEGRO_KEY_RSHIFT;
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
	ALLEGRO_PATH * songs_path;
	const char * val;
	
	/* initialize T3F */
	if(!t3f_initialize("Live Studio Sessions", 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	if(!lss_setup_default_controllers(app))
	{
		return false;
	}
	if(!lss_load_global_resources(&app->resources))
	{
		return false;
	}
	
	/* create song database */
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
			return false;
		}
	}
	f = lss_song_list_count_files(al_path_cstr(songs_path, '/'), 0);
	printf("Found %d songs!\n", f);
	app->song_list = lss_create_song_list(t3f_get_filename(t3f_data_path, "song_list.cache"), f);
	if(!app->song_list)
	{
		return false;
	}
	lss_song_list_add_files(app->song_list, songs_path, 0);
//	app_load_song(app);
//	app->current_event = 0;
//	app->current_event_tick = (app->midi->track[1]->event[app->current_event]->pos_sec + app->offset) * 60.0;
	app->state = 0;
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
