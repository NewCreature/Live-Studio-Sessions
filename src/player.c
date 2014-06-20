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
	
	pp->next_notes = 0;
	while(cur_note < sp->track[pp->selected_track][pp->selected_difficulty].notes)
	{
		/* found next note */
		if(sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick != cur_tick)
		{
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

static bool lss_player_check_notes(LSS_SONG * sp, LSS_PLAYER * pp, int note[], int notes)
{
	int lane[8] = {0};
	int tap_lane[8] = {0};
	int i;

	for(i = 0; i < notes; i++)
	{
		lane[sp->track[pp->selected_track][pp->selected_difficulty].note[note[i]]->val] = 1;
	}
	for(i = 0; i < 5; i++)
	{
		if(pp->controller->controller->state[i].held)
		{
			tap_lane[i] = 1;
		}
	}
	if(!memcmp(lane, tap_lane, sizeof(int) * 8))
	{
		return true;
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
	bool missed = false;
	int points = 0;
	int stream;
	
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
			if(lss_player_check_notes(gp->song, &gp->player[0], gp->player[0].next_note, gp->player[0].next_notes))
			{
				if(gp->song_audio->streams > 1)
				{
					stream = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].stream;
					if(stream >= 0 && gp->song_audio->stream[stream])
					{
						al_set_audio_stream_gain(gp->song_audio->stream[stream], 1.0);
					}
				}
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
				if(!lss_player_check_notes(gp->song, &gp->player[0], gp->player[0].playing_note, gp->player[0].playing_notes))
				{
					if(gp->song_audio->streams > 1)
					{
						stream = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].stream;
						if(stream >= 0 && gp->song_audio->stream[stream])
						{
//							al_set_audio_stream_gain(gp->song_audio->stream[stream], 0.0);
						}
					}
					gp->player[0].playing_notes = 0;
				}
				else
				{
					for(i = 0; i < gp->player[0].playing_notes; i++)
					{
						gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].playing_note[i]]->play_tick = gp->current_tick - gp->av_delay;
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
			if(lss_player_check_notes(gp->song, &gp->player[0], gp->player[0].next_note, gp->player[0].next_notes))
			{
				if(gp->song_audio->streams > 1)
				{
					stream = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].stream;
					if(stream >= 0 && gp->song_audio->stream[stream])
					{
						al_set_audio_stream_gain(gp->song_audio->stream[stream], 1.0);
					}
				}
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
		if(gp->player[0].next_notes)
		{
			d = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[gp->player[0].next_note[0]]->tick - (gp->current_tick - gp->av_delay)));
			if(d < -8)
			{
				if(gp->song_audio->streams > 1)
				{
					stream = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].stream;
					if(stream >= 0 && gp->song_audio->stream[stream])
					{
						al_set_audio_stream_gain(gp->song_audio->stream[stream], 0.0);
					}
				}
				missed = true;
				lss_player_get_next_notes(gp->song, &gp->player[0]);
			}
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
	ALLEGRO_VERTEX v[32];
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR color_chart[5];
	double a, c, cy, z, end_z;
	int i, j;
	bool playing;
	int note_type;

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c;
	color_chart[0] = al_map_rgb(0, 150, 0);
	color_chart[1] = al_map_rgb(139, 0, 0);
	color_chart[2] = al_map_rgb(192, 192, 0);
	color_chart[3] = al_map_rgb(0, 0, 204);
	color_chart[4] = al_map_rgb(215, 78, 0);

	al_hold_bitmap_drawing(true);
	al_draw_bitmap(gp->studio_image, 0, 0, 0);
	al_hold_bitmap_drawing(false);
/*	z = -480;
	end_z = 2048;
	v[0].x = t3f_project_x(320 - 32, z);
	v[0].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[0].z = 0;
	v[0].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[1].x = t3f_project_x(320 + 4 * 80 + 32, z);
	v[1].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[1].z = 0;
	v[1].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[2].x = t3f_project_x(320 + 4 * 80 + 32, end_z);
	v[2].y = t3f_project_y(420 + cy + oy[0] + 2.0, end_z);
	v[2].z = 0;
	v[2].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	memcpy(&v[3], &v[2], sizeof(ALLEGRO_VERTEX));
	v[4].x = t3f_project_x(320 + 4 * 80, end_z);
	v[4].y = t3f_project_y(420 + cy + oy[4], end_z);
	v[4].z = 0;
	v[4].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	
	memcpy(&v[5], &v[4], sizeof(ALLEGRO_VERTEX));
	v[6].x = t3f_project_x(320 + 3 * 80, end_z);
	v[6].y = t3f_project_y(420 + cy + oy[3], end_z);
	v[6].z = 0;
	v[6].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	
	memcpy(&v[7], &v[6], sizeof(ALLEGRO_VERTEX));
	v[8].x = t3f_project_x(320 + 2 * 80, end_z);
	v[8].y = t3f_project_y(420 + cy + oy[2], end_z);
	v[8].z = 0;
	v[8].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	memcpy(&v[9], &v[8], sizeof(ALLEGRO_VERTEX));
	v[10].x = t3f_project_x(320 + 1 * 80, end_z);
	v[10].y = t3f_project_y(420 + cy + oy[1], end_z);
	v[10].z = 0;
	v[10].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	memcpy(&v[11], &v[10], sizeof(ALLEGRO_VERTEX));
	v[12].x = t3f_project_x(320 + 0 * 80, end_z);
	v[12].y = t3f_project_y(420 + cy + oy[0], end_z);
	v[12].z = 0;
	v[12].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	memcpy(&v[13], &v[12], sizeof(ALLEGRO_VERTEX));
	v[14].x = t3f_project_x(320 + 0 * 80 - 32, end_z);
	v[14].y = t3f_project_y(420 + cy + oy[0] + 2.0, end_z);
	v[14].z = 0;
	v[14].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	al_draw_prim(v, NULL, NULL, 0, 15, ALLEGRO_PRIM_TRIANGLE_FAN); */
	
/*	al_draw_line(320 + 0 * 80 - 32, 420 + cy + oy[0] + 2.0, 320 + 0 * 80, 420 + cy + oy[0], t3f_color_white, 2.0);
	al_draw_line(320 + 0 * 80, 420 + cy + oy[0], 320 + 1 * 80, 420 + cy + oy[1], t3f_color_white, 2.0);
	al_draw_line(320 + 1 * 80, 420 + cy + oy[1], 320 + 2 * 80, 420 + cy + oy[2], t3f_color_white, 2.0);
	al_draw_line(320 + 2 * 80, 420 + cy + oy[2], 320 + 3 * 80, 420 + cy + oy[3], t3f_color_white, 2.0);
	al_draw_line(320 + 3 * 80, 420 + cy + oy[3], 320 + 4 * 80, 420 + cy + oy[4], t3f_color_white, 2.0);
	al_draw_line(320 + 4 * 80, 420 + cy + oy[4], 320 + 4 * 80 + 32, 420 + cy + oy[4] + 2.0, t3f_color_white, 2.0); */
	
	al_hold_bitmap_drawing(true);
	al_draw_bitmap(gp->fret_board_image, 200, 320, 0);
	t3f_draw_bitmap(gp->beat_line_image, t3f_color_white, 280, 420 + 4, 0, 0);
	for(i = 0; i < gp->song->beats; i++)
	{
		z = ((gp->song->beat[i]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
		t3f_draw_bitmap(gp->beat_line_image, t3f_color_white, 280, 420 + 4, z, 0);
		if(z > 2048)
		{
			break;
		}
	}
	al_hold_bitmap_drawing(false);

	/* render note tails */
	for(i = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes - 1; i >= 0; i--)
	{
		playing = false;
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->active)
		{
			color = color_chart[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val];
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
				z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->play_tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
			}
			end_z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->length - (gp->current_tick - gp->av_delay))) * gp->board_speed;
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
				v[0].z = 0;
				v[0].color = color;
				v[1].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, z);
				v[1].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], z);
				v[1].z = 0;
				v[1].color = color;
				v[2].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
				v[2].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[2].z = 0;
				v[2].color = color;
				v[3].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 + 8, end_z);
				v[3].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[3].z = 0;
				v[3].color = color;
				v[4].x = t3f_project_x(320 + gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val * 80 - 8, end_z);
				v[4].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val], end_z);
				v[4].z = 0;
				v[4].color = color;
				al_draw_prim(v, NULL, NULL, 0, 5, ALLEGRO_PRIM_TRIANGLE_FAN);
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
			z = ((gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
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
				note_type = 5;
			}
			else
			{
				note_type = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->val;
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
		t3f_draw_rotated_bitmap(gp->note_texture[i], al_map_rgba_f(a, a, a, a), c, cy, 320 + i * 80, 420 + cy + oy[i], 0, rotate[i], 0);
	}
}
