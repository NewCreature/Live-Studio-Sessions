#include "t3f/resource.h"

#include "resources.h"

bool lss_load_global_resources(LSS_RESOURCES * rp)
{
	rp->font[LSS_FONT_SMALL] = t3f_load_resource((void *)(&rp->font[LSS_FONT_SMALL]), T3F_RESOURCE_TYPE_FONT, "data/teen.ttf", 20, 0, 0);
	if(!rp->font[LSS_FONT_SMALL])
	{
		return false;
	}
	rp->font[LSS_FONT_LARGE] = t3f_load_resource((void *)(&rp->font[LSS_FONT_LARGE]), T3F_RESOURCE_TYPE_FONT, "data/teen.ttf", 40, 0, 0);
	if(!rp->font[LSS_FONT_LARGE])
	{
		return false;
	}
	return true;
}

void lss_free_global_resources(LSS_RESOURCES * rp)
{
	int i;
	
	for(i = 0; i < LSS_MAX_FONTS; i++)
	{
		t3f_destroy_resource(rp->font[i]);
	}
}
