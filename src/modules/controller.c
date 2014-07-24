#include "t3f/t3f.h"
#include "t3f/controller.h"

#include "controller.h"

void lss_read_controller(LSS_CONTROLLER * cp)
{
	int i, x, y;

	switch(cp->source)
	{
		/* read controller input supplied by T3F */
		case LSS_CONTROLLER_SOURCE_CONTROLLER:
		{
			t3f_read_controller(cp->controller);
			break;
		}

		/* fill in controller data based on touch input */
		case LSS_CONTROLLER_SOURCE_TOUCH:
		{
			for(i = 0; i < cp->controller->bindings; i++)
			{
				cp->controller->state[i].down = false;
			}
			for(i = 0; i < T3F_MAX_TOUCHES; i++)
			{
				if(i == cp->strum_touch)
				{
					if(t3f_touch[i].active)
					{
						cp->old_strum_pos = cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pos;
						cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pos = t3f_touch[i].y;
						if((cp->old_strum_pos >= 270 && cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pos < 270) ||
						   (cp->old_strum_pos < 270 && cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pos >= 270))
						{
							cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].down = true;
						}
					}
					else
					{
						cp->strum_touch = -1;
					}
				}
				else
				{
					if(t3f_touch[i].active)
					{
						x = t3f_touch[i].x;
						y = t3f_touch[i].y;
						if(x < 64 * 5 && y < 64)
						{
							cp->controller->state[x / 64].down = true;
						}
						else if(x > 480)
						{
							cp->strum_touch = i;
							cp->old_strum_pos = t3f_touch[i].y;
							cp->controller->state[LSS_CONTROLLER_BINDING_GUITAR_STRUM_DOWN].pos = t3f_touch[i].y;
						}
					}
				}
			}
			break;
		}
	}
	t3f_update_controller(cp->controller);
}
