#include "song.h"

static int lss_song_difficulty_range_start[5] = {60, 72, 84, 96};

static int lss_song_get_event_difficulty(LSS_SONG * sp, int track, int event)
{
	int difficulty = -1;
	if(sp->source_midi->track[track]->event[event]->data_i[0] >= lss_song_difficulty_range_start[0] &&
	   sp->source_midi->track[track]->event[event]->data_i[0] < lss_song_difficulty_range_start[0] + 12)
	{
		difficulty = 0;
	}
	else if(sp->source_midi->track[track]->event[event]->data_i[0] >= lss_song_difficulty_range_start[1] &&
	        sp->source_midi->track[track]->event[event]->data_i[0] < lss_song_difficulty_range_start[1] + 12)
	{
		difficulty = 1;
	}
	else if(sp->source_midi->track[track]->event[event]->data_i[0] >= lss_song_difficulty_range_start[2] &&
	        sp->source_midi->track[track]->event[event]->data_i[0] < lss_song_difficulty_range_start[2] + 12)
	{
		difficulty = 2;
	}
	else if(sp->source_midi->track[track]->event[event]->data_i[0] >= lss_song_difficulty_range_start[3] &&
	        sp->source_midi->track[track]->event[event]->data_i[0] < lss_song_difficulty_range_start[3] + 12)
	{
		difficulty = 3;
	}
	return difficulty;
}

/* backtrack to first event at this position in this difficulty and see if there
 * are any forced-HOPO markings, return 0 for no forced status, 1 for on, and 2 for off */
static int lss_song_get_note_forced_hopo_status(LSS_SONG * sp, int track, int event)
{
	int difficulty;
	int current_event = event;
	int last_event;
	int mark;

	difficulty = lss_song_get_event_difficulty(sp, track, event);
	/* find first event that occurs at the same tick as 'event' */
	while(sp->source_midi->track[track]->event[current_event]->tick == sp->source_midi->track[track]->event[event]->tick)
	{
		last_event = current_event;
		current_event--;
		if(current_event < 0)
		{
			current_event = 0;
			break;
		}
	}
	current_event = last_event;

	/* check all events that occur at the same tick as 'event' to see if any of
	 * them are forced-HOPO marks */
	while(sp->source_midi->track[track]->event[current_event]->tick == sp->source_midi->track[track]->event[event]->tick)
	{
		if(lss_song_get_event_difficulty(sp, track, current_event) == difficulty)
		{
			if(sp->source_midi->track[track]->event[current_event]->type == RTK_MIDI_EVENT_TYPE_NOTE_ON && sp->source_midi->track[track]->event[current_event]->data_i[1] != 0)
			{
				mark = sp->source_midi->track[track]->event[current_event]->data_i[0] - lss_song_difficulty_range_start[difficulty];

				/* found forced HOPO on */
				if(mark == 5)
				{
					return 1;
				}

				/* found forced HOPO off */
				else if(mark == 6)
				{
					return 2;
				}
			}
		}
		current_event++;
		if(current_event >= sp->source_midi->track[track]->events)
		{
			break;
		}
	}
	return 0;
}

static bool lss_song_allocate_notes(LSS_SONG * sp)
{
	int i, j, k;
	int difficulty;

	/* count number of notes in each track and difficulty so we can allocate enough memory */
	for(i = 0; i < sp->source_midi->tracks; i++)
	{
		for(j = 0; j < sp->source_midi->track[i]->events; j++)
		{
			if(sp->source_midi->track[i]->event[j]->type == RTK_MIDI_EVENT_TYPE_NOTE_ON && sp->source_midi->track[i]->event[j]->data_i[1] != 0)
			{
				difficulty = lss_song_get_event_difficulty(sp, i, j);
				if(difficulty >= 0 && sp->source_midi->track[i]->event[j]->data_i[0] - lss_song_difficulty_range_start[difficulty] < 5)
				{
					sp->track[i][difficulty].notes++;
				}
			}
		}
	}

	/* allocate memory for notes */
	for(i = 0; i < LSS_SONG_MAX_TRACKS; i++)
	{
		for(j = 0; j < LSS_SONG_MAX_DIFFICULTIES; j++)
		{
			if(sp->track[i][j].notes)
			{
				sp->track[i][j].note = malloc(sizeof(LSS_SONG_NOTE *) * sp->track[i][j].notes);
				if(!sp->track[i][j].note)
				{
					return false;
				}
				for(k = 0; k < sp->track[i][j].notes; k++)
				{
					sp->track[i][j].note[k] = malloc(sizeof(LSS_SONG_NOTE));
					if(!sp->track[i][j].note[k])
					{
						return false;
					}
					memset(sp->track[i][j].note[k], 0, sizeof(LSS_SONG_NOTE));
				}
			}
		}
	}
	return true;
}

static int lss_song_get_note_end_event(LSS_SONG * sp, int track, int note_on_event)
{
	int i;

	for(i = note_on_event; i < sp->source_midi->track[track]->events; i++)
	{
		if(sp->source_midi->track[track]->event[i]->type == RTK_MIDI_EVENT_TYPE_NOTE_OFF && sp->source_midi->track[track]->event[i]->data_i[0] == sp->source_midi->track[track]->event[note_on_event]->data_i[0])
		{
			return i;
		}
		if(sp->source_midi->track[track]->event[i]->type == RTK_MIDI_EVENT_TYPE_NOTE_ON && sp->source_midi->track[track]->event[i]->data_i[1] == 0 && sp->source_midi->track[track]->event[i]->data_i[0] == sp->source_midi->track[track]->event[note_on_event]->data_i[0])
		{
			return i;
		}
	}
	return -1;
}

static double rtk_tick_to_real_time(int division, double bpm, unsigned long ticks)
{
	return ticks / (double)division * ((double)60.0 / (bpm));
}

/* convert MIDI tick to real time (seconds), must be called after rtk_build_tempo_map() */
static double lss_song_get_z(RTK_MIDI * mp, unsigned long tick, double offset)
{
	double current_bpm = 120.0; // default to 120 BPM (MIDI standard)
	double time_span = 0.0;
	int current_tick = 0;
	int i;
	double z = 0.0;

	if(mp->tempo_events)
	{
		i = 0;
		while(i < mp->tempo_events && mp->tempo_event[i]->tick < tick)
		{
			current_bpm = rtk_ppqn_to_bpm(mp->tempo_event[i]->data_i[0]);
			i++;
			if(i < mp->tempo_events && mp->tempo_event[i]->tick < tick)
			{
				current_tick = mp->tempo_event[i]->tick;
				time_span = (mp->tempo_event[i]->pos_sec - mp->tempo_event[i - 1]->pos_sec);
				z += ((time_span * 60.0) * (double)LSS_SONG_PLACEMENT_SCALE) * (current_bpm / 120.0);
			}
		}
	}

	/* add the remaining time from the tempo change to the desired tick */
	time_span = rtk_tick_to_real_time(mp->raw_data->divisions, current_bpm, tick - current_tick);
	z += (((time_span + offset) * 60.0) * (double)LSS_SONG_PLACEMENT_SCALE) * (current_bpm / 120.0);

	return z;
}

static bool lss_song_populate_tracks(LSS_SONG * sp)
{
	int i, j, k, d;
	int difficulty;
	int note_off_event;
	int previous_note_tick[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
	bool chord[16] = {false};
	int hopo_threshold, hopo_forced;
	int stream;

	hopo_threshold = sp->source_midi->raw_data->divisions / 3; // 12th note
	for(i = 0; i < sp->source_midi->tracks; i++)
	{
		for(j = 0; j < 16; j++)
		{
			previous_note_tick[j] = -1;
			chord[j] = false;
		}
		stream = -1;
		if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "T1 GEMS"))
		{
			stream = 1;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_INSTRUMENT;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART GUITAR"))
		{
			stream = 1;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_INSTRUMENT;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART BASS"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_INSTRUMENT;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART GUITAR COOP"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_INSTRUMENT;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART RHYTHM"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART DRUMS"))
		{
			stream = 3;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_INSTRUMENT;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART GUITAR C"))
		{
			stream = 1;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "LEAD C"))
		{
			stream = 1;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART BASS C"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART GUITAR COOP C"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART RHYTHM C"))
		{
			stream = 2;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "PART DRUMS C"))
		{
			stream = 3;
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		else if(sp->source_midi->track[i]->name && !strcmp(sp->source_midi->track[i]->name, "VOCALS C"))
		{
			for(k = 0; k < 4; k++)
			{
				sp->track[i][k].type = LSS_SONG_TRACK_TYPE_GAMEPAD;
			}
		}
		for(j = 0; j < sp->source_midi->track[i]->events; j++)
		{
			if(sp->source_midi->track[i]->event[j]->type == RTK_MIDI_EVENT_TYPE_NOTE_ON && sp->source_midi->track[i]->event[j]->data_i[1] != 0)
			{
				difficulty = lss_song_get_event_difficulty(sp, i, j);
				if(difficulty >= 0 && sp->source_midi->track[i]->event[j]->data_i[0] - lss_song_difficulty_range_start[difficulty] < 5)
				{
					sp->track[i][difficulty].stream = stream;
					note_off_event = lss_song_get_note_end_event(sp, i, j);
					if(note_off_event >= 0)
					{
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->val = sp->source_midi->track[i]->event[j]->data_i[0] - lss_song_difficulty_range_start[difficulty];
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick = (sp->source_midi->track[i]->event[j]->pos_sec + sp->offset) * 60.0;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->play_tick = sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->length = (sp->source_midi->track[i]->event[note_off_event]->pos_sec + sp->offset) * 60.0 - sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->active = true;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->visible = true;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->start_z = lss_song_get_z(sp->source_midi, sp->source_midi->track[i]->event[j]->tick, sp->offset);
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->end_z = lss_song_get_z(sp->source_midi, sp->source_midi->track[i]->event[note_off_event]->tick, sp->offset);

						hopo_forced = lss_song_get_note_forced_hopo_status(sp, i, j);

						/* check for auto HOPO status (12th note or shorter) */
						if(previous_note_tick[difficulty] >= 0)
						{
							d = sp->source_midi->track[i]->event[j]->tick - previous_note_tick[difficulty];
							if(d > 0)
							{
								if(d <= hopo_threshold)
								{
									/* previous note wasn't part of a chord */
									if(!chord[difficulty])
									{
										/* previous note doesn't have the same value as this note */
										if(sp->track[i][difficulty].note_count && sp->track[i][difficulty].note[sp->track[i][difficulty].note_count - 1]->val != sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->val)
										{
											sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->hopo = true;
										}
									}
								}
								chord[difficulty] = false;
							}
							else if(hopo_forced == 0)
							{
								sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->hopo = false;

								/* make sure previous note is not marked HOPO */
								if(sp->track[i][difficulty].note_count > 0)
								{
									sp->track[i][difficulty].note[sp->track[i][difficulty].note_count - 1]->hopo = false;
								}
								chord[difficulty] = true;
							}
						}
						/* override autodetected HOPO status if forced HOPO detected */
						if(hopo_forced == 1)
						{
							sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->hopo = true;
						}
						else if(hopo_forced == 2)
						{
							sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->hopo = false;
						}
						sp->track[i][difficulty].note_count++;
					}
					previous_note_tick[difficulty] = sp->source_midi->track[i]->event[j]->tick;
				}
			}
		}
	}
	return true;
}

bool lss_song_mark_beats(LSS_SONG * sp, double total_length)
{
	int i;
	int current_beat_event = 0;
	double BPM = 120.0;
	double current_time = 0.0, previous_time = 0.0;
	double beat_time;
	int current_beat = 0;
	double current_z = (((sp->offset) * 60.0) * (double)LSS_SONG_PLACEMENT_SCALE) * (BPM / 120.0);;

	/* count beats */
	sp->beats = 0;
	beat_time = 60.0 / BPM;
	if(sp->source_midi->tempo_events)
	{
		while(current_time < total_length)
		{
			if(current_beat_event < sp->source_midi->tempo_events)
			{
				/* get new BPM if we are sitting on a tempo change */
				if(current_time >= sp->source_midi->tempo_event[current_beat_event]->pos_sec - 0.05)
				{
					current_time = sp->source_midi->tempo_event[current_beat_event]->pos_sec;
					current_beat_event++;
					if(current_beat_event <= sp->source_midi->tempo_events)
					{
						BPM = rtk_ppqn_to_bpm(sp->source_midi->tempo_event[current_beat_event - 1]->data_i[0]);
						beat_time = 60.0 / BPM;
					}
				}
			}
			sp->beats++;
			current_time += beat_time;
		}
	}
	sp->beats += 8;

	/* allocate beats */
	sp->beat = malloc(sizeof(LSS_SONG_BEAT *) * sp->beats);
	if(!sp->beat)
	{
		return false;
	}
	for(i = 0; i < sp->beats; i++)
	{
		sp->beat[i] = malloc(sizeof(LSS_SONG_BEAT));
		if(!sp->beat[i])
		{
			return false;
		}
	}

	/* initialize beats */
	BPM = 120.0;
	beat_time = 60.0 / BPM;
	current_beat_event = 0;
	current_time = 0.0;
	previous_time = 0.0;
	current_beat = 8;
	if(sp->source_midi->tempo_events)
	{
		while(current_time < total_length)
		{
			/* get new BPM if we are sitting on a tempo change */
			if(current_beat_event < sp->source_midi->tempo_events)
			{
				if(current_time >= sp->source_midi->tempo_event[current_beat_event]->pos_sec - 0.05)
				{
					previous_time = current_time;
					current_time = sp->source_midi->tempo_event[current_beat_event]->pos_sec;
					current_beat_event++;
					if(current_beat_event <= sp->source_midi->tempo_events)
					{
						BPM = rtk_ppqn_to_bpm(sp->source_midi->tempo_event[current_beat_event - 1]->data_i[0]);
						beat_time = 60.0 / BPM;
						current_z += (((current_time - previous_time) * 60.0) * (double)LSS_SONG_PLACEMENT_SCALE) * (BPM / 120.0);
					}
				}
			}
			if(current_beat < sp->beats)
			{
				sp->beat[current_beat]->tick = (current_time + sp->offset) * 60.0;
				sp->beat[current_beat]->z = current_z;
				sp->beat[current_beat]->BPM = BPM;
			}
			/* fill in pre-audio beats */
			if(current_beat == 8)
			{
				for(i = 0; i < 8; i++)
				{
					sp->beat[7 - i]->tick = (current_time + sp->offset - (beat_time * (double)(i + 1))) * 60.0;
					sp->beat[7 - i]->z = sp->beat[7 - i]->tick * LSS_SONG_PLACEMENT_SCALE;
					sp->beat[7 - i]->BPM = sp->beat[8]->BPM;
				}
			}
			current_beat++;
			previous_time = current_time;
			current_time += beat_time;
			current_z += (((current_time - previous_time) * 60.0) * (double)LSS_SONG_PLACEMENT_SCALE) * (BPM / 120.0);
		}
	}
	return true;
}

LSS_SONG * lss_load_song(ALLEGRO_PATH * pp)
{
	ALLEGRO_PATH * pcp;
	const char * val;
	LSS_SONG * sp;

	sp = malloc(sizeof(LSS_SONG));
	if(!sp)
	{
		return NULL;
	}
	memset(sp, 0, sizeof(LSS_SONG));

	pcp = al_clone_path(pp);
	if(!pcp)
	{
		free(sp);
		return NULL;
	}

	/* load source MIDI */
	al_set_path_filename(pcp, "notes.mid");
	sp->source_midi = rtk_load_midi(al_path_cstr(pcp, '/'));
	if(!sp->source_midi)
	{
		al_destroy_path(pcp);
		free(sp);
		return NULL;
	}

	/* load song tags file */
	al_set_path_filename(pcp, "song.ini");
	sp->tags = al_load_config_file(al_path_cstr(pcp, '/'));
	if(!sp->tags)
	{
		return NULL;
	}

	/* load relevant tags */
	val = al_get_config_value(sp->tags, "song", "delay");
	if(val)
	{
		sp->offset = (double)atoi(val) / 1000.0;
	}

	if(!lss_song_allocate_notes(sp))
	{
		return NULL;
	}

	if(!lss_song_populate_tracks(sp))
	{
		return NULL;
	}

	/* clean up */
	al_destroy_path(pcp);

	return sp;
}

void lss_destroy_song(LSS_SONG * sp)
{
	int i, j, k;

	for(i = 0; i < LSS_SONG_MAX_TRACKS; i++)
	{
		for(j = 0; j < LSS_SONG_MAX_DIFFICULTIES; j++)
		{
			if(sp->track[i][j].notes)
			{
				for(k = 0; k < sp->track[i][j].notes; k++)
				{
					free(sp->track[i][j].note[k]);
				}
				free(sp->track[i][j].note);
			}
		}
	}
	for(i = 0; i < sp->beats; i++)
	{
		free(sp->beat[i]);
	}
	free(sp->beat);
	free(sp);
}

void lss_song_hide_prior_notes(LSS_SONG * sp, int track, int difficulty, int note)
{
	int i;

	for(i = 0; i < note; i++)
	{
		sp->track[track][difficulty].note[i]->hidden = true;
	}
}
