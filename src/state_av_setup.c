#include "instance.h"
#include "state_game_player.h"

void lss_state_av_setup_logic(APP_INSTANCE * app)
{
	char buf[64];
	bool touch_up = false, touch_down = false;
	int i;

	/* check touch controls */
	for(i = 0; i < T3F_MAX_TOUCHES; i++)
	{
		if(t3f_touch[i].active)
		{
			if(t3f_touch[i].y < 270)
			{
				touch_up = true;
			}
			else
			{
				touch_down = true;
			}
			t3f_touch[i].active = 0;
		}
	}

	lss_player_logic(&app->game, 0);
	if(t3f_key[ALLEGRO_KEY_UP] || touch_up)
	{
		app->game.av_delay++;
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN] || touch_down)
	{
		app->game.av_delay--;
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK])
	{
		app->game.done = true;
		lss_game_exit(&app->game);
		sprintf(buf, "%d", app->game.av_delay);
		al_set_config_value(t3f_config, "Live Studio Sessions", "av_delay", buf);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
	if(app->game.current_tick == 0)
	{
		al_stop_timer(t3f_timer);
		if(!lss_set_song_audio_playing(app->game.song_audio, true))
		{
//			return false;
		}
		al_start_timer(t3f_timer);
	}
	app->game.current_tick++;
}

void lss_state_av_setup_render(APP_INSTANCE * app)
{
	al_draw_bitmap(app->game.studio_image, 0, 0, 0);
	lss_player_render_board(&app->game, 0);
	al_hold_bitmap_drawing(true);
	t3f_draw_textf(app->resources.font[LSS_FONT_LARGE], t3f_color_white, 0, 0, 0, 0, "Live Studio Sessions - AV Setup");
	t3f_draw_textf(app->resources.font[LSS_FONT_SMALL], t3f_color_white, 0, 48, 0, 0, "Delay - %d", app->game.av_delay);
	al_hold_bitmap_drawing(false);
}
