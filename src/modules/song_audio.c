#include "t3f/t3f.h"
#include "t3f/debug.h"

#include "song_audio.h"

LSS_SONG_AUDIO * lss_load_song_audio(ALLEGRO_PATH * pp)
{
	ALLEGRO_PATH * pcp;
	LSS_SONG_AUDIO * ap;
	double length;
	int i;

	ap = malloc(sizeof(LSS_SONG_AUDIO));
	if(!ap)
	{
		return NULL;
	}
	memset(ap, 0, sizeof(LSS_SONG_AUDIO));

	pcp = al_clone_path(pp);
	if(!pcp)
	{
		free(ap);
		return NULL;
	}

	/* load all available audio streams */
	al_set_path_filename(pcp, "song.ogg");
	ap->stream[0] = al_load_audio_stream(al_path_cstr(pcp, '/'), 4, 1024);
	al_set_path_filename(pcp, "guitar.ogg");
	ap->stream[1] = al_load_audio_stream(al_path_cstr(pcp, '/'), 4, 1024);
	al_set_path_filename(pcp, "rhythm.ogg");
	ap->stream[2] = al_load_audio_stream(al_path_cstr(pcp, '/'), 4, 1024);
	al_set_path_filename(pcp, "drums.ogg");
	ap->stream[3] = al_load_audio_stream(al_path_cstr(pcp, '/'), 4, 1024);

	/* count streams */
	ap->streams = 0;
	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			ap->streams++;
		}
	}

	/* get stream length */
	ap->length = 0.0;
	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			al_set_audio_stream_playing(ap->stream[i], false);
			length = al_get_audio_stream_length_secs(ap->stream[i]);
			if(length > ap->length)
			{
				ap->length = length;
			}
		}
	}

	ap->playing = false;

	al_destroy_path(pcp);
	return ap;
}

void lss_destroy_song_audio(LSS_SONG_AUDIO * ap)
{
	int i;

	if(ap->playing)
	{
		lss_set_song_audio_playing(ap, false);
	}
	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			al_destroy_audio_stream(ap->stream[i]);
		}
	}
	free(ap);
}

static volatile int lss_song_audio_callback_counter = 0;

static void lss_song_audio_callback(void * buf, unsigned int samples, void * data)
{
	lss_song_audio_callback_counter++;
}

bool lss_set_song_audio_playing(LSS_SONG_AUDIO * ap, bool playing)
{
	int i;

	t3f_debug_message("lss_set_song_audio_playing() enter\n");
	if(playing && !ap->playing)
	{
		/* create voice for audio to be played back through */
/*		ap->voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
		if(!ap->voice)
		{
			return false;
		}
		if(!al_set_voice_playing(ap->voice, false))
		{
			printf("failed to stop voice\n");
		} */

		/* create mixer into which all streams will be mixed before passing to the voice */
/*		ap->mixer = al_create_mixer(al_get_voice_frequency(ap->voice), ALLEGRO_AUDIO_DEPTH_FLOAT32, al_get_voice_channels(ap->voice));
		if(!ap->mixer)
		{
			al_destroy_voice(ap->voice);
			return false;
		} */

		/* set a callback for the mixer so we can start the audio at the exact
		 * time we want */
		t3f_debug_message("\tSetting mixer callback...\n");
		if(!al_set_mixer_postprocess_callback(al_get_default_mixer(), lss_song_audio_callback, NULL))
		{
			t3f_debug_message("\tFailed to set mixer callback!\n");
		}

		/* attach mixer to voice, must be done before starting stream playback
		 * or it causes crashes on Mac (look into this later) */
/*		if(!al_attach_mixer_to_voice(ap->mixer, ap->voice))
		{
			printf("failed to attach mixer to voice\n");
			return false;
		} */

		/* start voice if it isn't already playing */
/*		if(!al_get_voice_playing(ap->voice))
		{
			if(!al_set_voice_playing(ap->voice, true))
			{
				printf("failed to start voice playback\n");
				return false;
			}
		} */
		/* attach the streams to the mixer and set them to playing */
		t3f_debug_message("\tWaiting for callback counter...\n");
		lss_song_audio_callback_counter = 0;
		while(lss_song_audio_callback_counter == 0)
		{
		}
		t3f_debug_message("\tStarting audio streams...\n");
		for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
		{
			if(ap->stream[i])
			{
				t3f_debug_message("\t\tStart audio stream %d...\n", i);
				if(!al_attach_audio_stream_to_mixer(ap->stream[i], al_get_default_mixer()))
				{
					t3f_debug_message("\t\tFailed to attach stream %d to audio mixer\n", i);
					return false;
				}
				if(!al_set_audio_stream_playing(ap->stream[i], true))
				{
					t3f_debug_message("\t\tFailed to start playing audio stream %d\n", i);
					return false;
				}
			}
		}
		if(lss_song_audio_callback_counter > 1)
		{
			t3f_debug_message("\tAudio stream start took too long, waiting for callback counter...\n");
			lss_song_audio_callback_counter = 0;
			while(lss_song_audio_callback_counter == 0)
			{
			}
			t3f_debug_message("\tRewinding audio streams...\n");
			lss_set_song_audio_position(ap, 0.0);
		}

		ap->playing = true;
		t3f_debug_message("\tRemoving mixer callback...\n");
		al_set_mixer_postprocess_callback(al_get_default_mixer(), NULL, NULL);
	}
	else if(!playing && ap->playing)
	{
//		al_set_voice_playing(ap->voice, false);
		t3f_debug_message("\tStopping audio streams...\n");
		for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
		{
			if(ap->stream[i])
			{
				al_set_audio_stream_playing(ap->stream[i], false);
//				al_detach_audio_stream(ap->stream[i]); // don't detach until Allegro mutex bug is fixed
			}
		}
//		al_detach_mixer(ap->mixer);
//		al_destroy_mixer(ap->mixer);
//		al_destroy_voice(ap->voice);
		ap->playing = false;
	}
	t3f_debug_message("lss_set_song_audio_playing() exit\n");
	return true;
}

void lss_set_song_audio_loop(LSS_SONG_AUDIO * ap, double start, double end)
{
	int i;

	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			al_set_audio_stream_loop_secs(ap->stream[i], start, end);
		}
	}
}

void lss_set_song_audio_position(LSS_SONG_AUDIO * ap, double pos)
{
	int i;

	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			al_seek_audio_stream_secs(ap->stream[i], pos);
		}
	}
}
