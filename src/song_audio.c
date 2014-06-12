#include "t3f/t3f.h"

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
	
	/* get stream length */
	ap->length = 0.0;
	for(i = 0; i < 4; i++)
	{
		if(ap->stream[i])
		{
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
	if(ap->playing)
	{
		lss_set_song_audio_playing(ap, false);
	}
	free(ap);
}

bool lss_set_song_audio_playing(LSS_SONG_AUDIO * ap, bool playing)
{
	int i;
	
	if(playing && !ap->playing)
	{
		/* create voice for audio to be played back through */
		ap->voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
		if(!ap->voice)
		{
			return false;
		}
		
		/* create mixer into which all streams will be mixed before passing to the voice */
		ap->mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
		if(!ap->mixer)
		{
			al_destroy_voice(ap->voice);
			return false;
		}
		
		/* attach the streams to the mixer and set them to playing */
		for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
		{
			if(ap->stream[i])
			{
				al_attach_audio_stream_to_mixer(ap->stream[i], ap->mixer);
				al_set_audio_stream_playing(ap->stream[i], true);
			}
		}
		
		/* attach mixer to voice and start audio playback */
		al_attach_mixer_to_voice(ap->mixer, ap->voice);
		al_set_voice_playing(ap->voice, true);
		ap->playing = true;
	}
	else if(!playing && ap->playing)
	{
		al_set_voice_playing(ap->voice, false);
		for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
		{
			if(ap->stream[i])
			{
				al_set_audio_stream_playing(ap->stream[i], false);
				al_detach_audio_stream(ap->stream[i]);
				al_destroy_audio_stream(ap->stream[i]);
			}
		}
		al_detach_mixer(ap->mixer);
		al_destroy_mixer(ap->mixer);
		al_destroy_voice(ap->voice);
		ap->playing = false;
	}
	return false;
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

	lss_set_song_audio_playing(ap, false);
	
	for(i = 0; i < LSS_SONG_AUDIO_MAX_STREAMS; i++)
	{
		if(ap->stream[i])
		{
			al_seek_audio_stream_secs(ap->stream[i], pos);
		}
	}
}
