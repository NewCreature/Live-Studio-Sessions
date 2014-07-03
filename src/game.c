#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"

#include "resources.h"
#include "game.h"
#include "player.h"

void lss_add_bitmap_to_atlas(T3F_ATLAS * ap, ALLEGRO_BITMAP ** bp, int type)
{
	ALLEGRO_BITMAP * abp;

	abp = t3f_add_bitmap_to_atlas(ap, bp, type);
	if(abp)
	{
		al_destroy_bitmap(*bp);
		*bp = abp;
	}
}

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path)
{
	gp->note_texture[0] = t3f_load_resource((void *)(&gp->note_texture[0]), T3F_RESOURCE_TYPE_BITMAP, "data/note_green_strum.png", 0, 0, 0);
	if(!gp->note_texture[0])
	{
		return false;
	}
	gp->note_texture[1] = t3f_load_resource((void *)(&gp->note_texture[1]), T3F_RESOURCE_TYPE_BITMAP, "data/note_red_strum.png", 0, 0, 0);
	if(!gp->note_texture[1])
	{
		return false;
	}
	gp->note_texture[2] = t3f_load_resource((void *)(&gp->note_texture[2]), T3F_RESOURCE_TYPE_BITMAP, "data/note_yellow_strum.png", 0, 0, 0);
	if(!gp->note_texture[2])
	{
		return false;
	}
	gp->note_texture[3] = t3f_load_resource((void *)(&gp->note_texture[3]), T3F_RESOURCE_TYPE_BITMAP, "data/note_blue_strum.png", 0, 0, 0);
	if(!gp->note_texture[3])
	{
		return false;
	}
	gp->note_texture[4] = t3f_load_resource((void *)(&gp->note_texture[4]), T3F_RESOURCE_TYPE_BITMAP, "data/note_orange_strum.png", 0, 0, 0);
	if(!gp->note_texture[4])
	{
		return false;
	}
	gp->note_texture[5] = t3f_load_resource((void *)(&gp->note_texture[5]), T3F_RESOURCE_TYPE_BITMAP, "data/note_green.png", 0, 0, 0);
	if(!gp->note_texture[5])
	{
		return false;
	}
	gp->note_texture[6] = t3f_load_resource((void *)(&gp->note_texture[6]), T3F_RESOURCE_TYPE_BITMAP, "data/note_red.png", 0, 0, 0);
	if(!gp->note_texture[6])
	{
		return false;
	}
	gp->note_texture[7] = t3f_load_resource((void *)(&gp->note_texture[7]), T3F_RESOURCE_TYPE_BITMAP, "data/note_yellow.png", 0, 0, 0);
	if(!gp->note_texture[7])
	{
		return false;
	}
	gp->note_texture[8] = t3f_load_resource((void *)(&gp->note_texture[8]), T3F_RESOURCE_TYPE_BITMAP, "data/note_blue.png", 0, 0, 0);
	if(!gp->note_texture[8])
	{
		return false;
	}
	gp->note_texture[9] = t3f_load_resource((void *)(&gp->note_texture[9]), T3F_RESOURCE_TYPE_BITMAP, "data/note_orange.png", 0, 0, 0);
	if(!gp->note_texture[9])
	{
		return false;
	}
	gp->studio_image = t3f_load_resource((void *)(&gp->studio_image), T3F_RESOURCE_TYPE_BITMAP, "data/studio.png", 0, 0, 0);
	if(!gp->studio_image)
	{
		return false;
	}
	gp->fret_board_image = t3f_load_resource((void *)(&gp->fret_board_image), T3F_RESOURCE_TYPE_BITMAP, "data/fret_board.png", 0, 0, 0);
	if(!gp->fret_board_image)
	{
		return false;
	}
	gp->beat_line_image = t3f_load_resource((void *)(&gp->beat_line_image), T3F_RESOURCE_TYPE_BITMAP, "data/beat_line.png", 0, 0, 0);
	if(!gp->beat_line_image)
	{
		return false;
	}
	gp->atlas = t3f_create_atlas(1024, 1024);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[0], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[1], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[2], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[3], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[4], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[5], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[6], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[7], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[8], T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->note_texture[9], T3F_ATLAS_SPRITE);
	gp->current_tick = 0;
	gp->song_audio = lss_load_song_audio(song_path);
	if(!gp->song_audio)
	{
		return false;
	}
	gp->board_y = 420.0;
	gp->board_speed = 12.0;
	lss_initialize_player(gp, 0);
	lss_song_mark_beats(gp->song, gp->song_audio->length);
	al_stop_timer(t3f_timer);
	if(!lss_set_song_audio_playing(gp->song_audio, true))
	{
		return false;
	}
	al_start_timer(t3f_timer);
	gp->done = false;
	return true;
}

void lss_game_exit(LSS_GAME * gp)
{
	int i;
	
	lss_destroy_song_audio(gp->song_audio);
	lss_destroy_song(gp->song);
	for(i = 0; i < 10; i++)
	{
		t3f_destroy_resource(gp->note_texture[i]);
	}
	t3f_destroy_resource(gp->studio_image);
	t3f_destroy_resource(gp->fret_board_image);
	t3f_destroy_resource(gp->beat_line_image);
	t3f_destroy_atlas(gp->atlas);
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
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || gp->player[0].life <= 0 || gp->current_tick >= gp->song_audio->length * 60.0)
	{
		gp->done = true;
		lss_game_exit(gp);
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
	}
	gp->current_tick++;
}

void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp)
{
	al_draw_bitmap(gp->studio_image, 0, 0, 0);
	lss_player_render_board(gp, 0);
	al_hold_bitmap_drawing(true);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "%d/%d", gp->player[0].hit_notes, gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 24, 0, "%d", gp->player[0].life);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 480, 0, ALLEGRO_ALIGN_CENTRE, "Score: %d", gp->player[0].score);
	al_hold_bitmap_drawing(false);
}
