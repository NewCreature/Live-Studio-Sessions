#ifndef LSS_SONG_LIST_H
#define LSS_SONG_LIST_H

#define LSS_SONG_LIST_FLAG_NO_CHECKSUM 1
#define LSS_SONG_LIST_FLAG_RECURSE     2

typedef struct
{

	/* file info */
	ALLEGRO_PATH * path;
	unsigned long checksum;
	char id[256]; // unique identifier used for leaderboards and such
	
	/* cached song info */
	char artist[128];
	char title[256];
	char frets[128];

} LSS_SONG_LIST_ENTRY;

typedef struct
{

	ALLEGRO_CONFIG * cache;
	char cache_filename[1024];
	LSS_SONG_LIST_ENTRY ** entry;
	int entries;

} LSS_SONG_LIST;

LSS_SONG_LIST * lss_create_song_list(const char * fn, int entries);
void lss_destroy_song_list(LSS_SONG_LIST * dp);
unsigned long lss_song_list_count_files(const char * location, int flags);
void lss_song_list_add_files(LSS_SONG_LIST * dp, const ALLEGRO_PATH * path, int flags);

void lss_song_list_sort(LSS_SONG_LIST * dp, int field, const char * filter);

#endif
