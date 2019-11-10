#ifndef LSS_PROFILE_H
#define LSS_PROFILE_H

#define LSS_MAX_PROFILES             32
#define LSS_PROFILE_MAX_CAREERS      32
#define LSS_PROFILE_CAREER_MAX_TIERS 32
#define LSS_PROFILE_CAREER_MAX_SONGS 64

typedef struct
{

	bool song_passed[LSS_PROFILE_CAREER_MAX_SONGS];

} LSS_PROFILE_CAREER_TIER;

typedef struct
{

	const char * name;
	LSS_PROFILE_CAREER_TIER tier[LSS_PROFILE_CAREER_MAX_TIERS];
	int tiers;

} LSS_PROFILE_CAREER;

typedef struct
{

	ALLEGRO_CONFIG * config;

	/* cached data */
	const char * name;
	LSS_PROFILE_CAREER career[LSS_PROFILE_MAX_CAREERS];
	int careers;

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
