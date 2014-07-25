#include "t3f/gui.h"

#include "modules/text_entry.h"

#include "instance.h"
#include "resources.h"

#include "state.h"
#include "state_title.h"
#include "state_game.h"

static char lss_new_profile_buffer[256] = {0};

static bool lss_new_profile_entry_callback(void * data, int c);

void lss_select_menu(LSS_TITLE_DATA * tp, int menu)
{
	tp->current_menu = menu;
	tp->menu[tp->current_menu]->hover_element = -1;
	t3f_select_next_gui_element(tp->menu[tp->current_menu]);
}

/* main menu */
int lss_menu_proc_play(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_create_profiles_menu(app);
	lss_select_menu(&app->title, LSS_MENU_PROFILES);
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
	t3f_exit();
	return 1;
}

/* profiles menu */
int lss_menu_proc_profiles_select(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	
	app->game.player[0].profile = &app->profiles->entry[i - 1];
	t3f_clear_touch_data();
	app->state = LSS_STATE_SONG_SELECT;
	return 1;
}

int lss_menu_proc_profiles_new(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	
	lss_begin_text_entry(data, "", lss_new_profile_buffer, 32, lss_new_profile_entry_callback);
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

	lss_end_text_entry();
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

	lss_end_text_entry();
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
		if(lss_game_initialize(&app->game, app->song_list->entry[0]->path))
		{
			app->state = LSS_STATE_AV_SETUP;
		}
	}
	return 1;
}

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
	t3f_bind_controller(app->controller[0].controller, i - 1);
	t3f_write_controller_config(t3f_config, "LSS Guitar", app->controller[0].controller);
	t3f_destroy_gui(app->title.menu[LSS_MENU_CONTROLLER]);
	lss_create_controller_menu(app);
	app->title.menu[LSS_MENU_CONTROLLER]->hover_element = i;
	app->title.block_count = 2;
	al_start_timer(t3f_timer);
	return 1;
}

int lss_menu_proc_options_controller_back(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	lss_select_menu(&app->title, LSS_MENU_OPTIONS);
	return 1;
}

bool lss_create_profiles_menu(APP_INSTANCE * app)
{
	int i, pos;
	char name_buf[256];

	app->title.menu[LSS_MENU_PROFILES] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_PROFILES])
	{
		return false;
	}
	pos = 0;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], NULL, "Live Studio Sessions - Profiles", app->resources.font[LSS_FONT_LARGE], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += 48;
	for(i = 0; i < app->profiles->entries; i++)
	{
		strcpy(name_buf, app->profiles->entry[i].name);
		t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_select, name_buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
		pos += 24;
	}
	if(app->profiles->entries < LSS_MAX_PROFILES)
	{
		t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_new, "New Profile", app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
		pos += 24;
	}
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_PROFILES], lss_menu_proc_profiles_back, "Back", app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	return true;
}

bool lss_create_controller_menu(APP_INSTANCE * app)
{
	int pos;
	char buf[256];

	/* controllers menu */
	app->title.menu[LSS_MENU_CONTROLLER] = t3f_create_gui(0, 0);
	if(!app->title.menu[LSS_MENU_CONTROLLER])
	{
		return false;
	}
	pos = 0;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], NULL, "Live Studio Sessions - Controller Setup", app->resources.font[LSS_FONT_LARGE], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	pos += 48;
	sprintf(buf, "Green Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_GREEN), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_GREEN));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Red Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_RED), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_RED));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Yellow Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_YELLOW), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_YELLOW));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Blue Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_BLUE), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_BLUE));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Orange Fret (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_ORANGE), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_ORANGE));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Strum Down (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	sprintf(buf, "Strum Up (%s - %s)", t3f_get_controller_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP), t3f_get_controller_binding_name(app->controller[0].controller, LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP));
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_set, buf, app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += 24;
	t3f_add_gui_text_element(app->title.menu[LSS_MENU_CONTROLLER], lss_menu_proc_options_controller_back, "Back", app->resources.font[LSS_FONT_SMALL], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	return true;
}

bool lss_title_initialize(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp)
{
	t3f_set_gui_driver(NULL);
	memset(dp->menu, 0, sizeof(T3F_GUI *) * LSS_MAX_MENUS);
	
	/* main menu */
	dp->menu[LSS_MENU_MAIN] = t3f_create_gui(0, 0);
	if(!dp->menu[LSS_MENU_MAIN])
	{
		return false;
	}
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], NULL, "Live Studio Sessions", rp->font[LSS_FONT_LARGE], 8, 0, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_play, "Play", rp->font[LSS_FONT_SMALL], 8, 48, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_options, "Options", rp->font[LSS_FONT_SMALL], 8, 72, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_quit, "Quit", rp->font[LSS_FONT_SMALL], 8, 96, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	
	/* new profile menu */
	dp->menu[LSS_MENU_NEW_PROFILE] = t3f_create_gui(0, 0);
	if(!dp->menu[LSS_MENU_NEW_PROFILE])
	{
		return false;
	}
	t3f_add_gui_text_element(dp->menu[LSS_MENU_NEW_PROFILE], NULL, "Live Studio Sessions - New Profile", rp->font[LSS_FONT_LARGE], 8, 0, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_NEW_PROFILE], NULL, lss_new_profile_buffer, rp->font[LSS_FONT_SMALL], 8, 48, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_NEW_PROFILE], lss_menu_proc_profiles_new_ok, "Okay", rp->font[LSS_FONT_SMALL], 8, 72, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_NEW_PROFILE], lss_menu_proc_profiles_new_cancel, "Cancel", rp->font[LSS_FONT_SMALL], 8, 96, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	
	/* options menu */
	dp->menu[LSS_MENU_OPTIONS] = t3f_create_gui(0, 0);
	if(!dp->menu[LSS_MENU_OPTIONS])
	{
		return false;
	}
	t3f_add_gui_text_element(dp->menu[LSS_MENU_OPTIONS], NULL, "Live Studio Sessions - Options", rp->font[LSS_FONT_LARGE], 8, 0, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_OPTIONS], lss_menu_proc_options_controllers, "Controllers", rp->font[LSS_FONT_SMALL], 8, 48, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_OPTIONS], lss_menu_proc_options_av_setup, "A/V Setup", rp->font[LSS_FONT_SMALL], 8, 72, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_OPTIONS], lss_menu_proc_options_back, "Back", rp->font[LSS_FONT_SMALL], 8, 96, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);

	return true;
}

void lss_title_exit(LSS_TITLE_DATA * dp)
{
	int i;
	
	for(i = 0; i < LSS_MAX_MENUS; i++)
	{
		if(dp->menu[i])
		{
			t3f_destroy_gui(dp->menu[i]);
			dp->menu[i] = NULL;
		}
	}
}

void lss_title_logic(LSS_TITLE_DATA * dp, APP_INSTANCE * app)
{
	if(dp->block_count > 0)
	{
		dp->block_count--;
	}
	if(!lss_process_text_entry())
	{
		if(dp->current_menu >= 0)
		{
			t3f_process_gui(dp->menu[dp->current_menu], app);
			lss_read_controller(&app->controller[0]);
			if(!dp->block_count)
			{
				if(t3f_key[ALLEGRO_KEY_ENTER] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed)
				{
					t3f_activate_selected_gui_element(dp->menu[dp->current_menu], app);
					t3f_key[ALLEGRO_KEY_ENTER] = 0;
				}
				else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
				{
					t3f_select_previous_gui_element(dp->menu[dp->current_menu]);
					t3f_key[ALLEGRO_KEY_UP] = 0;
				}
				else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
				{
					t3f_select_next_gui_element(dp->menu[dp->current_menu]);
					t3f_key[ALLEGRO_KEY_DOWN] = 0;
				}
				else if(t3f_key[ALLEGRO_KEY_ESCAPE] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
				{
					dp->menu[dp->current_menu]->hover_element = 0;
					t3f_select_previous_gui_element(dp->menu[dp->current_menu]);
					t3f_activate_selected_gui_element(dp->menu[dp->current_menu], app);
					t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
				}
			}
		}
	}
}

void lss_title_render(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp)
{
	if(dp->current_menu >= 0)
	{
		t3f_render_gui(dp->menu[dp->current_menu]);
	}
}
