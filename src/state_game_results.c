#include "modules/obfuscate.h"

#include "instance.h"
#include "state_game.h"

void lss_game_results_logic(APP_INSTANCE * app)
{
}

void lss_game_results_render(APP_INSTANCE * app)
{
	int total_notes;
	int pos = 0;
	int i;
	
	total_notes = app->game.player[0].hit_notes + app->game.player[0].missed_notes;
	al_hold_bitmap_drawing(true);
	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "Results for %s", app->game.player[0].profile->name);
	pos += 48;
	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "Score: %d", app->game.player[0].score); 
	pos += 24;
	if(total_notes > 0)
	{
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "Accuracy: %d", (app->game.player[0].hit_notes * 100) / total_notes);
	}
	else
	{
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "Accuracy: N/A");
	}
	pos += 24;
	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "High Score: %d", app->game.player[0].high_score);
	if(app->leaderboard)
	{
		pos += 48;
		al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "Global Top Scores");
		pos += 48;
		for(i = 0; i < app->leaderboard->entries; i++)
		{
			al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, pos, 0, "%s", app->leaderboard->entry[i]->name);
			al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 480, pos, 0, "%d", lss_unobfuscate_value(app->leaderboard->entry[i]->score));
			pos += 24;
		}
	}
	al_hold_bitmap_drawing(false);
}
