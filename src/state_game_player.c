#include "t3f/t3f.h"
#include "t3f/view.h"
#include "t3f/draw.h"
#include "t3f/input.h"

#include "modules/obfuscate.h"

#include "struct_game.h"

static bool lss_player_set_next_notes(LSS_SONG * sp, LSS_PLAYER * pp, int cur_note)
{
	int cur_tick = sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick;

	pp->next_notes.notes = 0;
	while(cur_note < sp->track[pp->selected_track][pp->selected_difficulty].notes && sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick == cur_tick)
	{
		pp->next_notes.note[pp->next_notes.notes] = cur_note;
		pp->next_notes.notes++;
		cur_note++;
	}
	return true;
}

static bool lss_player_get_next_notes(LSS_SONG * sp, LSS_PLAYER * pp)
{
	int cur_note = pp->next_notes.note[pp->next_notes.notes - 1];
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

	pp->next_notes.notes = 0;
	while(cur_note < sp->track[pp->selected_track][pp->selected_difficulty].notes)
	{
		/* found next note */
		if(sp->track[pp->selected_track][pp->selected_difficulty].note[cur_note]->tick != cur_tick)
		{
			return lss_player_set_next_notes(sp, pp, cur_note);
		}
		cur_note++;
	}
	return false;
}

static bool lss_player_check_notes(LSS_SONG * sp, LSS_PLAYER * pp, int note[], int notes)
{
	int lane = 0;
	int tap_lane = 0;
	int i;

	for(i = 0; i < notes; i++)
	{
		lane |= (1 << sp->track[pp->selected_track][pp->selected_difficulty].note[note[i]]->val);
	}
	for(i = 0; i < 5; i++)
	{
		if(pp->controller->input->element[pp->controller->map[i]].held)
		{
			tap_lane |= (1 << i);
		}
	}
	if(lane == tap_lane)
	{
		return true;
	}
	return false;
}

static bool lss_player_check_notes_lenient(LSS_SONG * sp, LSS_PLAYER * pp, int note[], int notes)
{
	int lane = 0;
	int tap_lane = 0;
	int i;

	for(i = 0; i < notes; i++)
	{
		lane |= (1 << sp->track[pp->selected_track][pp->selected_difficulty].note[note[i]]->val);
	}
	for(i = 0; i < 5; i++)
	{
		if(pp->controller->input->element[pp->controller->map[i]].held)
		{
			tap_lane |= (1 << i);
		}
	}
	if((lane & tap_lane) == lane && (lane | tap_lane) <= (lane << 1))
	{
		return true;
	}
	return false;
}

static int lss_player_check_visibility(double z, double end_z)
{
	if(z <= 2048.0 + 128.0 && end_z > -640.0)
	{
		return 0;
	}
	if(z > 2048.0 + 128.0)
	{
		return 1;
	}
	return -1;
}

static int lss_player_note_visible(LSS_GAME * gp, int player, int note)
{
	double z, end_z;

//	z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[note]->start_z - (gp->camera_z - gp->delay_z))) * gp->board_speed;
//	end_z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[note]->end_z - (gp->camera_z - gp->delay_z))) * gp->board_speed;
	if(note < gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes)
	{
		z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[note]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
		end_z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[note]->tick + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[note]->length - (gp->current_tick - gp->av_delay))) * gp->board_speed;
		return lss_player_check_visibility(z, end_z);
	}
	return 0;
}

static int lss_player_beat_visible(LSS_GAME * gp, int beat)
{
	double z;

//	z = ((gp->song->beat[beat]->z - (gp->camera_z - gp->delay_z))) * gp->board_speed;
	if(beat < gp->song->beats)
	{
		z = ((gp->song->beat[beat]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
		return lss_player_check_visibility(z, z);
	}
	return 0;
}

static void lss_player_detect_visible_notes(LSS_GAME * gp, int player)
{
	/* no visible notes yet, so keep checking first note until it is visible */
	if(gp->player[player].first_visible_note < 0)
	{
		if(!lss_player_note_visible(gp, player, 0))
		{
			gp->player[player].first_visible_note = 0;
			gp->player[player].last_visible_note = 0;
			while(!lss_player_note_visible(gp, player, gp->player[player].last_visible_note))
			{
				gp->player[player].last_visible_note++;
				if(gp->player[player].last_visible_note >= gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes - 1)
				{
					break;
				}
			}
		}
	}
	else
	{
		if(gp->player[player].first_visible_note < gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes - 1)
		{
			while(lss_player_note_visible(gp, player, gp->player[player].first_visible_note) < 0)
			{
				gp->player[player].first_visible_note++;
				if(gp->player[player].first_visible_note >= gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes - 1)
				{
					break;
				}
			}
		}
		if(gp->player[player].last_visible_note < gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes - 1)
		{
			while(!lss_player_note_visible(gp, player, gp->player[player].last_visible_note))
			{
				gp->player[player].last_visible_note++;
				if(gp->player[player].last_visible_note >= gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].notes - 1)
				{
					break;
				}
			}
		}
	}
}

static void lss_player_detect_visible_beats(LSS_GAME * gp, int player)
{
	/* no visible notes yet, so keep checking first note until it is visible */
	if(gp->player[player].first_visible_beat < 0)
	{
		gp->player[player].first_visible_beat = 0;
		while(lss_player_beat_visible(gp, gp->player[player].first_visible_beat))
		{
			gp->player[player].first_visible_beat++;
			if(gp->player[player].first_visible_beat >= gp->song->beats)
			{
				break;
			}
		}
		gp->player[player].last_visible_beat = gp->player[player].first_visible_beat;
		while(!lss_player_beat_visible(gp, gp->player[player].last_visible_beat))
		{
			gp->player[player].last_visible_beat++;
			if(gp->player[player].last_visible_beat >= gp->song->beats)
			{
				break;
			}
		}
	}
	else
	{
		if(gp->player[player].first_visible_beat < gp->song->beats - 1)
		{
			while(lss_player_beat_visible(gp, gp->player[player].first_visible_beat) < 0)
			{
				gp->player[player].first_visible_beat++;
				if(gp->player[player].first_visible_beat >= gp->song->beats - 1)
				{
					break;
				}
			}
		}
		if(gp->player[player].last_visible_beat < gp->song->beats - 1)
		{
			while(!lss_player_beat_visible(gp, gp->player[player].last_visible_beat))
			{
				gp->player[player].last_visible_beat++;
				if(gp->player[player].last_visible_beat >= gp->song->beats - 1)
				{
					break;
				}
			}
		}
	}
}

void lss_player_reset_beat_markers(LSS_GAME * gp, int player)
{
	gp->player[player].first_visible_beat = -1;
	lss_player_detect_visible_beats(gp, player);
}

void lss_initialize_player(LSS_GAME * gp, int player)
{
	const char * val;
	char buf[64] = {0};

	gp->player[player].playing_notes.notes = 0;
	gp->player[player].next_notes.note[0] = -1;
	gp->player[player].next_notes.notes = 1;
	gp->player[player].streak = 0;
	gp->player[player].multiplier = 1;
	gp->player[player].life = 100;
	gp->player[player].miss_streak = 0;
	gp->player[player].full_combo = true;
	gp->player[player].score = 0;
	gp->player[player].high_score = 0;
	sprintf(buf, "high_score_%d_%d", gp->player[player].selected_track, gp->player[player].selected_difficulty);
	val = al_get_config_value(gp->player[player].profile->config, gp->song_id, buf);
	if(val)
	{
		gp->player[player].high_score = lss_unobfuscate_value(atoi(val));
	}
	if(gp->player[player].controller->source == LSS_CONTROLLER_SOURCE_TOUCH)
	{
		gp->player[player].controller->block_strum = false;
	}
	gp->player[player].first_visible_note = -1;
	gp->player[player].last_visible_note = -1;
	gp->player[player].first_visible_beat = -1;
	gp->player[player].last_visible_beat = -1;
	lss_player_get_next_notes(gp->song, &gp->player[player]);
	gp->player[player].stars = 0;
}

static void handle_auto_strum(LSS_GAME * gp, int player)
{
	int i;
	bool allow_strum = true;
	int held = 0;

	for(i = 0; i < 5; i++)
	{
		if(gp->player[player].controller->input->element[gp->player[player].controller->map[i]].held)
		{
			gp->player[player].controller->fret_button_tick[i]++;
			if(gp->player[player].controller->fret_button_tick[i] > 10)
			{
				allow_strum = false;
			}
			held++;
		}
		else
		{
			gp->player[player].controller->fret_button_tick[i] = -1;
		}
	}
	if(held <= 0)
	{
		gp->player[player].controller->block_strum = false;
	}

	for(i = 0; i < gp->player[player].hittable_notes_groups; i++)
	{
		if(lss_player_check_notes(gp->song, &gp->player[player], gp->player[player].hittable_notes[i].note, gp->player[player].hittable_notes[i].notes))
		{
			if(allow_strum && !gp->player[player].controller->block_strum)
			{
				gp->player[player].controller->input->element[gp->player[player].controller->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN]].pressed = true;
				gp->player[player].controller->block_strum = true;
			}
		}
	}
}

void lss_player_logic(LSS_GAME * gp, int player)
{
	int i, j, d, d2, t, accuracy = 2;
	bool dropped = false;
	int points = 0;
	int stream;
	int life_add;
	int cur_tick = -1;
	int group = -1;
	int missed_notes = 0;
	int missed_groups = 0;
	bool hopo_strum = false;
	bool hopo_strum_check = false; // true if we needed to check next_notes for valid strummable note
	bool hopo_strum_pass = false;
	bool strum = false;

	lss_player_detect_visible_notes(gp, player);
	lss_player_detect_visible_beats(gp, player);

	/* move on to next note if we missed this one, do this first because logic
	 * below assumes next_notes is current */
	if(gp->player[player].next_notes.notes)
	{
		d = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[0]]->tick - (gp->current_tick - gp->av_delay)));
		if(d < -8)
		{
			if(gp->song_audio->streams > 1)
			{
				stream = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].stream;
				if(stream >= 0 && gp->song_audio->stream[stream])
				{
					al_set_audio_stream_gain(gp->song_audio->stream[stream], gp->missed_audio_gain);
				}
			}
			for(i = 0; i < gp->player[player].next_notes.notes; i++)
			{
				gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_MISS;
			}
			missed_notes = gp->player[player].next_notes.notes;
			missed_groups = 1;
			lss_player_get_next_notes(gp->song, &gp->player[player]);
		}
	}

	/* find all hittable notes */
	if(gp->player[player].next_notes.notes > 0)
	{
		/* only allow notes at or past current note to be hittable */
		for(i = gp->player[player].next_notes.note[0]; i <= gp->player[player].last_visible_note; i++)
		{
			d = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay)));
			if(d >= -8 && d <= 8)
			{
				/* first hittable note */
				if(cur_tick < 0)
				{
					cur_tick = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick;
					group = 0;
					gp->player[player].hittable_notes[group].notes = 0;
				}

				/* move on to next group */
				else if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick != cur_tick)
				{
					group++;
					gp->player[player].hittable_notes[group].notes = 0;
					cur_tick = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick;
				}

				/* add note to current group */
				gp->player[player].hittable_notes[group].note[gp->player[player].hittable_notes[group].notes] = i;
				gp->player[player].hittable_notes[group].notes++;
			}
		}
	}
	gp->player[player].hittable_notes_groups = group + 1;

	/* check for note hits */
	if(gp->song->track[gp->player[player].selected_track]->type == LSS_SONG_TRACK_TYPE_GAMEPAD)
	{
		handle_auto_strum(gp, player);
	}
	if(gp->player[player].controller->input->element[gp->player[player].controller->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN]].pressed || gp->player[player].controller->input->element[gp->player[player].controller->map[LSS_CONTROLLER_BINDING_GUITAR_STRUM_UP]].pressed)
	{
		if(!gp->player[player].block_menu_strum)
		{
			strum = true;
		}
	}
	if(strum)
	{
		/* first check to see if we have strummed a HOPO note */
		if(gp->player[player].playing_notes.notes && gp->player[player].playing_notes.hopo)
		{
			if(lss_player_check_notes(gp->song, &gp->player[player], gp->player[player].playing_notes.note, gp->player[player].playing_notes.notes))
			{
				/* see if we may also be strumming the next note */
				if(gp->player[player].next_notes.notes && lss_player_check_notes(gp->song, &gp->player[player], gp->player[player].next_notes.note, gp->player[player].next_notes.notes))
				{
					hopo_strum_check = true;
					d = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[0]]->tick - (gp->current_tick - gp->av_delay)));
				}

				/* have strummed the next note as well as the previous note,
				 * only eat strum of the HOPO note is closer than the next note */
				if(hopo_strum_check)
				{
					d2 = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[0]]->tick - (gp->current_tick - gp->av_delay)));
					if(abs(d2) < abs(d))
					{
						hopo_strum_pass = true;
					}
				}

				/* if the strummed note only matches the HOPO note, eat the strum */
				else
				{
					hopo_strum_pass = true;
				}
			}
		}
		if(hopo_strum_pass)
		{
			gp->player[player].playing_notes.hopo = false; // prevent extra strums from going unnoticed
			hopo_strum = true;
			gp->player[player].score -= gp->player[player].playing_notes.hopo_points;

			/* adjust note accuracy to match strum position */
			for(i = 0; i < gp->player[player].playing_notes.notes; i++)
			{
				if(d >= -1.0 && d <= 1.0)
				{
					gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_PERFECT;
					accuracy = 4;
				}
				else if(d >= -2.0 && d <= 2.0)
				{
					gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_GOOD;
					accuracy = 3;
				}
				else if(d >= -5.0 && d <= 5.0)
				{
					gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_OKAY;
					accuracy = 2;
				}
				else
				{
					gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_BAD;
					accuracy = 1;
				}
			}
		}

		/* add adjusted points to score */
		points = gp->player[player].playing_notes.notes * (LSS_GAME_NOTE_BASE_POINTS * accuracy) * gp->player[player].multiplier;
		gp->player[player].score += points;

		/* if we have strummed a HOPO note, ignore the rest of the logic so we
		 * don't kill the streak or mess with the next_notes */
		if(!hopo_strum)
		{
			for(i = 0; i < gp->player[player].hittable_notes_groups; i++)
			{
				/* if note is within hit window */
				d = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[0]]->tick - (gp->current_tick - gp->av_delay)));
				if(lss_player_check_notes(gp->song, &gp->player[player], gp->player[player].hittable_notes[i].note, gp->player[player].hittable_notes[i].notes))
				{
					if(gp->song_audio->streams > 1)
					{
						stream = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].stream;
						if(stream >= 0 && gp->song_audio->stream[stream])
						{
							al_set_audio_stream_gain(gp->song_audio->stream[stream], gp->playing_audio_gain);
						}
					}
					for(j = 0; j < gp->player[player].hittable_notes[i].notes; j++)
					{
						gp->player[player].playing_notes.note[j] = gp->player[player].hittable_notes[i].note[j];
						gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->visible = false;
						gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->play_tick = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->tick;
						if(d >= -1.0 && d <= 1.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_PERFECT;
							accuracy = 4;
						}
						else if(d >= -2.0 && d <= 2.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_GOOD;
							accuracy = 3;
						}
						else if(d >= -5.0 && d <= 5.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_OKAY;
							accuracy = 2;
						}
						else
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].hittable_notes[i].note[j]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_BAD;
							accuracy = 1;
						}
					}
					gp->player[player].playing_notes.notes = gp->player[player].hittable_notes[i].notes;
					gp->player[player].playing_notes.hopo = false;
					gp->player[player].multiplier = gp->player[player].streak / 8 + 1;
					if(gp->player[player].multiplier > 4)
					{
						gp->player[player].multiplier = 4;
					}
					points = gp->player[player].playing_notes.notes * (LSS_GAME_NOTE_BASE_POINTS * accuracy) * gp->player[player].multiplier;
					gp->player[player].score += points;
					gp->player[player].streak++;
					life_add = gp->player[player].streak;
					if(life_add > 4)
					{
						life_add = 4;
					}
					gp->player[player].miss_streak = 0;
					gp->player[player].life += life_add;
					if(gp->player[player].life > 100)
					{
						gp->player[player].life = 100;
					}
					missed_notes += gp->player[player].hittable_notes[i].note[0] - gp->player[player].next_notes.note[0];
					missed_groups += i;
					lss_player_set_next_notes(gp->song, &gp->player[player], gp->player[player].hittable_notes[i].note[0]);
					lss_player_get_next_notes(gp->song, &gp->player[player]);
					break;
				}
			}

			/* no note matches means we should kill the combo */
			if(i == gp->player[player].hittable_notes_groups)
			{
				if(gp->song_audio->streams > 1)
				{
					stream = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].stream;
					if(stream >= 0 && gp->song_audio->stream[stream])
					{
						al_set_audio_stream_gain(gp->song_audio->stream[stream], gp->missed_audio_gain);
					}
				}
				gp->player[player].playing_notes.notes = 0;
				gp->player[player].streak = 0;
				gp->player[player].multiplier = 1;
				gp->player[player].full_combo = false;
			}
		}
	}
	else
	{
		gp->player[player].block_menu_strum = false;
		/* see if we are holding a sustain */
		if(gp->player[player].playing_notes.notes)
		{
			/* see if we have reached the end of the note */
			t = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[0]]->tick + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[0]]->length;
			if(gp->current_tick - gp->av_delay > t)
			{
				gp->player[player].playing_notes.notes = 0;
			}

			/* continue sustain */
			else
			{
				if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[0]]->hopo)
				{
					if(!lss_player_check_notes_lenient(gp->song, &gp->player[player], gp->player[player].playing_notes.note, gp->player[player].playing_notes.notes))
					{
						dropped = true;
					}
				}
				else
				{
					if(!lss_player_check_notes(gp->song, &gp->player[player], gp->player[player].playing_notes.note, gp->player[player].playing_notes.notes))
					{
						dropped = true;
					}
				}
				if(dropped)
				{
					if(gp->song_audio->streams > 1)
					{
						stream = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].stream;
						if(stream >= 0 && gp->song_audio->stream[stream])
						{
//							al_set_audio_stream_gain(gp->song_audio->stream[stream], 0.0);
						}
					}
					gp->player[player].playing_notes.notes = 0;
				}
				else
				{
					if(gp->current_tick - gp->av_delay >= gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[0]]->tick)
					{
						for(i = 0; i < gp->player[player].playing_notes.notes; i++)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].playing_notes.note[i]]->play_tick = gp->current_tick - gp->av_delay;
						}
						gp->player[player].multiplier = gp->player[player].streak / 8 + 1;
						if(gp->player[player].multiplier > 4)
						{
							gp->player[player].multiplier = 4;
						}
						points = gp->player[player].playing_notes.notes * LSS_GAME_NOTE_SUSTAIN_BASE_POINTS * gp->player[player].multiplier;
						gp->player[player].score += points;
					}
				}
			}
		}

		/* see if we are hitting a HOPO note */
		if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[0]]->hopo && gp->player[player].streak > 0 && missed_groups == 0)
		{
			d = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[0]]->tick - (gp->current_tick - gp->av_delay)));
			if(d >= -8 && d <= 8)
			{
				if(lss_player_check_notes_lenient(gp->song, &gp->player[player], gp->player[player].next_notes.note, gp->player[player].next_notes.notes))
				{
					if(gp->song_audio->streams > 1)
					{
						stream = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].stream;
						if(stream >= 0 && gp->song_audio->stream[stream])
						{
							al_set_audio_stream_gain(gp->song_audio->stream[stream], gp->playing_audio_gain);
						}
					}
					for(i = 0; i < gp->player[player].next_notes.notes; i++)
					{
						gp->player[player].playing_notes.note[i] = gp->player[player].next_notes.note[i];
						gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->visible = false;
						gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->play_tick = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->tick;
						if(d >= -1.0 && d <= 1.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_PERFECT;
							accuracy = 4;
						}
						else if(d >= -2.0 && d <= 2.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_GOOD;
							accuracy = 3;
						}
						else if(d >= -5.0 && d <= 5.0)
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_OKAY;
							accuracy = 2;
						}
						else
						{
							gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[gp->player[player].next_notes.note[i]]->hit_level = LSS_SONG_NOTE_HIT_LEVEL_BAD;
							accuracy = 1;
						}
					}
					gp->player[player].playing_notes.notes = gp->player[player].next_notes.notes;
					gp->player[player].playing_notes.hopo = true;
					gp->player[player].multiplier = gp->player[player].streak / 8 + 1;
					if(gp->player[player].multiplier > 4)
					{
						gp->player[player].multiplier = 4;
					}
					points = gp->player[player].playing_notes.notes * (LSS_GAME_NOTE_BASE_POINTS * accuracy) * gp->player[player].multiplier;
					gp->player[player].playing_notes.hopo_points = points;
					gp->player[player].score += points;
					gp->player[player].streak++;
					gp->player[player].miss_streak = 0;
					gp->player[player].life += gp->player[player].streak;
					if(gp->player[player].life > 100)
					{
						gp->player[player].life = 100;
					}
					lss_player_get_next_notes(gp->song, &gp->player[player]);
				}
			}
		}
	}
	if(missed_groups)
	{
		gp->player[player].streak = 0;
		gp->player[player].multiplier = 1;
//		gp->player[player].missed_notes += missed_notes;
		gp->player[player].full_combo = false;
		for(i = 0; i < missed_groups; i++)
		{
			gp->player[player].miss_streak++;
			if(gp->player[player].miss_streak > 4)
			{
				gp->player[player].miss_streak = 4;
			}
			if(!gp->no_fail)
			{
				gp->player[player].life -= gp->player[player].miss_streak;
			}
		}
	}
}

static void lss_player_render_primitive_fretboard(LSS_GAME * gp, int player)
{
	float oy[5] = {3.0 - 4.0, 1.0 - 4.0, 0.0 - 4.0, 1.0 - 4.0, 3.0 - 4.0};
	float c, cy, z, end_z;
	ALLEGRO_VERTEX v[32];

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c + c / 4 - 2;
	z = -480;
	end_z = 2048;
	v[0].x = t3f_project_x(320 - 40, z);
	v[0].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[0].z = 0;
	v[0].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[1].x = t3f_project_x(320 + 4 * 80 + 40, z);
	v[1].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[1].z = 0;
	v[1].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[2].x = t3f_project_x(320 + 4 * 80 + 40, end_z);
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
	v[14].x = t3f_project_x(320 + 0 * 80 - 40, end_z);
	v[14].y = t3f_project_y(420 + cy + oy[0] + 2.0, end_z);
	v[14].z = 0;
	v[14].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);

	al_draw_prim(v, NULL, NULL, 0, 15, ALLEGRO_PRIM_TRIANGLE_FAN);
}

static void lss_player_render_primitive_buttons(LSS_GAME * gp, int player)
{
	float oy[5] = {3.0 - 4.0, 1.0 - 4.0, 0.0 - 4.0, 1.0 - 4.0, 3.0 - 4.0};
	float c, cy, z, end_z;
	ALLEGRO_VERTEX v[32];

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c + c / 4 - 2;
	z = -32;
	end_z = 32;
	v[0].x = t3f_project_x(320 - 40, z);
	v[0].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[0].z = 0;
	v[0].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[1].x = t3f_project_x(320 + 4 * 80 + 40, z);
	v[1].y = t3f_project_y(420 + cy + oy[0] + 2.0, z);
	v[1].z = 0;
	v[1].color = al_map_rgba_f(0.0, 0.0, 0.0, 0.5);
	v[2].x = t3f_project_x(320 + 4 * 80 + 40, end_z);
	v[2].y = t3f_project_y(420 + cy + oy[0] + 2.0, end_z);
	v[2].z = 0;
	v[2].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[3], &v[2], sizeof(ALLEGRO_VERTEX));
	v[4].x = t3f_project_x(320 + 4 * 80, end_z);
	v[4].y = t3f_project_y(420 + cy + oy[4], end_z);
	v[4].z = 0;
	v[4].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[5], &v[4], sizeof(ALLEGRO_VERTEX));
	v[6].x = t3f_project_x(320 + 3 * 80, end_z);
	v[6].y = t3f_project_y(420 + cy + oy[3], end_z);
	v[6].z = 0;
	v[6].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[7], &v[6], sizeof(ALLEGRO_VERTEX));
	v[8].x = t3f_project_x(320 + 2 * 80, end_z);
	v[8].y = t3f_project_y(420 + cy + oy[2], end_z);
	v[8].z = 0;
	v[8].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[9], &v[8], sizeof(ALLEGRO_VERTEX));
	v[10].x = t3f_project_x(320 + 1 * 80, end_z);
	v[10].y = t3f_project_y(420 + cy + oy[1], end_z);
	v[10].z = 0;
	v[10].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[11], &v[10], sizeof(ALLEGRO_VERTEX));
	v[12].x = t3f_project_x(320 + 0 * 80, end_z);
	v[12].y = t3f_project_y(420 + cy + oy[0], end_z);
	v[12].z = 0;
	v[12].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	memcpy(&v[13], &v[12], sizeof(ALLEGRO_VERTEX));
	v[14].x = t3f_project_x(320 + 0 * 80 - 40, end_z);
	v[14].y = t3f_project_y(420 + cy + oy[0] + 2.0, end_z);
	v[14].z = 0;
	v[14].color = al_map_rgba_f(0.5, 0.5, 0.5, 1.0);

	al_draw_prim(v, NULL, NULL, 0, 15, ALLEGRO_PRIM_TRIANGLE_FAN);
}

static void lss_player_render_primitive_beat_line(LSS_GAME * gp, int player, double z)
{
	float oy[5] = {3.0 - 4.0, 1.0 - 4.0, 0.0 - 4.0, 1.0 - 4.0, 3.0 - 4.0};
	float c, cy;

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c + c / 4 - 2;
	al_draw_line(t3f_project_x(320 + 0 * 80 - 36, z), t3f_project_y(420 + cy + oy[0] + 2.0, z), t3f_project_x(320 + 0 * 80, z), t3f_project_y(420 + cy + oy[0], z), t3f_color_white, 2.0);
	al_draw_line(t3f_project_x(320 + 0 * 80, z), t3f_project_y(420 + cy + oy[0], z), t3f_project_x(320 + 1 * 80, z), t3f_project_y(420 + cy + oy[1], z), t3f_color_white, 2.0);
	al_draw_line(t3f_project_x(320 + 1 * 80, z), t3f_project_y(420 + cy + oy[1], z), t3f_project_x(320 + 2 * 80, z), t3f_project_y(420 + cy + oy[2], z), t3f_color_white, 2.0);
	al_draw_line(t3f_project_x(320 + 2 * 80, z), t3f_project_y(420 + cy + oy[2], z), t3f_project_x(320 + 3 * 80, z), t3f_project_y(420 + cy + oy[3], z), t3f_color_white, 2.0);
	al_draw_line(t3f_project_x(320 + 3 * 80, z), t3f_project_y(420 + cy + oy[3], z), t3f_project_x(320 + 4 * 80, z), t3f_project_y(420 + cy + oy[4], z), t3f_color_white, 2.0);
	al_draw_line(t3f_project_x(320 + 4 * 80, z), t3f_project_y(420 + cy + oy[4], z), t3f_project_x(320 + 4 * 80 + 36, z), t3f_project_y(420 + cy + oy[4] + 2.0, z), t3f_color_white, 2.0);
}

static ALLEGRO_COLOR lss_alpha_color(ALLEGRO_COLOR c1, float alpha)
{
	float r1, g1, b1, a1;

	al_unmap_rgba_f(c1, &r1, &g1, &b1, &a1);
	return al_map_rgba_f(r1 * alpha, g1 * alpha, b1 * alpha, a1 * alpha);
}

void lss_player_render_board(LSS_GAME * gp, int player)
{
	float rotate[5] = {-0.06, -0.03, 0.0, 0.03, 0.06};
	float oy[5] = {3.0 - 4.0, 1.0 - 4.0, 0.0 - 4.0, 1.0 - 4.0, 3.0 - 4.0};
	ALLEGRO_VERTEX v[32];
	ALLEGRO_COLOR color;
	ALLEGRO_COLOR color_chart[5];
	double a, c, cy, z, end_z;
	int i, j;
	bool playing;
	int note_type;

	c = al_get_bitmap_width(gp->note_texture[0]) / 2;
	cy = c + c / 4 - 2;
	color_chart[0] = LSS_NOTE_COLOR_0;
	color_chart[1] = LSS_NOTE_COLOR_1;
	color_chart[2] = LSS_NOTE_COLOR_2;
	color_chart[3] = LSS_NOTE_COLOR_3;
	color_chart[4] = LSS_NOTE_COLOR_4;

	t3f_select_view(gp->player[player].view);
	al_hold_bitmap_drawing(true);
//	lss_player_render_primitive_fretboard(gp, player);
	al_draw_bitmap(gp->fret_board_image, 200, 320, 0);
//	lss_player_render_primitive_beat_line(gp, player, 0);
	t3f_draw_bitmap(gp->beat_line_image, t3f_color_white, 280, 420 + 8, 0, 0);
	if(gp->player[player].first_visible_beat >= 0)
	{
		for(i = gp->player[player].first_visible_beat; i <= gp->player[player].last_visible_beat; i++)
		{
//			z = ((gp->song->beat[i]->z - gp->camera_z)) * gp->board_speed;
			z = ((gp->song->beat[i]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
			a = 1.0;
			if(z > 1984)
			{
				a = 1.0 - (z - 1984.0) / 128.0;
				if(a < 0.0)
				{
					a = 0.0;
				}
			}
			t3f_draw_bitmap(gp->beat_line_image, al_map_rgba_f(a, a, a, a), 280, 420 + 8, z, 0);
			if(z > 1984 + 128.0)
			{
				break;
			}
		}
	}
	al_hold_bitmap_drawing(false);
	al_draw_bitmap(gp->fret_buttons_image, 200, 320, 0);
	for(i = 0; i < 5; i++)
	{
		if(!gp->paused && gp->player[player].controller->input->element[gp->player[player].controller->map[i]].held)
		{
			a = 1.0;
		}
		else
		{
			a = 0.5;
		}
		t3f_draw_rotated_bitmap(gp->fret_button_texture[i], al_map_rgba_f(a, a, a, 1.0), c, cy, 320 + i * 80, 420 + cy + oy[i], 0, rotate[i], 0);
	}


	/* render note tails */
	if(gp->player[player].first_visible_note >= 0)
	{
		for(i = gp->player[player].last_visible_note; i >= gp->player[player].first_visible_note; i--)
		{
			playing = false;
			if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->active && !gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->hidden)
			{
				color = color_chart[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val];
				for(j = 0; j < gp->player[player].playing_notes.notes; j++)
				{
					if(gp->player[player].playing_notes.note[j] == i)
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
//					z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->start_z - gp->camera_z)) * gp->board_speed;
					z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->play_tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
				}
//				end_z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->end_z - gp->camera_z)) * gp->board_speed;
				end_z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->length - (gp->current_tick - gp->av_delay))) * gp->board_speed;
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
					v[0].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 - 8, z);
					v[0].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], z);
					v[0].z = 0;
					v[0].color = color;
					v[1].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 + 8, z);
					v[1].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], z);
					v[1].z = 0;
					v[1].color = color;
					v[2].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 + 8, end_z);
					v[2].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], end_z);
					v[2].z = 0;
					v[2].color = color;
					v[3].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 + 8, end_z);
					v[3].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], end_z);
					v[3].z = 0;
					v[3].color = color;
					v[4].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 - 8, end_z);
					v[4].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], end_z);
					v[4].z = 0;
					v[4].color = color;
					v[5].x = t3f_project_x(320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80 - 8, z);
					v[5].y = t3f_project_y(420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], z);
					v[5].z = 0;
					v[5].color = color;
					t3f_cache_primitive(gp->primitives, v, 6);
				}
			}
		}
		t3f_flush_cached_primitives(gp->primitives, NULL, ALLEGRO_PRIM_TRIANGLE_LIST);

		/* render notes */
		al_hold_bitmap_drawing(true);
		for(i = gp->player[player].last_visible_note; i >= gp->player[player].first_visible_note; i--)
		{
			playing = false;
			if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->active && !gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->hidden)
			{
				for(j = 0; j < gp->player[player].playing_notes.notes; j++)
				{
					if(gp->player[player].playing_notes.note[j] == i)
					{
						playing = true;
					}
				}
				if(playing)
				{
					z = 0.0;
				}
				else
				{
					z = ((gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->tick - (gp->current_tick - gp->av_delay))) * gp->board_speed;
				}
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
				if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->hopo)
				{
	//				a *= 0.5;
					note_type = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val + 5;
				}
				else
				{
					note_type = gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val;
				}
				if(gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->visible || playing)
				{
					t3f_draw_rotated_bitmap(gp->note_texture[note_type], al_map_rgba_f(a, a, a, a), c, cy, 320 + gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val * 80, 420 + cy + oy[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], z, rotate[gp->song->track[gp->player[player].selected_track][gp->player[player].selected_difficulty].note[i]->val], 0);
				}
			}
		}
	}

	/* render on-screen control helpers when using touch */
	if(gp->player[player].controller->source == LSS_CONTROLLER_SOURCE_TOUCH)
	{
		c = al_get_bitmap_width(gp->fret_button_image) / 2;
		for(i = 0; i < 5; i++)
		{
			if(gp->player[player].controller->input->element[gp->player[player].controller->map[i]].held)
			{
				a = 1.0;
			}
			else
			{
				a = 0.25;
			}
			t3f_draw_bitmap(gp->fret_button_image, lss_alpha_color(color_chart[i], a), LSS_TOUCH_OFFSET_X + i * LSS_TOUCH_SPACE_X, LSS_TOUCH_OFFSET_Y, 0, 0);
		}
	}
	al_hold_bitmap_drawing(false);
}
