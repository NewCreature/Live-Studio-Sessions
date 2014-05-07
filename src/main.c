#include "t3f/t3f.h"
#include "t3f/draw.h"
#include "t3f/resource.h"
#include "t3f/view.h"

#include "rtk/midi.h"

#include "song_list.h"
#include "song_audio.h"
#include "song.h"
#include "player.h"

/* structure to hold all of our app-specific data */
typedef struct
{
	
	/* global data */
	ALLEGRO_FONT * font;
	LSS_SONG_LIST * song_list;
	
	/* menu data */
	int selected_song;
	
	LSS_SONG * song;
	LSS_SONG_AUDIO * song_audio;

	LSS_PLAYER player[LSS_MAX_PLAYERS];
	
	int state;
	int av_delay;
	double offset;
	int current_tick;
	ALLEGRO_BITMAP * notes_texture;
	
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
				app->song = lss_load_song(app->song_list->entry[app->selected_song]->path);
				app->state = 1;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case 1:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				lss_destroy_song(app->song);
				app->state = 0;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_UP])
			{
				app->player[0].selected_track--;
				if(app->player[0].selected_track < 0)
				{
					app->player[0].selected_track = app->song->source_midi->tracks - 1;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->player[0].selected_track++;
				if(app->player[0].selected_track >= app->song->source_midi->tracks)
				{
					app->player[0].selected_track = 0;
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
				app->player[0].selected_difficulty--;
				if(app->player[0].selected_difficulty < 0)
				{
					app->player[0].selected_difficulty = 3;
				}
				t3f_key[ALLEGRO_KEY_UP] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_DOWN])
			{
				app->player[0].selected_difficulty++;
				if(app->player[0].selected_difficulty > 3)
				{
					app->player[0].selected_difficulty = 0;
				}
				t3f_key[ALLEGRO_KEY_DOWN] = 0;
			}
			if(t3f_key[ALLEGRO_KEY_ENTER])
			{
				app->song_audio = lss_load_song_audio(app->song_list->entry[app->selected_song]->path);
				if(app->song_audio)
				{
					lss_set_song_audio_playing(app->song_audio, true);
				}
				app->current_tick = 0;
				app->state = 3;
				t3f_key[ALLEGRO_KEY_ENTER] = 0;
			}
			break;
		}
		case 3:
		{
			if(t3f_key[ALLEGRO_KEY_ESCAPE])
			{
				lss_destroy_song_audio(app->song_audio);
//				t3f_stop_music();
				lss_destroy_song(app->song);
				app->state = 0;
				t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
			}
			app->current_tick++;
			break;
		}
	}
}

/* main rendering routine */
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int i;
	float a, z, end_z, c, cy;
	float rotate[5] = {-0.06, -0.03, 0.0, 0.03, 0.06};
	float oy[5] = {3.0, 1.0, 0.0, 1.0, 3.0};
	ALLEGRO_COLOR color;
	char * difficulty_text[4] = {"Easy", "Medium", "Hard", "Expert"};
	ALLEGRO_VERTEX v[5];

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
				al_draw_textf(app->font, color, 0, i * 20, 0, "%s", al_path_cstr(app->song_list->entry[app->selected_song + i]->path, '/'));
			}
			break;
		}
		case 1:
		{
			for(i = 0; i < app->song->source_midi->tracks; i++)
			{
				if(i == app->player[0].selected_track)
				{
					color = t3f_color_white;
				}
				else
				{
					color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
				}
				al_draw_textf(app->font, color, 0, i * 20, 0, "%s", app->song->source_midi->track[i]->name);
			}
			break;
		}
		case 2:
		{
			for(i = 0; i < 4; i++)
			{
				if(i == app->player[0].selected_difficulty)
				{
					color = t3f_color_white;
				}
				else
				{
					color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);
				}
				al_draw_textf(app->font, color, 0, i * 20, 0, "%s (%d)", difficulty_text[i], app->song->track[app->player[0].selected_track][i].note_count);
			}
			break;
		}
		case 3:
		{
			
			/* render note tails */
			for(i = app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].notes - 1; i >= 0; i--)
			{
				if(app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->active)
				{
					z = ((app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->tick - (app->current_tick - app->av_delay))) * 12.0;
					end_z = z + ((app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->length)) * 12.0;
					a = 1.0;
					if(z < 2048.0 + 128.0 && end_z > -640.0)
					{
						if(z < -639.0)
						{
							z = -639.0;
						}
						if(end_z < -639.0)
						{
							end_z = -639.0;
						}
						c = al_get_bitmap_width(app->notes_texture) / 2;
						cy = c + c / 2;
						v[0].x = t3f_project_x(160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
						v[0].y = t3f_project_y(340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], z);
						v[0].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
						v[1].x = t3f_project_x(160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80 + 8, z);
						v[1].y = t3f_project_y(340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], z);
						v[1].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
						v[2].x = t3f_project_x(160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
						v[2].y = t3f_project_y(340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], end_z);
						v[2].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
						v[3].x = t3f_project_x(160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80 - 8, end_z);
						v[3].y = t3f_project_y(340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], end_z);
						v[3].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
						v[4].x = t3f_project_x(160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
						v[4].y = t3f_project_y(340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], z);
						v[4].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
						al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
					}
				}
			}
			
			/* render notes */
			for(i = app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].notes - 1; i >= 0; i--)
			{
				if(app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->active)
				{
					z = ((app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->tick - (app->current_tick - app->av_delay))) * 12.0;
					a = 1.0;
					if(z > 2048.0)
					{
						a = 1.0 - (z - 2048.0) / 128.0;
						if(a < 0.0)
						{
							a = 0.0;
						}
					}
					c = al_get_bitmap_width(app->notes_texture) / 2;
					cy = c + c / 2;
					t3f_draw_scaled_rotated_bitmap(app->notes_texture, al_map_rgba_f(a, a, a, a), c, cy, 160 + app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val * 80, 340 + cy + oy[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], z, rotate[app->song->track[app->player[0].selected_track][app->player[0].selected_difficulty].note[i]->val], 1.5, 1.5, 0);
				}
			}
			for(i = 0; i < 5; i++)
			{
				c = al_get_bitmap_width(app->notes_texture) / 2;
				cy = c + c / 2;
				t3f_draw_scaled_rotated_bitmap(app->notes_texture, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), c, cy, 160 + i * 80, 340 + cy + oy[i], 0, rotate[i], 1.5, 1.5, 0);
//				t3f_draw_scaled_bitmap(app->notes_texture, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), 64 + i * 64 - 48, 320, 0, 96, 96, 0);
			}
			break;
		}
	}
	al_hold_bitmap_drawing(false);
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int f;
	ALLEGRO_PATH * songs_path;
	
	/* initialize T3F */
	if(!t3f_initialize("Live Studio Sessions", 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	app->font = t3f_load_resource((void *)(&app->font), T3F_RESOURCE_TYPE_FONT, "data/teen.ttf", 20, 0, 0);
	if(!app->font)
	{
		return false;
	}
	app->notes_texture = t3f_load_resource((void *)(&app->notes_texture), T3F_RESOURCE_TYPE_BITMAP, "data/note.pcx", 0, 0, 0);
	if(!app->notes_texture)
	{
		return false;
	}
	al_convert_mask_to_alpha(app->notes_texture, al_map_rgba_f(1.0, 0.0, 1.0, 1.0));
	
	/* create song database */
	songs_path = al_create_path("/Volumes/My Passport/Archive/Games/FOF/songs");
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
	app->av_delay = 15;
	app->selected_song = 0;
	app->player[0].selected_track = 0;
	app->player[0].selected_difficulty = 0;
	return true;
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
	return 0;
}
