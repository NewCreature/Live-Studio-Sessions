#include "game.h"

void lss_game_results_logic(LSS_GAME * gp)
{
}

void lss_game_results_render(LSS_GAME * gp, LSS_RESOURCES * rp)
{
	al_hold_bitmap_drawing(true);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "Results for %s", gp->player[0].profile->name);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 48, 0, "Score: %d", gp->player[0].score);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 72, 0, "Accuracy: %d", (gp->player[0].hit_notes * 100) / gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 96, 0, "High Score: %d", gp->player[0].high_score);
	al_hold_bitmap_drawing(false);
}
