#include <ctype.h>

#include "t3f/t3f.h"
#include "t3f/debug.h"
#include "song_list.h"

LSS_SONG_LIST * lss_create_song_list(const char * fn, int entries, int collections)
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
	dp->collection = malloc(collections * sizeof(LSS_SONG_COLLECTION *));
	if(!dp->collection)
	{
		free(dp);
		return NULL;
	}
	lss_song_list_add_collection(dp, NULL, 0);
	return dp;
}

void lss_destroy_song_list(LSS_SONG_LIST * dp)
{
	int i;

	if(dp)
	{
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
		for(i = 0; i < dp->collections; i++)
		{
			free(dp->collection[i]);
		}
		free(dp->collection);
		free(dp);
	}
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

unsigned long lss_song_list_count_collections(const char * location, int flags)
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
			lss_song_list_count_collections(al_path_cstr(pp, '/'), flags | LSS_SONG_LIST_FLAG_RECURSE);
		}
		else
		{
			if(compare_filename(pp, "collection.ini"))
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

static int lss_song_list_encode_character(int c)
{
	c = tolower(c);
	if((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
	{
		return c;
	}
	return '_';
}

static char * lss_song_list_encode_artist_title(char * out, char * artist, char * title)
{
	int i;

	strcpy(out, "______");
	for(i = 0; i < 3 && i < strlen(artist); i++)
	{
		out[i] = lss_song_list_encode_character(artist[i]);
	}
	for(i = 0; i < 3 && i < strlen(title); i++)
	{
		out[i + 3] = lss_song_list_encode_character(title[i]);
	}
	return out;
}

/* determine if song at path sp is in collection cp */
static bool lss_song_list_file_in_collection(const ALLEGRO_PATH * cp, const ALLEGRO_PATH * sp)
{
	int i;
	const char * cpp;
	const char * spp;

	cpp = al_path_cstr(cp, '/');
	spp = al_path_cstr(sp, '/');

	for(i = 0; i < strlen(cpp); i++)
	{
		if(i >= strlen(spp))
		{
			return false;
		}
		if(spp[i] != cpp[i])
		{
			return false;
		}
	}
	return true;
}

void lss_song_list_add_file(LSS_SONG_LIST * dp, const ALLEGRO_PATH * pp, int flags)
{
	const char * val;
	char val2[128] = {0};
	char buf[32] = {0};
	ALLEGRO_CONFIG * cp;
	ALLEGRO_PATH * midi_path;

	cp = al_load_config_file(al_path_cstr(pp, '/'));
	if(!cp)
	{
		return;
	}
//	printf("Adding song: %s\n", al_path_cstr(pp, '/'));
	dp->entry[dp->entries] = malloc(sizeof(LSS_SONG_LIST_ENTRY));
	if(dp->entry[dp->entries])
	{
//		memset(&dp->entry[dp->entries], 0, sizeof(LSS_SONG_LIST_ENTRY));
		dp->entry[dp->entries]->path = al_clone_path(pp);
//		dp->entry[dp->entries]->extra = NULL;
		if(!(flags & LSS_SONG_LIST_FLAG_NO_CHECKSUM))
		{
			if(dp->cache)
			{
				/* cache pertinent song info so we don't have to extract it from
				 * the config file every time */
				strcpy(dp->entry[dp->entries]->artist, "");
				strcpy(dp->entry[dp->entries]->title, "");
				strcpy(dp->entry[dp->entries]->frets, "");
				val = al_get_config_value(cp, "song", "artist");
				if(val)
				{
					strcpy(dp->entry[dp->entries]->artist, val);
				}
				val = al_get_config_value(cp, "song", "name");
				if(val)
				{
					strcpy(dp->entry[dp->entries]->title, val);
				}
				val = al_get_config_value(cp, "song", "frets");
				if(val)
				{
					strcpy(dp->entry[dp->entries]->frets, val);
				}
				val = al_get_config_value(cp, "song", "tier");
				if(val)
				{
					dp->entry[dp->entries]->tier = atoi(val);
				}
				else
				{
					dp->entry[dp->entries]->tier = 0;
				}
				val = al_get_config_value(cp, "song", "sort");
				if(val)
				{
					dp->entry[dp->entries]->sort = atoi(val);
				}
				else
				{
					dp->entry[dp->entries]->sort = 0;
				}

				val = al_get_config_value(dp->cache, al_path_cstr(pp, '/'), "checksum");
				if(!val)
				{
					/* get checksum from notes.mid file */
					midi_path = al_clone_path(pp);
					if(midi_path)
					{
						al_set_path_filename(midi_path, "notes.mid");
						dp->entry[dp->entries]->checksum = t3f_checksum_file(al_path_cstr(midi_path, '/'));
						al_destroy_path(midi_path);
					}

					/* create ID from checksum and song info */
					sprintf(dp->entry[dp->entries]->id, "%s%lu", lss_song_list_encode_artist_title(buf, dp->entry[dp->entries]->artist, dp->entry[dp->entries]->title), dp->entry[dp->entries]->checksum);
					sprintf(val2, "%lu", dp->entry[dp->entries]->checksum);
					al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "checksum", val2);

					/* store path of song under ID so we can reference songs by ID later */
					al_set_config_value(dp->cache, dp->entry[dp->entries]->id, "Path", al_path_cstr(pp, '/'));
//					printf("checksum: %s\n", val2);
				}
				else
				{
					dp->entry[dp->entries]->checksum = atoi(val);
					val = al_get_config_value(dp->cache, al_path_cstr(pp, '/'), "id");
					if(val)
					{
						strcpy(dp->entry[dp->entries]->id, val);
					}
					else
					{
						sprintf(dp->entry[dp->entries]->id, "%s%lu", lss_song_list_encode_artist_title(buf, dp->entry[dp->entries]->artist, dp->entry[dp->entries]->title), dp->entry[dp->entries]->checksum);
					}
				}

				/* store info from song.ini in the song list cache for easy
				 * access */
				al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "Artist", dp->entry[dp->entries]->artist);
				al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "Title", dp->entry[dp->entries]->title);
				al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "Frets", dp->entry[dp->entries]->frets);
				sprintf(val2, "%lu", dp->entry[dp->entries]->checksum);
				al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "checksum", val2);
				al_set_config_value(dp->cache, al_path_cstr(pp, '/'), "id", dp->entry[dp->entries]->id);
			}
			else
			{
				dp->entry[dp->entries]->checksum = t3f_checksum_file(al_path_cstr(dp->entry[dp->entries]->path, '/'));
			}
		}
		dp->entries++;
	}
	al_destroy_config(cp);
}

void lss_song_list_add_collection(LSS_SONG_LIST * dp, const ALLEGRO_PATH * pp, int flags)
{
	const char * val;
	ALLEGRO_CONFIG * cp = NULL;

	if(pp)
	{
		cp = al_load_config_file(al_path_cstr(pp, '/'));
	}
	dp->collection[dp->collections] = malloc(sizeof(LSS_SONG_COLLECTION));
	if(dp->collection[dp->collections])
	{
		memset(dp->collection[dp->collections], 0, sizeof(LSS_SONG_COLLECTION));
		if(pp)
		{
			dp->collection[dp->collections]->path = al_clone_path(pp);
			al_set_path_filename(dp->collection[dp->collections]->path, NULL);
		}
		if(cp)
		{
			val = al_get_config_value(cp, "collection", "name");
			if(val)
			{
				strcpy(dp->collection[dp->collections]->name, val);
			}
		}
		else
		{
			strcpy(dp->collection[dp->collections]->name, "No Collection");
		}
		dp->collections++;
	}
	if(cp)
	{
		al_destroy_config(cp);
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
			if(compare_filename(pp, "collection.ini"))
			{
				lss_song_list_add_collection(dp, pp, flags);
/*				if(pp2_database_callback)
				{
					pp2_database_callback(pp);
				} */
//				printf("Found collection: %s\n", al_path_cstr(pp, '/'));
			}
		}
		al_destroy_path(pp);
		al_destroy_fs_entry(fp);
	}
	al_destroy_fs_entry(dir);
}

void lss_song_list_collect_files(LSS_SONG_LIST * dp)
{
	int i, j;

	/* attach songs to collections */
	if(dp->collections > 0)
	{
		for(i = 0; i < dp->entries; i++)
		{
			/* see if this song is part of a collection */
			dp->entry[i]->collection = 0;
			for(j = 0; j < dp->collections; j++)
			{
				if(dp->collection[j]->path)
				{
					if(lss_song_list_file_in_collection(dp->collection[j]->path, dp->entry[i]->path))
					{
						dp->entry[i]->collection = j;
					}
				}
			}
		}
	}
}

static const char * lss_song_list_filter = NULL;
static int lss_song_list_filter_field = 0;

static int lss_song_list_stricmp(const char * s1, const char * s2)
{
	int pos = 0;

	while(1)
	{
		if(tolower(s1[pos]) != tolower(s2[pos]))
		{
			if(s1[pos] == '\0')
			{
				return -1;
			}
			else if(s2[pos] == '\0')
			{
				return 1;
			}
			else
			{
				return tolower(s1[pos]) - tolower(s2[pos]);
			}
		}
		else
		{
			break;
		}
		pos++;
	}
	return 0;
}

static int lss_song_list_strmatch(const char * s1, const char * s2)
{
	int pos = 0;

	while(1)
	{
		if(tolower(s1[pos]) != tolower(s2[pos]))
		{
			if(s1[pos] == '\0')
			{
				return -1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if(s1[pos] == '\0')
			{
				return -1;
			}
		}
		pos++;
	}
	return 0;
}

static int lss_song_list_sorter(const void * item_1, const void * item_2)
{
	LSS_SONG_LIST_ENTRY * sp1 = *(LSS_SONG_LIST_ENTRY **)(item_1);
	LSS_SONG_LIST_ENTRY * sp2 = *(LSS_SONG_LIST_ENTRY **)(item_2);
	int acmp, acmp2;

	if(lss_song_list_filter)
	{
		if(lss_song_list_filter_field == 0)
		{
			acmp = lss_song_list_strmatch(lss_song_list_filter, sp1->artist);
			acmp2 = lss_song_list_strmatch(lss_song_list_filter, sp2->artist);
			if(acmp && acmp2)
			{
				acmp = lss_song_list_stricmp(sp1->artist, sp2->artist);
				if(!acmp)
				{
					return lss_song_list_stricmp(sp1->title, sp2->title);
				}
			}
			else if(acmp)
			{
				return -1;
			}
			else if(acmp2)
			{
				return 1;
			}
		}
		else if(lss_song_list_filter_field == 1)
		{
			acmp = lss_song_list_strmatch(lss_song_list_filter, sp1->title);
			acmp2 = lss_song_list_strmatch(lss_song_list_filter, sp2->title);
			if(acmp && acmp2)
			{
				acmp = lss_song_list_stricmp(sp1->artist, sp2->title);
				if(!acmp)
				{
					return lss_song_list_stricmp(sp1->artist, sp2->artist);
				}
			}
			else if(acmp)
			{
				return -1;
			}
			else if(acmp2)
			{
				return 1;
			}
		}
	}
	if(lss_song_list_filter_field == 0)
	{
		acmp = lss_song_list_stricmp(sp1->artist, sp2->artist);
		if(!acmp)
		{
			return lss_song_list_stricmp(sp1->title, sp2->title);
		}
	}
	else
	{
		acmp = lss_song_list_stricmp(sp1->title, sp2->title);
		if(!acmp)
		{
			return lss_song_list_stricmp(sp1->artist, sp2->artist);
		}
	}
	return acmp;
}

/* sort the sprite list using the above helper function */
void lss_song_list_sort(LSS_SONG_LIST * dp, int field, const char * filter)
{
	t3f_debug_message("lss_song_list_sort() enter\n");
	lss_song_list_filter_field = field;
	lss_song_list_filter = filter;
	qsort(dp->entry, dp->entries, sizeof(LSS_SONG_LIST_ENTRY *), lss_song_list_sorter);
	dp->visible_entries = dp->entries;
	if(filter)
	{
		dp->visible_entries = 0;
		if(field == 0)
		{
			while(lss_song_list_strmatch(lss_song_list_filter, dp->entry[dp->visible_entries]->artist) && dp->visible_entries < dp->entries - 1)
			{
				dp->visible_entries++;
			}
		}
		else if(field == 1)
		{
			while(lss_song_list_strmatch(lss_song_list_filter, dp->entry[dp->visible_entries]->title) && dp->visible_entries < dp->entries - 1)
			{
				dp->visible_entries++;
			}
		}
	}
	t3f_debug_message("lss_song_list_sort() exit\n");
}

static int lss_song_list_collection_sort_collection;

static int lss_song_list_collection_sorter(const void * item_1, const void * item_2)
{
	LSS_SONG_LIST_ENTRY * sp1 = *(LSS_SONG_LIST_ENTRY **)(item_1);
	LSS_SONG_LIST_ENTRY * sp2 = *(LSS_SONG_LIST_ENTRY **)(item_2);

	if(sp1->collection == lss_song_list_collection_sort_collection && sp2->collection == lss_song_list_collection_sort_collection)
	{
//		printf("%d, %d - %d, %d\n", sp1->tier, sp1->sort, sp2->tier, sp2->sort);
		if(sp1->tier > 0 && sp2->tier > 0)
		{
			if(sp1->sort > 0 && sp2->sort > 0)
			{
				return ((sp1->tier * 100) + sp1->sort) - ((sp2->tier * 100) + sp2->sort);
			}
			else if(sp1->tier != sp2->tier)
			{
				return sp1->tier - sp2->tier;
			}
		}
		else if(sp1->tier > 0)
		{
			return -1;
		}
		else if(sp2->tier > 0)
		{
			return 1;
		}
		return lss_song_list_sorter(item_1, item_2);
	}
	else if(sp1->collection == lss_song_list_collection_sort_collection)
	{
		return -1;
	}
	else if(sp2->collection == lss_song_list_collection_sort_collection)
	{
		return 1;
	}
	return 1;
/*	if(sp1->collection == lss_song_list_collection_sort_collection && sp2->collection == lss_song_list_collection_sort_collection)
	{
		return 0;
	}
	else if(sp1->collection == lss_song_list_collection_sort_collection && sp2->collection != lss_song_list_collection_sort_collection)
	{
		return -1;
	}
	else if(sp1->collection != lss_song_list_collection_sort_collection && sp2->collection == lss_song_list_collection_sort_collection)
	{
		return 1;
	} */
	return -1;
}

void lss_song_list_sort_collection(LSS_SONG_LIST * dp, int collection)
{
	lss_song_list_collection_sort_collection = collection;
	qsort(dp->entry, dp->entries, sizeof(LSS_SONG_LIST_ENTRY *), lss_song_list_collection_sorter);
	dp->visible_entries = 0;
	while(dp->entry[dp->visible_entries]->collection == collection && dp->visible_entries < dp->entries - 1)
	{
		dp->visible_entries++;
	}
}
