#ifndef LSS_SONG_AUDIO_H
#define LSS_SONG_AUDIO_H

#include "t3f/t3f.h"
#include "waveform.h"

#define LSS_SONG_AUDIO_MAX_STREAMS 16

typedef struct
{

//	ALLEGRO_VOICE * voice;
//	ALLEGRO_MIXER * mixer;
	ALLEGRO_AUDIO_STREAM * stream[LSS_SONG_AUDIO_MAX_STREAMS];
	int streams;
	LSS_WAVEFORM * waveform;

	double length;
	bool playing;

	/* A/V synchronization */
	bool resync_video;

} LSS_SONG_AUDIO;

LSS_SONG_AUDIO * lss_load_song_audio(ALLEGRO_PATH * pp);
void lss_destroy_song_audio(LSS_SONG_AUDIO * ap);

bool lss_set_song_audio_playing(LSS_SONG_AUDIO * ap, bool playing);
void lss_set_song_audio_loop(LSS_SONG_AUDIO * ap, double start, double end);
void lss_set_song_audio_position(LSS_SONG_AUDIO * ap, double pos);
double lss_get_song_audio_length(LSS_SONG_AUDIO * ap);
double lss_get_song_audio_position(LSS_SONG_AUDIO * ap);

#endif
