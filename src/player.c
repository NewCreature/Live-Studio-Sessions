#include "t3f/t3f.h"
#include "t3f/view.h"
#include "t3f/draw.h"

#include "game.h"
#include "player.h"

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

void lss_initialize_player(LSS_GAME * gp, int player)
{
	gp->player[0].playing_notes = 0;
	gp->player[0].next_note[0] = -1;
	gp->player[0].next_notes = 1;
	gp->player[0].hit_notes = 0;
	gp->player[0].streak = 0;
	gp->player[0].life = 100;
	gp->player[0].miss_streak = 0;
	gp->player[0].score = 0;
	lss_player_get_next_notes(gp->song, &gp->player[0]);
}

void lss_player_logic(LSS_GAME * gp, int player)
{
	int i, d, m, t;
	int lane[8] = {0};
	int tap_lane[8] = {0};
	bool missed = false;
	int points = 0;
	
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
		/* if note is within hit window */
		d = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[0]]->tick - (gp->current_tick - gp->av_delay)));
		if(d >= -8.0 && d <= 8.0)
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
					gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->visible = false;
					gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->play_tick = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->tick;
				}
				gp->player[0].playing_notes = gp->player[0].next_notes;
				gp->player[0].hit_notes += gp->player[0].playing_notes;
				m = gp->player[0].streak / 8 + 1;
				if(m > 4)
				{
					m = 4;
				}
				points = gp->player[0].playing_notes * LSS_GAME_NOTE_BASE_POINTS * m;
				gp->player[0].score += points;
				gp->player[0].streak++;
				gp->player[0].miss_streak = 0;
				gp->player[0].life += gp->player[0].streak;
				if(gp->player[0].life > 100)
				{
					gp->player[0].life = 100;
				}
				lss_player_get_next_notes(gp->song, &gp->player[0]);
			}
			else
			{
				missed = true;
			}
		}
		else
		{
			missed = true;
		}
	}
	else
	{
		/* see if we are holding a sustain */
		if(gp->player[0].playing_notes)
		{
			/* see if we have reached the end of the note */
			t = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].playing_note[0]]->tick + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].playing_note[0]]->length;
			if(gp->current_tick - gp->av_delay > t)
			{
				gp->player[0].playing_notes = 0;
			}

			/* continue sustain */
			else
			{
				memset(lane, 0, sizeof(int) * 8);
				memset(tap_lane, 0, sizeof(int) * 8);
				for(i = 0; i < gp->player[0].playing_notes; i++)
				{
					lane[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].playing_note[i]]->val] = 1;
				}
				for(i = 0; i < 5; i++)
				{
					if(gp->player[0].controller->controller->state[i].held)
					{
						tap_lane[i] = 1;
					}
				}
				if(memcmp(lane, tap_lane, sizeof(int) * 8))
				{
					gp->player[0].playing_notes = 0;
				}
				else
				{
					for(i = 0; i < gp->player[0].playing_notes; i++)
					{
						gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].playing_note[i]]->play_tick = gp->current_tick;
					}
					m = gp->player[0].streak / 8 + 1;
					if(m > 4)
					{
						m = 4;
					}
					points = gp->player[0].playing_notes * LSS_GAME_NOTE_SUSTAIN_BASE_POINTS * m;
					gp->player[0].score += points;
				}
			}
		}
		
		/* see if we are hitting a HOPO note */
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[0]]->hopo && gp->player[0].streak > 0)
		{
			memset(lane, 0, sizeof(int) * 8);
			memset(tap_lane, 0, sizeof(int) * 8);
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
					gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->visible = false;
					gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->play_tick = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[i]]->tick;
				}
				gp->player[0].playing_notes = gp->player[0].next_notes;
				gp->player[0].hit_notes++;
				m = gp->player[0].streak / 8 + 1;
				if(m > 4)
				{
					m = 4;
				}
				points = gp->player[0].playing_notes * LSS_GAME_NOTE_BASE_POINTS * m;
				gp->player[0].score += points;
				gp->player[0].streak++;
				gp->player[0].miss_streak = 0;
				gp->player[0].life += gp->player[0].streak;
				if(gp->player[0].life > 100)
				{
					gp->player[0].life = 100;
				}
				lss_player_get_next_notes(gp->song, &gp->player[0]);
			}
		}
		
		/* move on to next note if we missed this one */
		d = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[0]]->tick - (gp->current_tick - gp->av_delay)));
		if(d < -8)
		{
			missed = true;
			lss_player_get_next_notes(gp->song, &gp->player[0]);
		}
	}
	if(missed)
	{
		gp->player[0].streak = 0;
		gp->player[0].miss_streak++;
		gp->player[0].life -= gp->player[0].miss_streak;
	}
}

void lss_player_render_board(LSS_GAME * gp, int player)
{
	float rotate[5] = {-0.06, -0.03, 0.0, 0.03, 0.06};
	float oy[5] = {3.0, 1.0, 0.0, 1.0, 3.0};
	ALLEGRO_VERTEX v[5];
	ALLEGRO_COLOR color;
	double a, c, cy, z, end_z;
	int i, j;
	bool playing;
	int note_type;

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c;

	al_hold_bitmap_drawing(true);
	al_draw_bitmap(gp->studio_image, 0, 0, 0);
	al_hold_bitmap_drawing(false);
	z = -480;
	end_z = 2048;
	v[0].x = t3f_project_x(320 - 32, z);
	v[0].y = t3f_project_y(420 + cy, z);
	v[0].color = t3f_color_black;
	v[1].x = t3f_project_x(320 + 4 * 80 + 32, z);
	v[1].y = t3f_project_y(420 + cy, z);
	v[1].color = t3f_color_black;
	v[2].x = t3f_project_x(320 + 4 * 80 + 32, end_z);
	v[2].y = t3f_project_y(420 + cy, end_z);
	v[2].color = t3f_color_black;
	v[3].x = t3f_project_x(320 - 32, end_z);
	v[3].y = t3f_project_y(420 + cy, end_z);
	v[3].color = t3f_color_black;
	v[4].x = t3f_project_x(320 - 32, z);
	v[4].y = t3f_project_y(420 + cy, z);
	v[4].color = t3f_color_black;
	al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	
	/* render note tails */
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		playing = false;
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			color = al_map_rgba_f(0.5, 0.5, 1.0, 1.0);
			for(j = 0; j < gp->player[0].playing_notes; j++)
			{
				if(gp->player[0].playing_note[j] == i)
				{
					color = t3f_color_white;
					playing = true;
				}
			}
			if(playing)
			{
				z = 0.0;
			}
			else
			{
				z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->play_tick - (gp->current_tick - gp->av_delay))) * 12.0;
			}
			end_z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->length - (gp->current_tick - gp->av_delay))) * 12.0;
			a = 1.0;
			if(z < 2048.0 + 128.0 && end_z > -640.0 && end_z > z)
			{
				if(z < -639.0)
				{
					z = -639.0;
				}
				if(end_z < -639.0)
				{
					end_z = -639.0;
				}
				else if(end_z > 2048.0)
				{
					end_z = 2048.0;
				}
				v[0].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
				v[0].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[0].color = color;
				v[1].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, z);
				v[1].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[1].color = color;
				v[2].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
				v[2].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[2].color = color;
				v[3].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, end_z);
				v[3].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[3].color = color;
				v[4].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, z);
				v[4].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[4].color = color;
				al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
			}
		}
	}
	
	/* render notes */
	al_hold_bitmap_drawing(true);
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		playing = false;
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay))) * 12.0;
			a = 1.0;
			note_type = 0;
			if(z > 2048.0)
			{
				a = 1.0 - (z - 2048.0) / 128.0;
				if(a < 0.0)
				{
					a = 0.0;
				}
			}
			if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->hopo)
			{
//				a *= 0.5;
				note_type = 1;
			}
			if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->visible)
			{
				t3f_draw_rotated_bitmap(gp->note_texture[note_type], al_map_rgba_f(a, a, a, a), c, cy, 320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80, 420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z, rotate[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], 0);
			}
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
		t3f_draw_rotated_bitmap(gp->note_texture[0], al_map_rgba_f(a, a, a, a), c, cy, 320 + i * 80, 420 + cy + oy[i], 0, rotate[i], 0);
	}
}
