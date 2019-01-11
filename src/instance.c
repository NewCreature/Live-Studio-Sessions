#include "instance.h"

APP_INSTANCE * lss_create_instance(void)
{
	APP_INSTANCE * app;

	app = malloc(sizeof(APP_INSTANCE));
	if(app)
	{
		memset(app, 0, sizeof(APP_INSTANCE));
	}
	return app;
}

void lss_destroy_instance(APP_INSTANCE * app)
{
	free(app);
}
