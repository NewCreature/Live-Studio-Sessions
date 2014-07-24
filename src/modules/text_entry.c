#include "t3f/t3f.h"

static ALLEGRO_USTR * lss_entered_text_ustr = NULL;
static char * lss_entered_text = NULL;
static int lss_entering_text_pos = 0;
static int lss_entering_text = 0;
static int lss_entering_text_max = 0;
static void * lss_entering_text_data = NULL;
static bool (*lss_entering_text_proc)(void * data, int c) = NULL;

bool lss_begin_text_entry(void * data, const char * initial, char * buffer, int max_length, bool (*proc)(void * data, int c))
{
	t3f_clear_keys();
	lss_entered_text_ustr = al_ustr_new(initial);
	if(!lss_entered_text_ustr)
	{
		return false;
	}
	lss_entered_text = buffer;
	lss_entering_text_max = max_length;
	lss_entering_text_data = data;
	lss_entering_text_proc = proc;
	al_ustr_to_buffer(lss_entered_text_ustr, lss_entered_text, max_length * 4);
	lss_entering_text = 1;
	return true;
}

void lss_end_text_entry(void)
{
	al_ustr_free(lss_entered_text_ustr);
	lss_entering_text = 0;
}

bool lss_process_text_entry(void)
{
	if(lss_entering_text)
	{
		int key = t3f_read_key(0);
		switch(key)
		{
			/* backspace */
			case '\b':
			case 127:
			{
				if(lss_entering_text_pos > 0)
				{
					al_ustr_remove_chr(lss_entered_text_ustr, lss_entering_text_pos - 1);
					lss_entering_text_pos--;
					al_ustr_to_buffer(lss_entered_text_ustr, lss_entered_text, 256);
				}
				break;
			}
			
			/* ignore these keys */
			case '\r':
			case '\t':
			case '\0':
			{
				break;
			}
			
			/* type these keys */
			default:
			{
				if(lss_entering_text_pos < lss_entering_text_max)
				{
					al_ustr_insert_chr(lss_entered_text_ustr, lss_entering_text_pos, key);
					lss_entering_text_pos++;
					al_ustr_to_buffer(lss_entered_text_ustr, lss_entered_text, 256);
				}
				break;
			}
		}
		if(lss_entering_text_proc)
		{
			lss_entering_text_proc(lss_entering_text_data, key);
		}
		return true;
	}
	return false;
}
