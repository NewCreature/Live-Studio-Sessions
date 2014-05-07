#include "t3f/t3f.h"
#include "song_list.h"

LSS_SONG_LIST * lss_create_song_list(const char * fn, int entries)
{
	LSS_SONG_LIST * dp;
	
	dp = malloc(sizeof(LSS_SONG_LIST));
	if(!dp)
	{
		return NULL;
	}
	memset(dp, 0, sizeof(LSS_SONG_LIST));
	dp->entry = malloc(entries * sizeof(LSS_SONG_LIST_ENTRY *));
	if(!dp->entry)
	{
		free(dp);
		return NULL;
	}
	if(fn)
	{
		strcpy(dp->cache_filename, fn);
		dp->cache = al_load_config_file(fn);
		if(!dp->cache)
		{
			dp->cache = al_create_config();
		}
		if(!dp->cache)
		{
			return NULL;
		}
	}
	else
	{
		dp->cache = NULL;
	}
	return dp;
}

void lss_destroy_song_list(LSS_SONG_LIST * dp)
{
	int i;

	if(dp->cache)
	{
		al_save_config_file(dp->cache_filename, dp->cache);
		al_destroy_config(dp->cache);
	}
	for(i = 0; i < dp->entries; i++)
	{
		free(dp->entry[i]);
	}
	free(dp->entry);
	free(dp);
}

static bool compare_filename(const ALLEGRO_PATH * path, const char * fn)
{
//	int i;
	const char * cfn = NULL;
	
	cfn = al_get_path_filename(path);
	if(!strcmp(cfn, fn))
	{
		return true;
	}
	return false;
}

static unsigned long lss_song_list_file_count = 0;

unsigned long lss_song_list_count_files(const char * location, int flags)
{
	ALLEGRO_FS_ENTRY * dir;
	ALLEGRO_FS_ENTRY * fp;
	ALLEGRO_PATH * pp;
	ALLEGRO_PATH * path;
	const char * name;
	char cname[1024] = {0};
	
	/* reset counter if this is the first time entering this function */
	if(!(flags & LSS_SONG_LIST_FLAG_RECURSE))
	{
		lss_song_list_file_count = 0;
	}
	
	path = al_create_path(location);
	name = al_path_cstr(path, '/');
	strcpy(cname, name);
	if(cname[strlen(cname) - 1] == '/')
	{
		if(flags & LSS_SONG_LIST_FLAG_RECURSE)
		{
			if(cname[strlen(cname) - 2] == '.')
			{
				return 0;
			}
		}
		cname[strlen(cname) - 1] = 0;
	}
	
	dir = al_create_fs_entry(cname);
	if(!dir)
	{
		return 0;
	}
	if(!al_open_directory(dir))
	{
		return 0;
	}
	while(1)
	{
		fp = al_read_directory(dir);
		if(!fp)
		{
			break;
		}
		pp = al_create_path(al_get_fs_entry_name(fp));
		if(al_get_fs_entry_mode(fp) & ALLEGRO_FILEMODE_ISDIR)
		{
			lss_song_list_count_files(al_path_cstr(pp, '/'), flags | LSS_SONG_LIST_FLAG_RECURSE);
		}
		else
		{
			if(compare_filename(pp, "song.ini"))
			{
				lss_song_list_file_count++;
			}
		}
		al_destroy_path(pp);
		al_destroy_fs_entry(fp);
	}
	al_destroy_fs_entry(dir);
	al_destroy_path(path);
	return lss_song_list_file_count;
}

void lss_song_list_add_file(LSS_SONG_LIST * dp, const ALLEGRO_PATH * pp, int flags)
{
	const char * val;
	char val2[128] = {0};

//	printf("Adding song: %s\n", al_path_cstr(pp, '/'));
	dp->entry[dp->entries] = malloc(sizeof(LSS_SONG_LIST_ENTRY));
	if(dp->entry[dp->entries])
	{
		dp->entry[dp->entries]->path = al_clone_path(pp);
//		dp->entry[dp->entries]->extra = NULL;
		if(!(flags & LSS_SONG_LIST_FLAG_NO_CHECKSUM))
		{
			if(dp->cache)
			{
				val = al_get_config_value(dp->cache, al_path_cstr(pp, '/'), "checksum");
				if(!val)
				{
					dp->entry[dp->entries]->checksum = t3f_checksum_file(al_path_cstr(dp->entry[dp->entries]->path, '/'));
					sprintf(val2, "%lu", dp->entry[dp->entries]->checksum);
					al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "checksum", val2);
				}
				else
				{
					dp->entry[dp->entries]->checksum = atoi(val);
				}
			}
			else
			{
				dp->entry[dp->entries]->checksum = t3f_checksum_file(al_path_cstr(dp->entry[dp->entries]->path, '/'));
			}
		}
		dp->entries++;
	}
}

void lss_song_list_add_files(LSS_SONG_LIST * dp, const ALLEGRO_PATH * path, int flags)
{
	ALLEGRO_FS_ENTRY * dir;
	ALLEGRO_FS_ENTRY * fp;
	ALLEGRO_PATH * pp;
	const char * name;
	char cname[1024] = {0};
	
	/* ignore ./ and ../ path entries */
	name = al_path_cstr(path, '/');
	strcpy(cname, name);
	if(cname[strlen(cname) - 1] == '/')
	{
		if(flags & LSS_SONG_LIST_FLAG_RECURSE)
		{
			if(cname[strlen(cname) - 2] == '.')
			{
				return;
			}
		}
		cname[strlen(cname) - 1] = 0;
	}
	
//	printf("!Looking in %s\n", cname);
	dir = al_create_fs_entry(cname);
	if(!dir)
	{
		return;
	}
	if(!al_open_directory(dir))
	{
		return;
	}
//	printf("Looking in %s\n", cname);
	while(1)
	{
		fp = al_read_directory(dir);
		if(!fp)
		{
			break;
		}
		pp = al_create_path(al_get_fs_entry_name(fp));
//		name = al_path_to_string(al_get_entry_name(fp), '/');
		if(al_get_fs_entry_mode(fp) & ALLEGRO_FILEMODE_ISDIR)
		{
			lss_song_list_add_files(dp, pp, flags | LSS_SONG_LIST_FLAG_RECURSE);
		}
		else
		{
			if(compare_filename(pp, "song.ini"))
			{
				lss_song_list_add_file(dp, pp, flags);
/*				if(pp2_database_callback)
				{
					pp2_database_callback(pp);
				} */
//				printf("%s\n", al_path_to_string(pp, '/'));
			}
		}
		al_destroy_path(pp);
		al_destroy_fs_entry(fp);
	}
	al_destroy_fs_entry(dir);
}
