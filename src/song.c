#include "song.h"

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
				difficulty = -1;
				if(sp->source_midi->track[i]->event[j]->data_i[0] >= 60 && sp->source_midi->track[i]->event[j]->data_i[0] < 66)
				{
					difficulty = 0;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 72 && sp->source_midi->track[i]->event[j]->data_i[0] < 78)
				{
					difficulty = 1;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 84 && sp->source_midi->track[i]->event[j]->data_i[0] < 90)
				{
					difficulty = 2;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 96 && sp->source_midi->track[i]->event[j]->data_i[0] < 102)
				{
					difficulty = 3;
				}
				if(difficulty >= 0)
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
				sp->track[i][j].note = malloc(sizeof(LSS_SONG_NOTE) * sp->track[i][j].notes);
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
	}
	return -1;
}

static bool lss_song_populate_tracks(LSS_SONG * sp)
{
	int i, j, d;
	int difficulty, difficulty_offset;
	int note_off_event;
	int previous_note_tick[16] = {-1, -1, -1, -1, -1, -1, -1, -1, -1};
	bool chord[16] = {false};
	int hopo_threshold;

	hopo_threshold = sp->source_midi->raw_data->divisions / 3; // 12th note
	for(i = 0; i < sp->source_midi->tracks; i++)
	{
		for(j = 0; j < sp->source_midi->track[i]->events; j++)
		{
			if(sp->source_midi->track[i]->event[j]->type == RTK_MIDI_EVENT_TYPE_NOTE_ON && sp->source_midi->track[i]->event[j]->data_i[1] != 0)
			{
				difficulty = -1;
				if(sp->source_midi->track[i]->event[j]->data_i[0] >= 60 && sp->source_midi->track[i]->event[j]->data_i[0] < 66)
				{
					difficulty = 0;
					difficulty_offset = 60;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 72 && sp->source_midi->track[i]->event[j]->data_i[0] < 78)
				{
					difficulty = 1;
					difficulty_offset = 72;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 84 && sp->source_midi->track[i]->event[j]->data_i[0] < 90)
				{
					difficulty = 2;
					difficulty_offset = 84;
				}
				else if(sp->source_midi->track[i]->event[j]->data_i[0] >= 96 && sp->source_midi->track[i]->event[j]->data_i[0] < 102)
				{
					difficulty = 3;
					difficulty_offset = 96;
				}
				if(difficulty >= 0)
				{
					note_off_event = lss_song_get_note_end_event(sp, i, j);
					if(note_off_event >= 0)
					{
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->val = sp->source_midi->track[i]->event[j]->data_i[0] - difficulty_offset;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick = (sp->source_midi->track[i]->event[j]->pos_sec + sp->offset) * 60.0;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->play_tick = sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->length = (sp->source_midi->track[i]->event[note_off_event]->pos_sec + sp->offset) * 60.0 - sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->tick;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->active = true;
						sp->track[i][difficulty].note[sp->track[i][difficulty].note_count]->visible = true;
						
						/* check for auto HOPO status (12th note or shorter) */
						if(previous_note_tick[difficulty] >= 0)
						{
							d = sp->source_midi->track[i]->event[j]->tick - previous_note_tick[difficulty];
							if(d > 0)
							{
								if(d < hopo_threshold)
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
							else
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
						sp->track[i][difficulty].note_count++;
					}
					previous_note_tick[difficulty] = sp->source_midi->track[i]->event[j]->tick;
				}
			}
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
	free(sp);
}
