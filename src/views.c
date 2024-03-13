#include "struct_game.h"

void lss_update_views(LSS_GAME * gp)
{
	int i, j, c, x, y, w, h;
	int w1 = t3f_default_view->right - t3f_default_view->left;
	int h1 = t3f_default_view->bottom - t3f_default_view->top;
	int w2 = (t3f_default_view->right - t3f_default_view->left) / 2;
	int h2 = (t3f_default_view->bottom - t3f_default_view->top) / 2;
	int w3 = (t3f_default_view->right - t3f_default_view->left) / 4;
	int h3 = (t3f_default_view->bottom - t3f_default_view->top) / 4;
	int xywh1[] = {t3f_default_view->left, t3f_default_view->top, w1, h1};
	int xywh2[] = {t3f_default_view->left, t3f_default_view->top + h2, w2, h2, t3f_default_view->left + w2, t3f_default_view->top + h2, w2, h2};
	int xywh3[] = {t3f_default_view->left, t3f_default_view->top + h2, w2, h2, t3f_default_view->left + w2, t3f_default_view->top + h2, w2, h2, t3f_default_view->left, t3f_default_view->top, w2, h2};
	int xywh4[] = {t3f_default_view->left, t3f_default_view->top + h2, w2, h2, t3f_default_view->left + w2, t3f_default_view->top + h2, w2, h2, t3f_default_view->left, t3f_default_view->top, w2, h2, t3f_default_view->left + w2, t3f_default_view->top, w2, h2};
	int xywh5[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int xywh6[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int xywh7[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int xywh8[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	int * xywh = NULL;

	/* count players so we can set up correct split screen */
	c = 0;
	for(i = 0; i < LSS_MAX_PLAYERS; i++)
	{
		if(gp->player[i].active)
		{
			c++;
		}
	}

	/* single player */
	switch(c)
	{
		case 1:
		{
			xywh = xywh1;
			break;
		}
		case 2:
		{
			xywh = xywh2;
			break;
		}
		case 3:
		{
			xywh = xywh3;
			break;
		}
		case 4:
		{
			xywh = xywh4;
			break;
		}
	}
	if(!xywh)
	{
		return;
	}
	j = 0;
	for(i = 0; i < LSS_MAX_PLAYERS; i++)
	{
		if(gp->player[i].active)
		{
			x = xywh[j * 4 + 0];
			y = xywh[j * 4 + 1];
			w = xywh[j * 4 + 2];
			h = xywh[j * 4 + 3];
			t3f_adjust_view(gp->player[i].view, x, y, w, h, t3f_virtual_display_width / 2, t3f_virtual_display_height / 2, t3f_flags);
			j++;
		}
	}
}
