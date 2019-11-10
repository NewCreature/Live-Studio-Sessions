#include "t3f/t3f.h"

#include "profile.h"

static bool lss_load_profile_career(LSS_PROFILE * pp, int career)
{
	char buf[1024];
	char buf2[1024];
	const char * val;
	int i, j;

	sprintf(buf, "Career_%d", career);
	pp->career[career].name = al_get_config_value(pp->config, buf, "Name");
	val = al_get_config_value(pp->config, buf, "Tiers");
	if(val)
	{
		pp->career[career].tiers = atoi(val);
	}
	for(i = 0; i < pp->career[career].tiers; i++)
	{
		for(j = 0; j < LSS_PROFILE_CAREER_MAX_SONGS; j++)
		{
			sprintf(buf2, "Tier_%d_Passed_%d", i, j);
			val = al_get_config_value(pp->config, buf, buf2);
			if(val)
			{
				pp->career[career].tier[i].song_passed[j] = atoi(val);
			}
		}
	}
	return true;
}

LSS_PROFILES * lss_load_profiles(void)
{
	const ALLEGRO_FILE_INTERFACE * old_interface;
	LSS_PROFILES * pp;
	char fn[1024];
	char buf[1024];
	const char * val;
	int i, j;

	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	pp = malloc(sizeof(LSS_PROFILES));
	if(pp)
	{
		memset(pp, 0, sizeof(LSS_PROFILES));
		for(i = 0; i < LSS_MAX_PROFILES; i++)
		{
			sprintf(fn, "%s%d.dat", t3f_get_filename(t3f_data_path, "profile", buf, 1024), i);
			pp->entry[pp->entries].config = al_load_config_file(fn);
			if(pp->entry[pp->entries].config)
			{
				pp->entry[pp->entries].name = al_get_config_value(pp->entry[pp->entries].config, "Settings", "Name");
				val = al_get_config_value(pp->entry[pp->entries].config, "Settings", "Careers");
				if(val)
				{
					pp->entry[pp->entries].careers = atoi(val);
				}
				for(j = 0; j < pp->entry[pp->entries].careers; j++)
				{
					lss_load_profile_career(&pp->entry[pp->entries], j);
				}
				pp->entries++;
			}
		}
		if(pp->entries < 1)
		{
			pp->entry[0].config = al_create_config();
			if(pp->entry[0].config)
			{
				al_set_config_value(pp->entry[0].config, "Settings", "Name", "Guest");
				pp->entry[0].name = al_get_config_value(pp->entry[0].config, "Settings", "Name");
				pp->entries = 1;
			}
		}
	}
	al_set_new_file_interface(old_interface);
	return pp;
}

static void lss_save_profile_career(LSS_PROFILE * pp, int career)
{
	char buf[1024];
	char buf2[1024];
	char buf3[1024];
	int i, j;

	sprintf(buf, "Career_%d", career);
	al_set_config_value(pp->config, buf, "Name", pp->career[career].name);
	sprintf(buf2, "%d", pp->career[career].tiers);
	al_set_config_value(pp->config, buf, "Tiers", buf2);
	for(i = 0; i < pp->career[career].tiers; i++)
	{
		for(j = 0; j < LSS_PROFILE_CAREER_MAX_SONGS; j++)
		{
			sprintf(buf2, "Tier_%d_Passed_%d", i, j);
			sprintf(buf3, "%d", pp->career[career].tier[i].song_passed[j]);
			al_set_config_value(pp->config, buf, buf2, buf3);
		}
	}
}

bool lss_save_profiles(LSS_PROFILES * pp)
{
	const ALLEGRO_FILE_INTERFACE * old_interface;
	char fn[1024];
	char buf[1024];
	int i, j;

	old_interface = al_get_new_file_interface();
	al_set_standard_file_interface();
	for(i = 0; i < pp->entries; i++)
	{
		sprintf(buf, "%d", pp->entry[i].careers);
		al_set_config_value(pp->entry[i].config, "Settings", "Careers", buf);
		for(j = 0; j < pp->entry[i].careers; j++)
		{
			lss_save_profile_career(&pp->entry[i], j);
		}
		/* save config file */
		sprintf(fn, "%s%d.dat", t3f_get_filename(t3f_data_path, "profile", buf, 1024), i);
		al_save_config_file(fn, pp->entry[i].config);
	}
	al_set_new_file_interface(old_interface);
	return true;
}

void lss_destroy_profiles(LSS_PROFILES * pp)
{
	free(pp);
}
