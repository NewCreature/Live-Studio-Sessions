#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"

#include "resources.h"
#include "game.h"
#include "player.h"

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path)
{
	gp->note_texture[0] = t3f_load_resource((void *)(&gp->note_texture[0]), T3F_RESOURCE_TYPE_BITMAP, "data/note_strum.png", 0, 0, 0);
	if(!gp->note_texture[0])
	{
		return false;
	}
	gp->note_texture[1] = t3f_load_resource((void *)(&gp->note_texture[1]), T3F_RESOURCE_TYPE_BITMAP, "data/note_tap.png", 0, 0, 0);
	if(!gp->note_texture[1])
	{
		return false;
	}
	gp->studio_image = t3f_load_resource((void *)(&gp->studio_image), T3F_RESOURCE_TYPE_BITMAP, "data/studio.png", 0, 0, 0);
	if(!gp->studio_image)
	{
		return false;
	}
	gp->current_tick = 0;
	gp->song_audio = lss_load_song_audio(song_path);
	if(!gp->song_audio)
	{
		return false;
	}
	gp->board_y = 420.0;
	gp->board_speed = 12.0;
	lss_initialize_player(gp, 0);
	lss_set_song_audio_playing(gp->song_audio, true);
	gp->done = false;
	return true;
}

void lss_game_exit(LSS_GAME * gp)
{
	t3f_destroy_resource(gp->note_texture[0]);
	t3f_destroy_resource(gp->note_texture[1]);
	t3f_destroy_resource(gp->studio_image);
}

void lss_game_logic(LSS_GAME * gp)
{
	lss_player_logic(gp, 0);
	if(t3f_key[ALLEGRO_KEY_UP])
	{
		gp->board_speed += 1.0;
		t3f_key[ALLEGRO_KEY_UP] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_DOWN])
	{
		gp->board_speed -= 1.0;
		t3f_key[ALLEGRO_KEY_DOWN] = 0;
	}
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || gp->player[0].life <= 0)
	{
		lss_destroy_song_audio(gp->song_audio);
		lss_destroy_song(gp->song);
		gp->done = true;
		lss_game_exit(gp);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	gp->current_tick++;
}

void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp)
{
	lss_player_render_board(gp, 0);
	al_hold_bitmap_drawing(true);
	al_draw_textf(rp->font, t3f_color_white, 0, 0, 0, "%d/%d", gp->player[0].hit_notes, gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes);
	al_draw_textf(rp->font, t3f_color_white, 0, 24, 0, "%d", gp->player[0].life);
	al_draw_textf(rp->font, t3f_color_white, 480, 0, ALLEGRO_ALIGN_CENTRE, "Score: %d", gp->player[0].score);
	al_hold_bitmap_drawing(false);
}
