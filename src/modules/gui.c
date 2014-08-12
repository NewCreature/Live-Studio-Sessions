#include "t3f/t3f.h"
#include "t3f/gui.h"

void lss_update_gui_colors(T3F_GUI * gp, ALLEGRO_COLOR header_color, ALLEGRO_COLOR selected_color, ALLEGRO_COLOR normal_color)
{
	int i;
	
	for(i = 0; i < gp->elements; i++)
	{
		if(gp->element[i].flags & T3F_GUI_ELEMENT_STATIC)
		{
			gp->element[i].color = header_color;
		}
		else if(i == gp->hover_element)
		{
			gp->element[i].color = selected_color;
		}
		else
		{
			gp->element[i].color = normal_color;
		}
	}
}
