#include "t3f/t3f.h"
#include "t3f/gui.h"

#include <ctype.h>
#include "modules/gui.h"
#include "modules/text_entry.h"
#include "modules/obfuscate.h"

#include "instance.h"
#include "state.h"
#include "state_game.h"
#include "state_title.h"

static T3F_GUI * lss_song_list_menu = NULL;
static int lss_track[16];
static int lss_tracks = 0;
static int lss_diff[4];
static int lss_diffs = 0;

static int lss_song_list_font = LSS_FONT_LARGE;
static int lss_song_list_space = 20;
static int lss_song_list_visible = 20;

static double lss_song_list_scroll_pos = 0.0;
static double lss_song_list_scroll_velocity = 0.0;
static double lss_song_list_touch_pos = 0.0;
static int lss_song_list_touch_id = -1;
static bool lss_song_list_selected = false;
static bool lss_song_list_tapped = false;
static bool lss_song_list_scrolled = false;
static int lss_song_list_touch_frames = 0;
static int lss_song_list_tap_frames = 0;
static bool lss_song_list_mouse_init = true;
static int lss_song_list_mouse_x = 0;
static int lss_song_list_mouse_y = 0;
static int lss_song_list_mouse_z = 0;
static bool lss_song_list_mouse_button = false;
static bool lss_song_list_clicked = false;
static int lss_song_list_sort_type = 0;
static char lss_song_list_sort_filter[32] = {0};
static bool lss_song_list_sort_query = false;
static int lss_song_list_collection = 0;

void lss_state_song_list_initialize(APP_INSTANCE * app)
{
	lss_song_list_mouse_init = true;
	app->state = LSS_STATE_SONG_SELECT;
}

static void lss_enumerate_tracks(LSS_SONG * sp)
{
	int i, j;

	lss_tracks = 0;
	for(i = 0; i < sp->source_midi->tracks; i++)
	{
		for(j = 0; j < 4; j++)
		{
			if(sp->track[i][j].notes > 5)
			{
				lss_track[lss_tracks] = i;
				lss_tracks++;
				break;
			}
		}
	}
}

static void lss_enumerate_difficulties(LSS_SONG * sp, int track)
{
	int i;

	lss_diffs = 0;
	for(i = 0; i < 4; i++)
	{
		if(sp->track[track][i].notes > 5)
		{
			lss_diff[lss_diffs] = i;
			lss_diffs++;
		}
	}
}

static int lss_song_list_proc_select_difficulty(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_destroy_gui(lss_song_list_menu);
	lss_song_list_menu = NULL;
	app->game.player[0].selected_difficulty = i - 2;
	app->game.song_id = app->song_list->entry[app->selected_song]->id;
	app->game.player[0].selected_track = lss_track[app->game.player[0].selected_track];
	app->game.player[0].selected_difficulty = lss_diff[app->game.player[0].selected_difficulty];
	app->game.player[0].controller = &app->controller[0];
	app->game.player[0].block_menu_strum = true;
	lss_title_exit(&app->title);
	if(lss_game_initialize(&app->game, app->song_list->entry[app->selected_song]->path, &app->resources))
	{
		al_hide_mouse_cursor(t3f_display);
		app->state = LSS_STATE_GAME;
	}
	else
	{
		lss_title_initialize(&app->title, &app->resources, app->song_list);
		printf("failed to initialize game\n");
	}
	return 1;
}

static bool lss_create_difficulty_menu(APP_INSTANCE * app)
{
	char * track_name = NULL;
	char * difficulty_text[4] = {"Easy", "Medium", "Hard", "Expert"};
	char buf[256] = {0};
	int i, pos = 0;

	lss_song_list_menu = t3f_create_gui(0, 0);
	if(!lss_song_list_menu)
	{
		return false;
	}
	sprintf(buf, "%s - %s", app->song_list->entry[app->selected_song]->artist, app->song_list->entry[app->selected_song]->title);
	t3f_add_gui_text_element(lss_song_list_menu, NULL, buf, (void **)&app->resources.font[lss_song_list_font], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += lss_song_list_space;
	track_name = app->game.song->source_midi->track[lss_track[app->game.player[0].selected_track]]->name;
	if(!track_name)
	{
		track_name = "No Name";
	}
	t3f_add_gui_text_element(lss_song_list_menu, NULL, track_name, (void **)&app->resources.font[lss_song_list_font], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += lss_song_list_space * 2;
	for(i = 0; i < lss_diffs; i++)
	{
		t3f_add_gui_text_element(lss_song_list_menu, lss_song_list_proc_select_difficulty, difficulty_text[lss_diff[i]], (void **)&app->resources.font[lss_song_list_font], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
		pos += lss_song_list_space;
	}
	lss_song_list_menu->hover_element = -1;
	t3f_select_next_gui_element(lss_song_list_menu);
	return true;
}

static int lss_song_list_proc_select_track(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	t3f_destroy_gui(lss_song_list_menu);
	lss_song_list_menu = NULL;
	app->game.player[0].selected_track = i - 1;
	lss_enumerate_difficulties(app->game.song, lss_track[app->game.player[0].selected_track]);
	lss_create_difficulty_menu(app);
	app->game.player[0].selected_difficulty = 0;
	app->state = LSS_STATE_SONG_SELECT_DIFFICULTY;
	return 1;
}

static bool lss_create_track_list_menu(APP_INSTANCE * app)
{
	char * track_name = NULL;
	char buf[256] = {0};
	int i, pos = 0;

	lss_song_list_menu = t3f_create_gui(0, 0);
	if(!lss_song_list_menu)
	{
		return false;
	}
	sprintf(buf, "%s - %s", app->song_list->entry[app->selected_song]->artist, app->song_list->entry[app->selected_song]->title);
	t3f_add_gui_text_element(lss_song_list_menu, NULL, buf, (void **)&app->resources.font[lss_song_list_font], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
	pos += lss_song_list_space * 2;
	for(i = 0; i < lss_tracks; i++)
	{
		track_name = app->game.song->source_midi->track[lss_track[i]]->name;
		if(!track_name)
		{
			track_name = "No Name";
		}
		t3f_add_gui_text_element(lss_song_list_menu, lss_song_list_proc_select_track, track_name, (void **)&app->resources.font[lss_song_list_font], 8, pos, t3f_color_white, T3F_GUI_ELEMENT_SHADOW | T3F_GUI_ELEMENT_COPY);
		pos += lss_song_list_space;
	}
	lss_song_list_menu->hover_element = -1;
	t3f_select_next_gui_element(lss_song_list_menu);
	return true;
}

/* detect mouse scroll */
static void lss_state_song_list_mouse_logic(APP_INSTANCE * app)
{
	int old_x, old_y, old_z, diff;
	bool old_b;

	lss_song_list_clicked = false;
	if(lss_song_list_mouse_init)
	{
		lss_song_list_mouse_x = t3f_mouse_x;
		old_x = lss_song_list_mouse_x;
		lss_song_list_mouse_y = t3f_mouse_y;
		old_y = lss_song_list_mouse_y;
		lss_song_list_mouse_z = t3f_mouse_z;
		old_z = lss_song_list_mouse_z;
		lss_song_list_mouse_button = t3f_mouse_button[0];
		old_b = lss_song_list_mouse_button;
		lss_song_list_mouse_init = false;
	}
	else
	{
		old_x = lss_song_list_mouse_x;
		lss_song_list_mouse_x = t3f_mouse_x;
		old_y = lss_song_list_mouse_y;
		lss_song_list_mouse_y = t3f_mouse_y;
		old_z = lss_song_list_mouse_z;
		lss_song_list_mouse_z = t3f_mouse_z;
		old_b = lss_song_list_mouse_button;
		lss_song_list_mouse_button = t3f_mouse_button[0];
	}

	if(old_z != lss_song_list_mouse_z)
	{
		diff = old_z - lss_song_list_mouse_z;
		lss_song_list_scroll_pos += diff * lss_song_list_space;
	}
	if(old_y != lss_song_list_mouse_y || old_z != lss_song_list_mouse_z)
	{
		app->selected_song = (lss_song_list_mouse_y - lss_song_list_space) / lss_song_list_space + lss_song_list_scroll_pos / lss_song_list_space;
		if(app->selected_song < 0)
		{
			app->selected_song = 0;
		}
		else if(app->selected_song >= app->song_list->visible_entries)
		{
			app->selected_song = app->song_list->visible_entries - 1;
		}
	}
	if(lss_song_list_mouse_button && old_b != lss_song_list_mouse_button)
	{
		lss_song_list_clicked = true;
	}
}

/* detect touch screen scroll */
static void lss_state_song_list_touch_scroll_logic(APP_INSTANCE * app)
{
	int i;

	lss_song_list_tapped = false;
	if(lss_song_list_touch_id < 0)
	{
		for(i = 1; i < T3F_MAX_TOUCHES; i++)
		{
			if(t3f_touch[i].active)
			{
				lss_song_list_touch_pos = t3f_touch[i].y;
				lss_song_list_touch_id = i;
				lss_song_list_scrolled = false;
				lss_song_list_touch_frames = 0;
				break;
			}
		}
	}

	if(lss_song_list_touch_id >= 0)
	{
		if(t3f_touch[lss_song_list_touch_id].active)
		{
			if(lss_song_list_scrolled || fabs(t3f_touch[lss_song_list_touch_id].y - lss_song_list_touch_pos) > 2.0)
			{
				lss_song_list_scroll_velocity = lss_song_list_touch_pos - t3f_touch[lss_song_list_touch_id].y;
				lss_song_list_touch_pos = t3f_touch[lss_song_list_touch_id].y;
				lss_song_list_scrolled = true;
			}
			lss_song_list_touch_frames++;
			if(!lss_song_list_scrolled && lss_song_list_touch_frames >= 6)
			{
				lss_song_list_selected = true;
			}
		}
		else
		{
			lss_song_list_touch_id = -1;
			if(!lss_song_list_scrolled)
			{
				lss_song_list_selected = true;
				lss_song_list_tapped = true;
				lss_song_list_tap_frames = 0;
			}
		}
	}
	lss_song_list_scroll_pos += lss_song_list_scroll_velocity;
	if(lss_song_list_touch_id < 0)
	{
		if(lss_song_list_scroll_velocity < 0.0)
		{
			lss_song_list_scroll_velocity += 0.25;
			if(lss_song_list_scroll_velocity > 0.0)
			{
				lss_song_list_scroll_velocity = 0.0;
			}
		}
		else if(lss_song_list_scroll_velocity > 0.0)
		{
			lss_song_list_scroll_velocity -= 0.25;
			if(lss_song_list_scroll_velocity < 0.0)
			{
				lss_song_list_scroll_velocity = 0.0;
			}
		}
	}

	/* detect which song was tapped */
	if(lss_song_list_selected)
	{
		app->selected_song = (int)(lss_song_list_scroll_pos + lss_song_list_touch_pos - lss_song_list_space) / lss_song_list_space;
		if(app->selected_song < 0)
		{
			app->selected_song = 0;
		}
		else if(app->selected_song >= app->song_list->visible_entries)
		{
			app->selected_song = app->song_list->visible_entries - 1;
		}
	}

	if(lss_song_list_tapped)
	{
		lss_song_list_tap_frames++;
	}
	else if(lss_song_list_tap_frames)
	{
		lss_song_list_tap_frames++;
	}
}

static void lss_state_song_list_center(APP_INSTANCE * app)
{
	lss_song_list_scroll_pos = (app->selected_song + 1 - lss_song_list_visible / 2) * lss_song_list_space;
}

static bool lss_song_select_entry_callback(void * data, int c)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	if(c == '\t')
	{
		lss_song_list_sort_type++;
		if(lss_song_list_sort_type > 2)
		{
			lss_song_list_sort_type = 0;
		}
	}
	if(lss_song_list_sort_type < 2)
	{
		lss_song_list_sort(app->song_list, lss_song_list_sort_type, lss_song_list_sort_filter);
	}
	else
	{
		lss_song_list_sort_collection(app->song_list, lss_song_list_collection);
	}
	app->selected_song = 0;
	lss_state_song_list_center(app);
	return true;
}

void lss_state_song_list_song_select_logic(APP_INSTANCE * app)
{
	int max;

	if(t3f_key_pressed())
	{
		if(!lss_song_list_sort_query)
		{
			lss_begin_text_entry(app, "", lss_song_list_sort_filter, 32, lss_song_select_entry_callback);
			lss_song_list_sort_query = true;
		}
	}
	lss_song_list_space = t3f_get_font_line_height(app->resources.font[lss_song_list_font]);
	lss_song_list_visible = (540 - lss_song_list_space * 2) / lss_song_list_space;
	lss_song_list_selected = false;
	lss_state_song_list_mouse_logic(app);
	lss_state_song_list_touch_scroll_logic(app);
	lss_read_controller(&app->controller[0]);
	if(t3f_key[ALLEGRO_KEY_ENTER] ||
	   app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed ||
	   lss_song_list_tap_frames > 15 || lss_song_list_clicked)
	{
		app->game.song = lss_load_song(app->song_list->entry[app->selected_song]->path);
		lss_enumerate_tracks(app->game.song);
		if(lss_create_track_list_menu(app))
		{
			app->game.player[0].selected_track = 0;
			app->state = LSS_STATE_SONG_SELECT_TRACK;
		}
		lss_song_list_tap_frames = 0;
		if(lss_song_list_sort_query)
		{
			lss_end_text_entry();
			lss_song_list_sort_query = false;
		}
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		if(lss_song_list_sort_query)
		{
			lss_end_text_entry();
			lss_song_list_sort_query = false;
		}
		lss_create_profiles_menu(app);
		app->state = LSS_STATE_TITLE;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_LEFT] && lss_song_list_sort_type == 2)
	{
		if(lss_song_list_sort_query)
		{
			lss_end_text_entry();
			lss_song_list_sort_query = false;
		}
		lss_song_list_collection--;
		if(lss_song_list_collection < 0)
		{
			lss_song_list_collection = app->song_list->collections - 1;
		}
		lss_song_list_sort_collection(app->song_list, lss_song_list_collection);
		app->selected_song = 0;
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_LEFT] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_RIGHT] && lss_song_list_sort_type == 2)
	{
		if(lss_song_list_sort_query)
		{
			lss_end_text_entry();
			lss_song_list_sort_query = false;
		}
		lss_song_list_collection++;
		if(lss_song_list_collection >= app->song_list->collections)
		{
			lss_song_list_collection = 0;
		}
		lss_song_list_sort_collection(app->song_list, lss_song_list_collection);
		app->selected_song = 0;
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_RIGHT] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		app->selected_song--;
		if(app->selected_song < 0)
		{
			app->selected_song = app->song_list->visible_entries - 1;
		}
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
	{
		app->selected_song++;
		if(app->selected_song >= app->song_list->visible_entries)
		{
			app->selected_song = 0;
		}
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		app->selected_song -= lss_song_list_visible;
		if(app->selected_song < 0)
		{
			app->selected_song = app->song_list->visible_entries - 1;
		}
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		app->selected_song += lss_song_list_visible;
		if(app->selected_song >= app->song_list->visible_entries)
		{
			app->selected_song = 0;
		}
		lss_state_song_list_center(app);
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
	else if(app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_ORANGE].pressed)
	{
		app->game.no_fail = !app->game.no_fail;
	}
	else if(app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_BLUE].pressed)
	{
		app->selected_song = t3f_rand(&app->rng_state) % app->song_list->visible_entries;
		lss_state_song_list_center(app);
	}
	if(lss_song_list_sort_query)
	{
		lss_process_text_entry();
	}

	max = app->song_list->visible_entries * lss_song_list_space - lss_song_list_visible * lss_song_list_space;
	if(app->song_list->visible_entries < lss_song_list_visible)
	{
		lss_song_list_scroll_pos = 0;
	}
	else if(lss_song_list_scroll_pos < 0)
	{
		lss_song_list_scroll_pos = 0;
	}
	else if(lss_song_list_scroll_pos > max)
	{
		lss_song_list_scroll_pos = max;
	}
}

void lss_state_song_list_song_select_render(APP_INSTANCE * app)
{
	ALLEGRO_COLOR color;
	int i, offset;
	int start_song = (int)lss_song_list_scroll_pos / lss_song_list_space;
	const char * val;
	int i_val;
	char buf[256];
	char * type[3] = {"Artist", "Title", "Collection"};

	if(app->game.no_fail)
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG_ALT);
	}
	else
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG);
	}
	al_draw_tinted_bitmap(app->title.logo_bitmap, al_map_rgba_f(0.0, 0.0, 0.0, 0.125), 480 - al_get_bitmap_width(app->title.logo_bitmap) / 2, 270 - al_get_bitmap_height(app->title.logo_bitmap) / 2, 0);

	/* render filter info */
	al_hold_bitmap_drawing(true);
	t3f_draw_textf(app->resources.font[lss_song_list_font], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 4, 0 + 4, 0, 0, "Sort By %s", type[lss_song_list_sort_type]);
	t3f_draw_textf(app->resources.font[lss_song_list_font], t3f_color_white, 0, 0, 0, 0, "Sort By %s", type[lss_song_list_sort_type]);
	if(lss_song_list_sort_type < 2)
	{
		t3f_draw_textf(app->resources.font[lss_song_list_font], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_default_view->right + 4, 0 + 4, 0, ALLEGRO_ALIGN_RIGHT, "%s", lss_song_list_sort_filter);
		t3f_draw_textf(app->resources.font[lss_song_list_font], t3f_color_white, t3f_default_view->right, 0, 0, ALLEGRO_ALIGN_RIGHT, "%s", lss_song_list_sort_filter);
	}
	else if(lss_song_list_collection >= 0)
	{
		t3f_draw_textf(app->resources.font[lss_song_list_font], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), t3f_default_view->right + 4, 0 + 4, 0, ALLEGRO_ALIGN_RIGHT, "%s", app->song_list->collection[lss_song_list_collection]->name);
		t3f_draw_textf(app->resources.font[lss_song_list_font], t3f_color_white, t3f_default_view->right, 0, 0, ALLEGRO_ALIGN_RIGHT, "%s", app->song_list->collection[lss_song_list_collection]->name);
	}
	al_hold_bitmap_drawing(false);

	/* render song list entries */
	al_hold_bitmap_drawing(true);
	t3f_set_clipping_rectangle(0, lss_song_list_space * 1, t3f_default_view->right, t3f_default_view->bottom - lss_song_list_space * 2);
	for(i = start_song; i < start_song + lss_song_list_visible && i < app->song_list->visible_entries; i++)
	{
		if(i == app->selected_song)
		{
			color = t3f_color_white;
			offset = 0;
		}
		else
		{
			color = al_map_rgba_f(0.75, 0.75, 0.75, 1.0);
			offset = 2;
		}
		t3f_draw_textf(app->resources.font[lss_song_list_font], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 4, lss_song_list_space * 1 + i * lss_song_list_space - lss_song_list_scroll_pos + 4, 0, 0, "%s - %s", app->song_list->entry[i]->artist, app->song_list->entry[i]->title);
		t3f_draw_textf(app->resources.font[lss_song_list_font], color, 0 + offset, lss_song_list_space * 1 + i * lss_song_list_space - lss_song_list_scroll_pos + offset, 0, 0, "%s - %s", app->song_list->entry[i]->artist, app->song_list->entry[i]->title);
	}
	al_hold_bitmap_drawing(false);
	t3f_set_clipping_rectangle(0, 0, 0, 0);

	/* render info for selected song */
	sprintf(buf, "high_score_%d_%d", app->selected_track, app->selected_difficulty);
	val = al_get_config_value(app->game.player[0].profile->config, app->song_list->entry[app->selected_song]->id, buf);
	if(val)
	{
		i_val = atoi(val);
		t3f_draw_textf(app->resources.font[lss_song_list_font], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 4, 540 - lss_song_list_space + 4, 0, 0, "High Score: %d", lss_unobfuscate_value(i_val));
		t3f_draw_textf(app->resources.font[lss_song_list_font], t3f_color_white, 0, 540 - lss_song_list_space, 0, 0, "High Score: %d", lss_unobfuscate_value(i_val));
	}
}

static void lss_song_list_process_menu(APP_INSTANCE * app, T3F_GUI * menu)
{
	t3f_process_gui(menu, app);
	if(app->controller[0].source != LSS_CONTROLLER_SOURCE_TOUCH)
	{
		lss_read_controller(&app->controller[0]);
	}
	if(t3f_key[ALLEGRO_KEY_ENTER] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed)
	{
		t3f_activate_selected_gui_element(menu, app);
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		t3f_select_previous_gui_element(menu);
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
	{
		t3f_select_next_gui_element(menu);
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
}

void lss_state_song_list_track_select_logic(APP_INSTANCE * app)
{
	lss_song_list_process_menu(app, lss_song_list_menu);
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		t3f_destroy_gui(lss_song_list_menu);
		lss_song_list_menu = NULL;
		lss_destroy_song(app->game.song);
		lss_song_list_tap_frames = 0;
		lss_state_song_list_initialize(app);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
	else if(app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_ORANGE].pressed)
	{
		app->game.no_fail = !app->game.no_fail;
	}
	if(lss_song_list_menu)
	{
		lss_update_gui_colors(lss_song_list_menu, LSS_TITLE_COLOR_HEADER, LSS_TITLE_COLOR_SELECTED, LSS_TITLE_COLOR_NORMAL);
	}
}

void lss_state_song_list_track_select_render(APP_INSTANCE * app)
{
	if(app->game.no_fail)
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG_ALT);
	}
	else
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG);
	}
	al_draw_tinted_bitmap(app->title.logo_bitmap, al_map_rgba_f(0.0, 0.0, 0.0, 0.125), 480 - al_get_bitmap_width(app->title.logo_bitmap) / 2, 270 - al_get_bitmap_height(app->title.logo_bitmap) / 2, 0);
	al_hold_bitmap_drawing(true);
	t3f_render_gui(lss_song_list_menu);
	al_hold_bitmap_drawing(false);
}

void lss_state_song_list_difficulty_select_logic(APP_INSTANCE * app)
{
	lss_song_list_process_menu(app, lss_song_list_menu);
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		t3f_destroy_gui(lss_song_list_menu);
		lss_song_list_menu = NULL;
		lss_create_track_list_menu(app);
		app->state = LSS_STATE_SONG_SELECT_TRACK;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
	else if(app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_ORANGE].pressed)
	{
		app->game.no_fail = !app->game.no_fail;
	}
	if(lss_song_list_menu)
	{
		lss_update_gui_colors(lss_song_list_menu, LSS_TITLE_COLOR_HEADER, LSS_TITLE_COLOR_SELECTED, LSS_TITLE_COLOR_NORMAL);
	}
}

void lss_state_song_list_difficulty_select_render(APP_INSTANCE * app)
{
	if(app->game.no_fail)
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG_ALT);
	}
	else
	{
		al_clear_to_color(LSS_TITLE_COLOR_BG);
	}
	al_draw_tinted_bitmap(app->title.logo_bitmap, al_map_rgba_f(0.0, 0.0, 0.0, 0.125), 480 - al_get_bitmap_width(app->title.logo_bitmap) / 2, 270 - al_get_bitmap_height(app->title.logo_bitmap) / 2, 0);
	al_hold_bitmap_drawing(true);
	t3f_render_gui(lss_song_list_menu);
	al_hold_bitmap_drawing(false);
}
