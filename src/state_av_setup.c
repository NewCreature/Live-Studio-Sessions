#include "instance.h"
#include "state_game_player.h"

void lss_state_av_setup_logic(APP_INSTANCE * app)
{
	char buf[64];

	lss_player_logic(&app->game, 0);
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		app->game.av_delay++;
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		app->game.av_delay--;
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE])
	{
		app->game.done = true;
		lss_game_exit(&app->game);
		sprintf(buf, "%d", app->game.av_delay);
		al_set_config_value(t3f_config, "Live Studio Sessions", "av_delay", buf);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	app->game.current_tick++;
}

void lss_state_av_setup_render(APP_INSTANCE * app)
{
	al_draw_bitmap(app->game.studio_image, 0, 0, 0);
	lss_player_render_board(&app->game, 0);
	al_hold_bitmap_drawing(true);
	al_draw_textf(app->resources.font[LSS_FONT_LARGE], t3f_color_white, 0, 0, 0, "Live Studio Sessions - AV Setup");
	al_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 48, 0, "Delay - %d", app->game.av_delay);
	al_hold_bitmap_drawing(false);
}
