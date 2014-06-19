#include "instance.h"
#include "state.h"

void lss_state_logic(APP_INSTANCE * app)
{
	switch(app->state)
	{
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
				app->state = LSS_STATE_SONG_SELECT_TRACK;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
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
				al_draw_textf(app->resources.font[LSS_FONT_SMALL], color, 0, (i + 2) * 20, 0, "%s", app->game.song->source_midi->track[i]->name);
			}
			break;
		}
		case LSS_STATE_SONG_SELECT_DIFFICULTY:
		{
			al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "%s - %s", app->song_list->entry[app->selected_song]->artist, app->song_list->entry[app->selected_song]->title);
			al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 20, 0, "%s", app->game.song->source_midi->track[app->game.player[0].selected_track]->name);
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
				al_draw_textf(app->resources.font[LSS_FONT_SMALL], color, 0, (i + 3) * 20, 0, "%s (%d)", difficulty_text[i], app->game.song->track[app->game.player[0].selected_track][i].note_count);
			}
			break;
		}
		case LSS_STATE_GAME:
		{
			lss_game_render(&app->game, &app->resources);
			break;
		}
	}
}
