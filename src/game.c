#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"

#include "resources.h"
#include "game.h"

static bool lss_player_get_next_notes(LSS_SONG * sp, LSS_PLAYER * pp)
{
	int cur_note = pp->next_note[pp->next_notes - 1];
	int cur_tick;
	
	if(cur_note < 0)
	{
		cur_note = 0;
		cur_tick = -1;
	}
	else
	{
		cur_tick = sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick;
	}
	
	while(cur_note < sp->track[pp->selected_track][pp->selected_difficulty].notes)
	{
		/* found next note */
		if(sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick != cur_tick)
		{
			pp->next_notes = 0;
			cur_tick = sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick;
			while(cur_note < sp->track[pp->selected_track][pp->selected_difficulty].notes && sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick == cur_tick)
			{
				printf("next_note = %d\n", cur_note);
				pp->next_note[pp->next_notes] = cur_note;
				pp->next_notes++;
				cur_note++;
			}
			return true;
		}
		cur_note++;
	}
	return false;
}

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
	gp->player[0].playing_notes = 0;
	gp->player[0].next_note[0] = -1;
	gp->player[0].next_notes = 1;
	gp->player[0].hit_notes = 0;
	lss_player_get_next_notes(gp->song, &gp->player[0]);
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
	int i, d;
	int lane[8] = {0};
	int tap_lane[8] = {0};
	
	/* handle player logic */
	lss_read_controller(gp->player[0].controller);
	gp->player[0].hittable_notes = 0;
	for(i = 0; i < gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes; i++)
	{
		d = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay)));
		if(d >= -8 && d <= 8)
		{
			gp->player[0].hittable_note[gp->player[0].hittable_notes] = i;
			gp->player[0].hittable_notes++;
		}
	}
	
	/* check for note hits */
	if(gp->player[0].controller->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pressed || gp->player[0].controller->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP].pressed)
	{
		for(i = 0; i < gp->player[0].next_notes; i++)
		{
			lane[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->val] = 1;
		}
		for(i = 0; i < 5; i++)
		{
			if(gp->player[0].controller->controller->state[i].held)
			{
				tap_lane[i] = 1;
			}
		}
		if(!memcmp(lane, tap_lane, sizeof(int) * 8))
		{
			for(i = 0; i < gp->player[0].next_notes; i++)
			{
				gp->player[0].playing_note[i] = gp->player[0].next_note[i];
			}
			gp->player[0].playing_notes = gp->player[0].next_notes;
			gp->player[0].hit_notes++;
			lss_player_get_next_notes(gp->song, &gp->player[0]);
		}
	}
	else
	{
		/* move on to next note if we missed this one */
		d = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[0]]->tick - (gp->current_tick - gp->av_delay)));
		if(d < -8)
		{
			lss_player_get_next_notes(gp->song, &gp->player[0]);
		}
	}
	
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

void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp)
{
	float rotate[5] = {-0.06, -0.03, 0.0, 0.03, 0.06};
	float oy[5] = {3.0, 1.0, 0.0, 1.0, 3.0};
	ALLEGRO_VERTEX v[5];
	ALLEGRO_COLOR color;
	double a, c, cy, z, end_z;
	int i, j;

	/* render note tails */
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
			for(j = 0; j < gp->player[0].playing_notes; j++)
			{
				if(gp->player[0].playing_note[j] == i)
				{
					color = t3f_color_white;
				}
			}
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
				v[0].color = color;
				v[1].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, z);
				v[1].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[1].color = color;
				v[2].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
				v[2].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[2].color = color;
				v[3].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, end_z);
				v[3].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[3].color = color;
				v[4].x = t3f_project_x(160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
				v[4].y = t3f_project_y(340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[4].color = color;
				al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
			}
		}
	}
	
	/* render notes */
	al_hold_bitmap_drawing(true);
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
			for(j = 0; j < gp->player[0].playing_notes; j++)
			{
				if(gp->player[0].playing_note[j] == i)
				{
					a *= 0.5;
				}
			}
			c = al_get_bitmap_width(gp->notes_texture) / 2;
			cy = c + c / 2;
			t3f_draw_scaled_rotated_bitmap(gp->notes_texture, al_map_rgba_f(a, a, a, a), c, cy, 160 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80, 340 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z, rotate[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], 1.5, 1.5, 0);
		}
	}
	for(i = 0; i < 5; i++)
	{
		if(gp->player[0].controller->controller->state[i].down)
		{
			a = 1.0;
		}
		else
		{
			a = 0.5;
		}
		c = al_get_bitmap_width(gp->notes_texture) / 2;
		cy = c + c / 2;
		t3f_draw_scaled_rotated_bitmap(gp->notes_texture, al_map_rgba_f(a, a, a, a), c, cy, 160 + i * 80, 340 + cy + oy[i], 0, rotate[i], 1.5, 1.5, 0);
	}
	al_draw_textf(rp->font, t3f_color_white, 0, 0, 0, "%d/%d", gp->player[0].hit_notes, gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes);
	al_hold_bitmap_drawing(false);
}
