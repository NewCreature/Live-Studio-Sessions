#include "t3f/t3f.h"

#include "profile.h"

LSS_PROFILES * lss_load_profiles(void)
{
	LSS_PROFILES * pp;
	char fn[1024];
	int i;
	
	pp = malloc(sizeof(LSS_PROFILES));
	if(pp)
	{
		memset(pp, 0, sizeof(LSS_PROFILES));
		for(i = 0; i < LSS_MAX_PROFILES; i++)
		{
			sprintf(fn, "%s%d.dat", t3f_get_filename(t3f_data_path, "profile"), i);
			pp->entry[pp->entries].config = al_load_config_file(fn);
			if(pp->entry[pp->entries].config)
			{
				pp->entry[pp->entries].name = al_get_config_value(pp->entry[pp->entries].config, "Settings", "Name");
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
	return pp;
}

bool lss_save_profiles(LSS_PROFILES * pp)
{
	char fn[1024];
	int i;

	for(i = 0; i < pp->entries; i++)
	{
		/* save config file */
		sprintf(fn, "%s%d.dat", t3f_get_filename(t3f_data_path, "profile"), i);
		al_save_config_file(fn, pp->entry[i].config);
	}
	return true;
}

void lss_destroy_profiles(LSS_PROFILES * pp)
{
	free(pp);
}
