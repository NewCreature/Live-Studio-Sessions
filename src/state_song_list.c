#include "t3f/t3f.h"

#include "instance.h"
#include "state.h"

static int lss_track[16];
static int lss_tracks = 0;
static int lss_diff[4];
static int lss_diffs = 0;

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

void lss_state_song_list_song_select_logic(APP_INSTANCE * app)
{
	lss_read_controller(&app->controller[0]);
	if(t3f_key[ALLEGRO_KEY_ENTER] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed)
	{
		app->game.song = lss_load_song(app->song_list->entry[app->selected_song]->path);
		lss_enumerate_tracks(app->game.song);
		app->game.player[0].selected_track = 0;
		app->state = LSS_STATE_SONG_SELECT_TRACK;
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_ESCAPE] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		app->state = LSS_STATE_TITLE;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		app->selected_song--;
		if(app->selected_song < 0)
		{
			app->selected_song = app->song_list->entries - 1;
		}
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
	{
		app->selected_song++;
		if(app->selected_song >= app->song_list->entries)
		{
			app->selected_song = 0;
		}
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGUP])
	{
		app->selected_song -= 20;
		if(app->selected_song < 0)
		{
			app->selected_song = app->song_list->entries - 1;
		}
		t3f_key[ALLEGRO_KEY_PGUP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_PGDN])
	{
		app->selected_song += 20;
		if(app->selected_song >= app->song_list->entries)
		{
			app->selected_song = 0;
		}
		t3f_key[ALLEGRO_KEY_PGDN] = 0;
	}
}

void lss_state_song_list_song_select_render(APP_INSTANCE * app)
{
	ALLEGRO_COLOR color;
	int i;

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
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], color, 0, i * 20, 0, "%s - %s", app->song_list->entry[app->selected_song + i]->artist, app->song_list->entry[app->selected_song + i]->title);
	}
}

void lss_state_song_list_track_select_logic(APP_INSTANCE * app)
{
	lss_read_controller(&app->controller[0]);
	if(t3f_key[ALLEGRO_KEY_ENTER] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed)
	{
		lss_enumerate_difficulties(app->game.song, lss_track[app->game.player[0].selected_track]);
		app->game.player[0].selected_difficulty = 0;
		app->state = LSS_STATE_SONG_SELECT_DIFFICULTY;
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_ESCAPE] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		lss_destroy_song(app->game.song);
		app->state = LSS_STATE_SONG_SELECT;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		app->game.player[0].selected_track--;
		if(app->game.player[0].selected_track < 0)
		{
			app->game.player[0].selected_track = lss_tracks - 1;
		}
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
	{
		app->game.player[0].selected_track++;
		if(app->game.player[0].selected_track >= lss_tracks)
		{
			app->game.player[0].selected_track = 0;
		}
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
}

void lss_state_song_list_track_select_render(APP_INSTANCE * app)
{
	ALLEGRO_COLOR color;
	int i;

	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "%s - %s", app->song_list->entry[app->selected_song]->artist, app->song_list->entry[app->selected_song]->title);
	for(i = 0; i < lss_tracks; i++)
	{
		if(i == app->game.player[0].selected_track)
		{
			color = t3f_color_white;
		}
		else
		{
			color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
		}
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], color, 0, (i + 2) * 20, 0, "%s", app->game.song->source_midi->track[lss_track[i]]->name);
	}
}

void lss_state_song_list_difficulty_select_logic(APP_INSTANCE * app)
{
	lss_read_controller(&app->controller[0]);
	if(t3f_key[ALLEGRO_KEY_ENTER] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_GREEN].pressed)
	{
		app->game.song_id = app->song_list->entry[app->selected_song]->id;
		app->game.player[0].selected_track = lss_track[app->game.player[0].selected_track];
		app->game.player[0].selected_difficulty = lss_diff[app->game.player[0].selected_difficulty];
		app->game.player[0].controller = &app->controller[0];
		if(lss_game_initialize(&app->game, app->song_list->entry[app->selected_song]->path))
		{
			app->state = LSS_STATE_GAME;
		}
		else
		{
			printf("failed to initialize game\n");
		}
		t3f_key[ALLEGRO_KEY_ENTER] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_ESCAPE] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_RED].pressed)
	{
		app->state = LSS_STATE_SONG_SELECT_TRACK;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_UP] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		app->game.player[0].selected_difficulty--;
		if(app->game.player[0].selected_difficulty < 0)
		{
			app->game.player[0].selected_difficulty = lss_diffs - 1;
		}
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	else if(t3f_key[ALLEGRO_KEY_DOWN] || app->controller[0].controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed)
	{
		app->game.player[0].selected_difficulty++;
		if(app->game.player[0].selected_difficulty > lss_diffs - 1)
		{
			app->game.player[0].selected_difficulty = 0;
		}
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
}

void lss_state_song_list_difficulty_select_render(APP_INSTANCE * app)
{
	char * difficulty_text[4] = {"Easy", "Medium", "Hard", "Expert"};
	ALLEGRO_COLOR color;
	int i;

	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "%s - %s", app->song_list->entry[app->selected_song]->artist, app->song_list->entry[app->selected_song]->title);
	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 20, 0, "%s", app->game.song->source_midi->track[lss_track[app->game.player[0].selected_track]]->name);
	for(i = 0; i < lss_diffs; i++)
	{
		if(i == app->game.player[0].selected_difficulty)
		{
			color = t3f_color_white;
		}
		else
		{
			color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
		}
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], color, 0, (i + 3) * 20, 0, "%s (%d)", difficulty_text[lss_diff[i]], app->game.song->track[lss_track[app->game.player[0].selected_track]][lss_diff[i]].note_count);
	}
}
