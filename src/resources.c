#include "t3f/resource.h"

#include "resources.h"

bool lss_load_global_resources(LSS_RESOURCES * rp)
{
	rp->font[LSS_FONT_SMALL] = t3f_load_resource((void *)(&rp->font[LSS_FONT_SMALL]), t3f_font_resource_handler_proc, "data/teen.ttf", 20, 0, 0);
	if(!rp->font[LSS_FONT_SMALL])
	{
		return false;
	}
	rp->font[LSS_FONT_LARGE] = t3f_load_resource((void *)(&rp->font[LSS_FONT_LARGE]), t3f_font_resource_handler_proc, "data/teen.ttf", 40, 0, 0);
	if(!rp->font[LSS_FONT_LARGE])
	{
		return false;
	}
	t3f_load_resource((void *)(&rp->bitmap[LSS_BITMAP_PLATINUM]), t3f_bitmap_resource_handler_proc, "data/platinum_record.png", 0, 0, 0);
	if(!rp->bitmap[LSS_BITMAP_PLATINUM])
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
	for(i = 0; i < LSS_MAX_BITMAPS; i++)
	{
		t3f_destroy_resource(rp->bitmap[i]);
	}
}
