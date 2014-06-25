#include "instance.h"
#include "state.h"
#include "game_results.h"
#include "title.h"

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

void lss_state_logic(APP_INSTANCE * app)
{
	switch(app->state)
	{
		case LSS_STATE_TITLE:
		{
			lss_title_logic(&app->title, app);
			break;
		}
		case LSS_STATE_SONG_SELECT:
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
			if(t3f_key[ALLEGRO_KEY_PGUP])
			{
				app->selected_song -= 20;
				if(app->selected_song < 0)
				{
					app->selected_song = app->song_list->entries - 1;
				}
				t3f_key[ALLEGRO_KEY_PGUP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_PGDN])
			{
				app->selected_song += 20;
				if(app->selected_song >= app->song_list->entries)
				{
					app->selected_song = 0;
				}
				t3f_key[ALLEGRO_KEY_PGDN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->game.song = lss_load_song(app->song_list->entry[app->selected_song]->path);
				lss_enumerate_tracks(app->game.song);
				app->game.player[0].selected_track = 0;
				app->state = LSS_STATE_SONG_SELECT_TRACK;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			else if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				app->state = LSS_STATE_TITLE;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			break;
		}
		case LSS_STATE_SONG_SELECT_TRACK:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				lss_destroy_song(app->game.song);
				app->state = LSS_STATE_SONG_SELECT;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->game.player[0].selected_track--;
				if(app->game.player[0].selected_track < 0)
				{
					app->game.player[0].selected_track = lss_tracks - 1;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->game.player[0].selected_track++;
				if(app->game.player[0].selected_track >= lss_tracks)
				{
					app->game.player[0].selected_track = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				lss_enumerate_difficulties(app->game.song, lss_track[app->game.player[0].selected_track]);
				app->game.player[0].selected_difficulty = 0;
				app->state = LSS_STATE_SONG_SELECT_DIFFICULTY;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case LSS_STATE_SONG_SELECT_DIFFICULTY:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				app->state = LSS_STATE_SONG_SELECT_TRACK;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->game.player[0].selected_difficulty--;
				if(app->game.player[0].selected_difficulty < 0)
				{
					app->game.player[0].selected_difficulty = lss_diffs - 1;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->game.player[0].selected_difficulty++;
				if(app->game.player[0].selected_difficulty > lss_diffs - 1)
				{
					app->game.player[0].selected_difficulty = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->game.player[0].selected_track = lss_track[app->game.player[0].selected_track];
				app->game.player[0].selected_difficulty = lss_diff[app->game.player[0].selected_difficulty];
				app->game.player[0].profile = &app->profiles->entry[0];
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
			break;
		}
		case LSS_STATE_GAME:
		{
			lss_game_logic(&app->game);
			if(app->game.done)
			{
				app->state = LSS_STATE_GAME_RESULTS;
			}
			break;
		}
		case LSS_STATE_GAME_RESULTS:
		{
			lss_game_results_logic(&app->game);
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				app->state = LSS_STATE_SONG_SELECT;
			}
			break;
		}
	}
}

void lss_state_render(APP_INSTANCE * app)
{
	int i;
	ALLEGRO_COLOR color;
	char * difficulty_text[4] = {"Easy", "Medium", "Hard", "Expert"};

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
			break;
		}
		case LSS_STATE_SONG_SELECT_TRACK:
		{
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
			break;
		}
		case LSS_STATE_SONG_SELECT_DIFFICULTY:
		{
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
			break;
		}
		case LSS_STATE_GAME:
		{
			lss_game_render(&app->game, &app->resources);
			break;
		}
		case LSS_STATE_GAME_RESULTS:
		{
			lss_game_results_render(&app->game, &app->resources);
			break;
		}
	}
}
