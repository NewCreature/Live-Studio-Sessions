#ifndef LSS_PROFILE_H
#define LSS_PROFILE_H

#define LSS_MAX_PROFILES 32

typedef struct
{

	ALLEGRO_CONFIG * config;
	
	/* cached data */
	const char * name;

} LSS_PROFILE;

typedef struct
{

	LSS_PROFILE entry[LSS_MAX_PROFILES];
	int entries;

} LSS_PROFILES;

LSS_PROFILES * lss_load_profiles(void);
bool lss_save_profiles(LSS_PROFILES * pp);
void lss_destroy_profiles(LSS_PROFILES * pp);

#endif
