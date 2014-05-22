#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"

#include "game.h"

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path)
{
	gp->notes_texture = t3f_load_resource((void *)(&gp->notes_texture), T3F_RESOURCE_TYPE_BITMAP, "data/note.pcx", 0, 0, 0);
	if(!gp->notes_texture)
	{
		return false;
	}
	al_convert_mask_to_alpha(gp->notes_texture, al_map_rgba_f(1.0, 0.0, 1.0, 1.0));
	gp->current_tick = 0;
	gp->song_audio = lss_load_song_audio(song_path);
	if(!gp->song_audio)
	{
		return false;
	}
	lss_set_song_audio_playing(gp->song_audio, true);
	gp->done = false;
	return true;
}

void lss_game_exit(LSS_GAME * gp)
{
	t3f_destroy_resource(gp->notes_texture);
}

void lss_game_logic(LSS_GAME * gp)
{
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		lss_destroy_song_audio(gp->song_audio);
		lss_destroy_song(gp->song);
		gp->done = true;
		lss_game_exit(gp);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	gp->current_tick++;
}

void lss_game_render(LSS_GAME * gp)
{
	float rotate[5] = {-0.06, -0.03, 0.0, 0.03, 0.06};
	float oy[5] = {3.0, 1.0, 0.0, 1.0, 3.0};
	ALLEGRO_VERTEX v[5];
	double a, c, cy, z, end_z;
	int i;

	/* render note tails */
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay))) * 12.0;
			end_z = z + ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->length)) * 12.0;
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
				c = al_get_bitmap_width(gp->notes_texture) / 2;
				cy = c + c / 2;
				v[0].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
				v[0].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[0].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
				v[1].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, z);
				v[1].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[1].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
				v[2].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
				v[2].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[2].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
				v[3].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, end_z);
				v[3].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[3].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
				v[4].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
				v[4].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[4].color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
				al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
			}
		}
	}
	
	/* render notes */
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay))) * 12.0;
			a = 1.0;
			if(z > 2048.0)
			{
				a = 1.0 - (z - 2048.0) / 128.0;
				if(a < 0.0)
				{
					a = 0.0;
				}
			}
			c = al_get_bitmap_width(gp->notes_texture) / 2;
			cy = c + c / 2;
			t3f_draw_scaled_rotated_bitmap(gp->notes_texture, al_map_rgba_f(a, a, a, a), c, cy, 160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80, 340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z, rotate[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], 1.5, 1.5, 0);
		}
	}
	for(i = 0; i < 5; i++)
	{
		c = al_get_bitmap_width(gp->notes_texture) / 2;
		cy = c + c / 2;
		t3f_draw_scaled_rotated_bitmap(gp->notes_texture, al_map_rgba_f(0.5, 0.5, 0.5, 0.5), c, cy, 160 + i * 80, 340 + cy + oy[i], 0, rotate[i], 1.5, 1.5, 0);
	}
}
