#include "t3f/controller.h"

#include "controller.h"

void lss_read_controller(LSS_CONTROLLER * cp)
{
	t3f_read_controller(cp->controller);
	t3f_update_controller(cp->controller);
}
