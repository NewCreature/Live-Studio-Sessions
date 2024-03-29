#include "t3f/gui.h"
#include "t3f/resource.h"
#include "t3f/debug.h"
#include "t3f/android.h"
#ifndef T3F_ANDROID
	#include <allegro5/allegro_native_dialog.h>
#endif

#include "modules/song_list.h"
#include "modules/text_entry.h"
#include "modules/gui.h"

#include "instance.h"
#include "resources.h"

#include "state.h"
#include "state_title.h"
#include "state_game_setup.h"
#include "state_game.h"

static char lss_new_profile_buffer[256] = {0};

static bool lss_new_profile_entry_callback(void * data, int c);

void lss_select_menu(LSS_TITLE_DATA * tp, int menu)
{
	tp->current_menu = menu;
	tp->menu[tp->current_menu]->hover_element = -1;
	t3f_select_next_gui_element(tp->menu[tp->current_menu]);
}

#ifdef ALLEGRO_ANDROID
	static void lss_menu_proc_network_id_callback(void * data)
	{
		APP_INSTANCE * app = (APP_INSTANCE *)data;
		if(strlen(lss_new_profile_buffer) <= 0)
		{
			strcpy(lss_new_profile_buffer, "Anonymous");
		}
		lss_new_profile_entry_callback(data, '\r');
	}
#endif

/* main menu */
int lss_menu_proc_play(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_PLAY);
	return 1;
}

int lss_menu_proc_play_quick_play(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_create_profiles_menu(app);
	lss_select_menu(&app->title, LSS_MENU_PROFILES);
	return 1;
}

int lss_menu_proc_play_career(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	return 1;
}

int lss_menu_proc_play_back(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_MAIN);
	return 1;
}

int lss_menu_proc_options(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_OPTIONS);
	return 1;
}

int lss_menu_proc_quit(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_CONFIRM_EXIT);
	return 1;
}

/* profiles menu */
int lss_menu_proc_profiles_select(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int j;

	for(j = 0; j < LSS_MAX_PLAYERS; j++)
	{
		app->game.player[j].profile = &app->profiles->entry[i];
	}
	t3f_clear_touch_data();
	lss_state_song_list_initialize(app);
	return 1;
}

int lss_menu_proc_profiles_new(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	#ifdef ALLEGRO_ANDROID
		t3f_open_edit_box("Enter Name", lss_new_profile_buffer, 32, "CapWords", lss_menu_proc_network_id_callback, data);
	#else
		lss_begin_text_entry(data, "", lss_new_profile_buffer, 32, lss_new_profile_entry_callback);
	#endif
	lss_select_menu(&app->title, LSS_MENU_NEW_PROFILE);
	return 1;
}

int lss_menu_proc_profiles_back(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_MAIN);
	return 1;
}

static bool lss_is_string_empty(const char * text)
{
	int i;

	if(strlen(text) < 1)
	{
		return true;
	}
	for(i = 0; i < (int)strlen(text); i++)
	{
		if(text[i] != ' ')
		{
			return false;
		}
	}
	return true;
}

int lss_menu_proc_profiles_new_ok(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	#ifndef ALLEGRO_ANDROID
		lss_end_text_entry();
	#endif
	if(!lss_is_string_empty(lss_new_profile_buffer))
	{
		app->profiles->entry[app->profiles->entries].config = al_create_config();
		if(app->profiles->entry[app->profiles->entries].config)
		{
			al_set_config_value(app->profiles->entry[app->profiles->entries].config, "Settings", "Name", lss_new_profile_buffer);
			app->profiles->entry[app->profiles->entries].name = al_get_config_value(app->profiles->entry[app->profiles->entries].config, "Settings", "Name");
			app->profiles->entries++;
		}
	}
	lss_create_profiles_menu(app);
	lss_select_menu(&app->title, LSS_MENU_PROFILES);
	return 1;
}

int lss_menu_proc_profiles_new_cancel(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	#ifndef ALLEGRO_ANDROID
		lss_end_text_entry();
	#endif
	lss_select_menu(&app->title, LSS_MENU_PROFILES);
	return 1;
}

static bool lss_new_profile_entry_callback(void * data, int c)
{
	if(c == '\r')
	{
		lss_menu_proc_profiles_new_ok(data, 0, NULL);
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	return true;
}

/* options menu */
int lss_menu_proc_options_controllers(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(lss_create_controller_menu(app))
	{
		lss_select_menu(&app->title, LSS_MENU_CONTROLLER);
	}
	return 1;
}

int lss_menu_proc_options_av_setup(void * data, int ip, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i, j;

	app->game.song = lss_load_song(app->song_list->entry[0]->path);
	if(app->game.song)
	{
		for(i = 0; i < app->game.song->source_midi->tracks; i++)
		{
			for(j = 0; j < 4; j++)
			{
				if(app->game.song->track[i][j].notes > 5)
				{
					app->game.player[0].selected_track = i;
					app->game.player[0].selected_difficulty = j;
					i = 100;
					break;
				}
			}
		}
		app->game.song_id = app->song_list->entry[0]->id;
		app->game.player[0].profile = &app->profiles->entry[0];
		app->game.player[0].controller = &app->controller[0];
		if(lss_game_initialize(&app->game, app->song_list->entry[0]->path, &app->resources))
		{
			lss_title_exit(&app->title);
			t3f_clear_touch_data();
			al_hide_mouse_cursor(t3f_display);
			app->state = LSS_STATE_AV_SETUP;
		}
	}
	return 1;
}

#ifndef T3F_ANDROID
int lss_menu_proc_options_library(void * data, int ip, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	ALLEGRO_FILECHOOSER * fc;
	ALLEGRO_PATH * pp = NULL, * ipp, * fpp;
	char buf[1024];
	const char * pc;
	int f = 0;
	int c = 0;

	al_stop_timer(t3f_timer);
	fc = al_create_native_file_dialog(al_get_config_value(t3f_config, "Live Studio Sessions", "Library Path"), "Choose Song Library Location", NULL, ALLEGRO_FILECHOOSER_FOLDER);
	if(fc)
	{
		if(al_show_native_file_dialog(t3f_display, fc))
		{
			if(al_get_native_file_dialog_count(fc))
			{
				pc = al_get_native_file_dialog_path(fc, 0);
				if(pc)
				{
					pp = al_create_path(pc);
				}
				if(pp)
				{
					ipp = al_create_path("data/songs_copyright");
					fpp = al_create_path("data/songs");
					if(ipp && fpp)
					{
						lss_destroy_song_list(app->song_list);
						f = lss_song_list_count_files(al_path_cstr(fpp, '/'), 0);
						f += lss_song_list_count_files(al_path_cstr(ipp, '/'), 0);
						f += lss_song_list_count_files(al_path_cstr(pp, '/'), 0);
						c = lss_song_list_count_collections(al_path_cstr(fpp, '/'), 0);
						c += lss_song_list_count_collections(al_path_cstr(ipp, '/'), 0);
						c += lss_song_list_count_collections(al_path_cstr(pp, '/'), 0);
						app->song_list = lss_create_song_list(t3f_get_filename(t3f_data_path, "song_list.cache", buf, 1024), f, c);
						if(app->song_list)
						{
							lss_song_list_add_files(app->song_list, fpp, 0);
							lss_song_list_add_files(app->song_list, ipp, 0);
							lss_song_list_add_files(app->song_list, pp, 0);
							lss_song_list_collect_files(app->song_list);
							lss_song_list_sort(app->song_list, 0, NULL);
						}
						al_set_config_value(t3f_config, "Live Studio Sessions", "Library Path", al_path_cstr(pp, '/'));
						al_destroy_path(ipp);
					}
					al_destroy_path(pp);
				}
			}
		}
		al_destroy_native_file_dialog(fc);
	}
	al_start_timer(t3f_timer);
	return 1;
}
#endif

int lss_menu_proc_options_back(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_MAIN);
	return 1;
}

/* controllers menu */
int lss_menu_proc_options_controller_set(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	al_stop_timer(t3f_timer);
//	t3f_bind_controller(app->controller[0].controller, i - 1);
//	t3f_write_controller_config(t3f_config, "LSS Guitar", app->controller[0].controller);
	t3f_destroy_gui(app->title.menu[LSS_MENU_CONTROLLER]);
	lss_create_controller_menu(app);
	app->title.menu[LSS_MENU_CONTROLLER]->hover_element = i;
	app->title.block_count = 2;
	al_start_timer(t3f_timer);
	t3f_process_events(true);
	t3f_key[ALLEGRO_KEY_ENTER] = 0;
	return 1;
}

int lss_menu_proc_options_controller_back(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_OPTIONS);
	return 1;
}

int lss_menu_proc_exit_yes(void * data, int i, void * p)
{
	t3f_exit();
	return 1;
}

int lss_menu_proc_exit_no(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_MAIN);
	return 1;
}

bool lss_create_profiles_menu(APP_INSTANCE * app)
{
	int i, pos, space;
	char name_buf[256];

	space = t3f_get_font_line_height(app->resources.font[LSS_FONT_LARGE]);
	if(app->title.menu[LSS_MENU_PROFILES])
	{
		t3f_destroy_gui(app->title.menu[LSS_MENU_PROFILES]);
	}
	app->title.menu[LSS_MENU_PROFILES] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_PROFILES])
	{
		return false;
	}
	pos = 0;
//	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], NULL, "Live Studio Sessions - Profiles", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	for(i = 0; i < app->profiles->entries; i++)
	{
		strcpy(name_buf, app->profiles->entry[i].name);
		t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_select, name_buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
		pos += space;
	}
	if(app->profiles->entries < LSS_MAX_PROFILES)
	{
		t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_new, "New Profile", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
		pos += space;
	}
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_back, "Back", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_PROFILES], LSS_MENU_TOP, LSS_MENU_BOTTOM);
	return true;
}

bool lss_create_controller_menu(APP_INSTANCE * app)
{
	int pos, space;
	char buf[256];

	space = t3f_get_font_line_height(app->resources.font[LSS_FONT_LARGE]);

	/* controllers menu */
	app->title.menu[LSS_MENU_CONTROLLER] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_CONTROLLER])
	{
		return false;
	}
	pos = 0;
/*	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], NULL, "Live Studio Sessions - Controller Setup", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	sprintf(buf, "Green Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_GREEN), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_GREEN));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Red Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_RED), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_RED));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Yellow Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_YELLOW), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_YELLOW));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Blue Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_BLUE), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_BLUE));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Orange Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_ORANGE), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_ORANGE));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Strum Down (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Strum Up (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Strum Fast (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_FAST), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_FAST));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	sprintf(buf, "Menu (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_MENU), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_MENU));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_back, "Back", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW); */
	return true;
}

bool lss_title_initialize(APP_INSTANCE * app)
{
	int pos, space;
	int i;
	t3f_set_gui_driver(NULL);
	memset(app->title.menu, 0, sizeof(T3F_GUI *) * LSS_MAX_MENUS);

	space = t3f_get_font_line_height(app->resources.font[LSS_FONT_LARGE]);

	t3f_srand(&app->title.rng, time(0));
	app->title.bg_bitmap = t3f_load_resource((void *)(&app->title.bg_bitmap), t3f_bitmap_resource_handler_proc, "data/title_bg.png", 0, 0, 0);
	if(!app->title.bg_bitmap)
	{
//		return false;
	}
	app->title.logo_bitmap = t3f_load_resource((void *)(&app->title.logo_bitmap), t3f_bitmap_resource_handler_proc, "data/logo.png", 0, 0, 0);
	if(!app->title.logo_bitmap)
	{
//		return false;
	}

	/* main menu */
	app->title.menu[LSS_MENU_MAIN] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_MAIN])
	{
		return false;
	}
	pos = 0;
//	t3f_add_gui_text_element(app->title.menu[LSS_MENU_MAIN], NULL, "Live Studio Sessions", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_MAIN], lss_menu_proc_play, "Play", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_MAIN], lss_menu_proc_options, "Options", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_MAIN], lss_menu_proc_quit, "Quit", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_MAIN], LSS_MENU_TOP, LSS_MENU_BOTTOM);

	app->title.menu[LSS_MENU_PLAY] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_PLAY])
	{
		return false;
	}
	pos = 0;
//	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PLAY], NULL, "Live Studio Sessions - Play", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PLAY], lss_menu_proc_play_quick_play, "Quick Play", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PLAY], lss_menu_proc_play_career, "Career", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PLAY], lss_menu_proc_play_back, "Back", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_PLAY], LSS_MENU_TOP, LSS_MENU_BOTTOM);

	/* new profile menu */
	app->title.menu[LSS_MENU_NEW_PROFILE] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_NEW_PROFILE])
	{
		return false;
	}
	pos = 0;
//	t3f_add_gui_text_element(app->title.menu[LSS_MENU_NEW_PROFILE], NULL, "Live Studio Sessions - New Profile", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_NEW_PROFILE], NULL, lss_new_profile_buffer, (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_NEW_PROFILE], lss_menu_proc_profiles_new_ok, "Okay", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_NEW_PROFILE], lss_menu_proc_profiles_new_cancel, "Cancel", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_NEW_PROFILE], LSS_MENU_TOP, LSS_MENU_BOTTOM);

	/* options menu */
	app->title.menu[LSS_MENU_OPTIONS] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_OPTIONS])
	{
		return false;
	}
	pos = 0;
//	t3f_add_gui_text_element(app->title.menu[LSS_MENU_OPTIONS], NULL, "Live Studio Sessions - Options", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_OPTIONS], lss_menu_proc_options_controllers, "Controllers", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_OPTIONS], lss_menu_proc_options_av_setup, "A/V Setup", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	#ifndef T3F_ANDROID
		t3f_add_gui_text_element(app->title.menu[LSS_MENU_OPTIONS], lss_menu_proc_options_library, "Select Library Folder", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
		pos += space;
	#endif
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_OPTIONS], lss_menu_proc_options_back, "Back", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_OPTIONS], LSS_MENU_TOP, LSS_MENU_BOTTOM);

	/* confirm exit menu */
	app->title.menu[LSS_MENU_CONFIRM_EXIT] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_CONFIRM_EXIT])
	{
		return false;
	}
	pos = 0;
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONFIRM_EXIT], NULL, "Exit to OS?", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += space * 2;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONFIRM_EXIT], lss_menu_proc_exit_yes, "Yes", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	pos += space;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONFIRM_EXIT], lss_menu_proc_exit_no, "No", (void **)&app->resources.font[LSS_FONT_LARGE], t3f_default_view->left + LSS_MENU_LEFT, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_center_gui(app->title.menu[LSS_MENU_CONFIRM_EXIT], LSS_MENU_TOP, LSS_MENU_BOTTOM);

	/* set up controllers */
	for(i = 0; i < al_get_num_joysticks() && i < LSS_MAX_CONTROLLERS; i++)
	{
		t3f_map_input_for_xbox_controller(app->controller[i].input, i);
	}
	if(i < LSS_MAX_CONTROLLERS)
	{
		lss_map_keyboard_menu_controls(&app->controller[i]);
	}

	/* start song audio */
	app->title.song_audio = lss_load_song_audio(app->song_list->entry[t3f_rand(&app->title.rng) % app->song_list->entries]->path);
	if(app->title.song_audio)
	{
		lss_set_song_audio_playing(app->title.song_audio, true);
	}

	return true;
}

void lss_title_exit(LSS_TITLE_DATA * dp)
{
	int i;

	if(dp->song_audio)
	{
		lss_destroy_song_audio(dp->song_audio);
		dp->song_audio = NULL;
	}
	if(dp->logo_bitmap)
	{
		t3f_destroy_resource(dp->logo_bitmap);
		dp->logo_bitmap = NULL;
	}
	if(dp->bg_bitmap)
	{
		t3f_destroy_resource(dp->bg_bitmap);
		dp->bg_bitmap = NULL;
	}
	for(i = 0; i < LSS_MAX_MENUS; i++)
	{
		if(dp->menu[i])
		{
			t3f_destroy_gui(dp->menu[i]);
			dp->menu[i] = NULL;
		}
	}
}

void lss_title_menu_logic(T3F_GUI * gp, LSS_CONTROLLER * cp, int block_count, bool back_enabled, void * data)
{
	t3f_process_gui(gp, data);
	if(!block_count)
	{
		if(!lss_process_text_entry())
		{
			if(cp->input->element[T3F_GAMEPAD_A].pressed)
			{
				t3f_activate_selected_gui_element(gp, data);
			}
			else if(cp->input->element[T3F_GAMEPAD_DPAD_UP].pressed)
			{
				t3f_select_previous_gui_element(gp);
			}
			else if(cp->input->element[T3F_GAMEPAD_DPAD_DOWN].pressed)
			{
				t3f_select_next_gui_element(gp);
			}
		}
		if(t3f_key[ALLEGRO_KEY_BACK] || cp->input->element[T3F_GAMEPAD_START].pressed)
		{
			if(back_enabled)
			{
				gp->hover_element = 0;
				t3f_select_previous_gui_element(gp);
				t3f_activate_selected_gui_element(gp, data);
			}
			t3f_key[ALLEGRO_KEY_BACK] = 0;
		}
	}
}

void lss_title_logic(LSS_TITLE_DATA * dp, APP_INSTANCE * app)
{
	if(dp->block_count > 0)
	{
		dp->block_count--;
	}
	if(dp->current_menu >= 0)
	{
		lss_title_menu_logic(dp->menu[dp->current_menu], &app->controller[0], app->title.block_count, true, app);
		/* only update the GUI colors if we are still on the title screen */
		if(app->state == LSS_STATE_TITLE)
		{
			lss_update_gui_colors(dp->menu[dp->current_menu], LSS_TITLE_COLOR_HEADER, LSS_TITLE_COLOR_SELECTED, LSS_TITLE_COLOR_NORMAL);
		}
	}
}

void lss_title_render(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp)
{
	float w = t3f_default_view->right - t3f_default_view->left;
	float s;
	if(dp->bg_bitmap)
	{
		al_draw_bitmap(dp->bg_bitmap, 480 - al_get_bitmap_width(dp->bg_bitmap) / 2, 270 - al_get_bitmap_height(dp->bg_bitmap) / 2, 0);
	}
	else
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG);
	}
	if(dp->logo_bitmap)
	{
		s = w / al_get_bitmap_width(dp->logo_bitmap);
		t3f_draw_scaled_bitmap(dp->logo_bitmap, t3f_color_white, t3f_default_view->left, t3f_default_view->top, 0, al_get_bitmap_width(dp->logo_bitmap) * s, al_get_bitmap_height(dp->logo_bitmap) * s, 0);
	}
	if(dp->current_menu >= 0)
	{
		t3f_render_gui(dp->menu[dp->current_menu]);
	}
}
