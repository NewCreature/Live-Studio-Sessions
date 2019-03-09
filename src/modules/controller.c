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
				if(t3f_touch[i].active)
				{
					x = t3f_touch[i].x;
					y = t3f_touch[i].y;
					if(x >= LSS_TOUCH_OFFSET_X && x < LSS_TOUCH_OFFSET_X + LSS_TOUCH_SPACE_X * 5 && y >= LSS_TOUCH_OFFSET_Y)
					{
						cp->controller->state[(x - LSS_TOUCH_OFFSET_X) / LSS_TOUCH_SPACE_X].down = true;
					}
				}
			}
			break;
		}
	}
	t3f_update_controller(cp->controller);
}
