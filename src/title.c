#include "t3f/gui.h"

#include "instance.h"
#include "title.h"
#include "resources.h"
#include "state.h"

int lss_menu_proc_play(void * data, int i, void * p)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	
	app->state = LSS_STATE_SONG_SELECT;
	return 1;
}

int lss_menu_proc_options(void * data, int i, void * p)
{
	return 1;
}

int lss_menu_proc_quit(void * data, int i, void * p)
{
	t3f_exit();
	return 1;
}

bool lss_title_initialize(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp)
{
	t3f_set_gui_driver(NULL);
	memset(dp->menu, 0, sizeof(T3F_GUI *) * LSS_MAX_MENUS);
	
	/* main menu */
	dp->menu[LSS_MENU_MAIN] = t3f_create_gui(0, 0);
	if(!dp->menu[LSS_MENU_MAIN])
	{
		return false;
	}
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], NULL, "Live Studio Sessions", rp->font[LSS_FONT_LARGE], 8, 0, t3f_color_white, T3F_GUI_ELEMENT_STATIC | T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_play, "Play", rp->font[LSS_FONT_SMALL], 8, 48, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_options, "Options", rp->font[LSS_FONT_SMALL], 8, 72, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	t3f_add_gui_text_element(dp->menu[LSS_MENU_MAIN], lss_menu_proc_quit, "Quit", rp->font[LSS_FONT_SMALL], 8, 96, t3f_color_white, T3F_GUI_ELEMENT_SHADOW);
	
	return true;
}

void lss_title_exit(LSS_TITLE_DATA * dp)
{
	int i;
	
	for(i = 0; i < LSS_MAX_MENUS; i++)
	{
		if(dp->menu[i])
		{
			t3f_destroy_gui(dp->menu[i]);
			dp->menu[i] = NULL;
		}
	}
}

void lss_title_logic(LSS_TITLE_DATA * dp, APP_INSTANCE * app)
{
	if(dp->current_menu >= 0)
	{
		t3f_process_gui(dp->menu[dp->current_menu], app);
	}
}

void lss_title_render(LSS_TITLE_DATA * dp, LSS_RESOURCES * rp)
{
	if(dp->current_menu >= 0)
	{
		t3f_render_gui(dp->menu[dp->current_menu]);
	}
}
