#include "t3f/resource.h"

#include "resources.h"

bool lss_load_global_resources(LSS_RESOURCES * rp)
{
	rp->font = t3f_load_resource((void *)(&rp->font), T3F_RESOURCE_TYPE_FONT, "data/teen.ttf", 20, 0, 0);
	if(!rp->font)
	{
		return false;
	}
	return true;
}

void lss_free_global_resources(LSS_RESOURCES * rp)
{
	t3f_destroy_resource(rp->font);
}
