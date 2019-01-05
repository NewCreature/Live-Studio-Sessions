#include "t3f/t3f.h"
#include "t3f/resource.h"
#include "t3f/view.h"
#include "t3f/draw.h"
#include "t3f/primitives.h"
#include "t3f/debug.h"

#include "resources.h"
#include "state_game.h"
#include "state_game_player.h"

void lss_add_bitmap_to_atlas(T3F_ATLAS * ap, ALLEGRO_BITMAP ** bp, int type)
{
	t3f_add_bitmap_to_atlas(ap, bp, type);
}

bool lss_game_initialize(LSS_GAME * gp, ALLEGRO_PATH * song_path)
{
	t3f_debug_message("lss_game_initialize() enter\n");
	al_stop_timer(t3f_timer);
	t3f_debug_message("\tLoading textures...\n");
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
	gp->fret_button_image = t3f_load_resource((void *)(&gp->fret_button_image), T3F_RESOURCE_TYPE_BITMAP, "data/fret_button.png", 0, 0, 0);
	if(!gp->fret_button_image)
	{
		return false;
	}
	gp->strum_bar_image = t3f_load_resource((void *)(&gp->strum_bar_image), T3F_RESOURCE_TYPE_BITMAP, "data/strum_bar.png", 0, 0, 0);
	if(!gp->strum_bar_image)
	{
		return false;
	}
	t3f_debug_message("\tGenerating atlas...\n");
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
	lss_add_bitmap_to_atlas(gp->atlas, &gp->fret_button_image, T3F_ATLAS_SPRITE);
	lss_add_bitmap_to_atlas(gp->atlas, &gp->strum_bar_image, T3F_ATLAS_SPRITE);
	t3f_debug_message("\tLoading song audio...\n");
	gp->song_audio = lss_load_song_audio(song_path);
	if(!gp->song_audio)
	{
		return false;
	}
	t3f_debug_message("\tCreating primitives cache...\n");
	gp->primitives = t3f_create_primitives_cache(1024);
	if(!gp->primitives)
	{
		return false;
	}
	gp->board_y = 420.0;
//	gp->board_speed = 1.0;
	gp->board_speed = 12.0;
	gp->delay_z = gp->av_delay * LSS_SONG_PLACEMENT_SCALE;
	t3f_debug_message("\tInitializing player...\n");
	lss_initialize_player(gp, 0);
	t3f_debug_message("\tGenerating beat markers...\n");
	lss_song_mark_beats(gp->song, gp->song_audio->length);
	gp->current_tick = gp->song->beat[0]->tick;
	gp->current_beat = 0;
	gp->camera_z = gp->song->beat[0]->z - gp->delay_z;
	gp->camera_vz = (float)LSS_SONG_PLACEMENT_SCALE * (gp->board_speed * (gp->song->beat[0]->BPM / 120.0));
	al_start_timer(t3f_timer);
	gp->done = false;
	t3f_debug_message("lss_game_initialize() exit\n");
	return true;
}

void lss_game_exit(LSS_GAME * gp)
{
	int i;

	t3f_debug_message("lss_game_exit() enter\n");
	t3f_debug_message("\tDestroying primitives cache...\n");
	t3f_destroy_primitives_cache(gp->primitives);
	t3f_debug_message("\tDestroying song audio...\n");
	lss_destroy_song_audio(gp->song_audio);
	t3f_debug_message("\tDestroying song...\n");
	lss_destroy_song(gp->song);
	t3f_debug_message("\tDestroying textures...\n");
	for(i = 0; i < 10; i++)
	{
		t3f_destroy_resource(gp->note_texture[i]);
	}
	t3f_destroy_resource(gp->studio_image);
	t3f_destroy_resource(gp->fret_board_image);
	t3f_destroy_resource(gp->beat_line_image);
	t3f_destroy_resource(gp->fret_button_image);
	t3f_destroy_resource(gp->strum_bar_image);
	t3f_debug_message("\tDestroying atlas...\n");
	t3f_destroy_atlas(gp->atlas);
	t3f_debug_message("lss_game_exit() exit\n");
}

static void lss_game_get_player_results(LSS_GAME * gp, int player)
{
	int i;

	t3f_debug_message("lss_game_get_player_results() enter\n");
	gp->player[0].total_notes = 0;
	gp->player[0].hit_notes = 0;
	gp->player[0].missed_notes = 0;
	gp->player[0].bad_notes = 0;
	gp->player[0].good_notes = 0;
	gp->player[0].perfect_notes = 0;
	for(i = 0; i < gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes; i++)
	{
		if(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->hit_level != LSS_SONG_NOTE_HIT_LEVEL_NONE)
		{
			gp->player[0].total_notes++;
			switch(gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].note[i]->hit_level)
			{
				case LSS_SONG_NOTE_HIT_LEVEL_MISS:
				{
					gp->player[0].missed_notes++;
					break;
				}
				case LSS_SONG_NOTE_HIT_LEVEL_BAD:
				{
					gp->player[0].hit_notes++;
					gp->player[0].bad_notes++;
					break;
				}
				case LSS_SONG_NOTE_HIT_LEVEL_OKAY:
				{
					gp->player[0].hit_notes++;
					break;
				}
				case LSS_SONG_NOTE_HIT_LEVEL_GOOD:
				{
					gp->player[0].hit_notes++;
					gp->player[0].good_notes++;
					break;
				}
				case LSS_SONG_NOTE_HIT_LEVEL_PERFECT:
				{
					gp->player[0].hit_notes++;
					gp->player[0].perfect_notes++;
					break;
				}
			}
		}
	}

	gp->player[0].stars = 0;
	/* give one star for completing the song */
	if(gp->player[0].total_notes >= gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes)
	{
		gp->player[0].stars++;
	}

	/* give one star for hitting half of the notes */
	if(gp->player[0].stars && (gp->player[0].hit_notes * 100) / gp->player[0].total_notes >= 50)
	{
		gp->player[0].stars++;
	}

	/* give one star for hitting three quarters of the notes */
	if(gp->player[0].stars && (gp->player[0].hit_notes * 100) / gp->player[0].total_notes >= 75)
	{
		gp->player[0].stars++;
	}

	/* give one star for hitting all of the notes */
	if(gp->player[0].stars && (gp->player[0].hit_notes * 100) / gp->player[0].total_notes >= 100)
	{
		gp->player[0].stars++;
	}

	/* give one star for no bad notes */
	if(gp->player[0].stars >= 4 && gp->player[0].perfect_notes + gp->player[0].good_notes >= gp->player[0].total_notes)
	{
		gp->player[0].stars++;
	}

	/* calculate accuracy */
	if(gp->player[0].total_notes > 0)
	{
		gp->player[0].accuracy = ((double)(gp->player[0].hit_notes) * 100.0) / (double)gp->player[0].total_notes;
	}
	else
	{
		gp->player[0].accuracy = -1.0;
	}

	/* calculate completion percentage */
	gp->player[0].completion = ((float)gp->player[0].total_notes / (float)gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes) * 100.0;

	/* full combo only awarded if song is completed */
	if(!gp->player[0].stars)
	{
		gp->player[0].full_combo = false;
	}
	t3f_debug_message("lss_game_get_player_results() exit\n");
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
	if(t3f_key[ALLEGRO_KEY_ESCAPE] || t3f_key[ALLEGRO_KEY_BACK] || gp->player[0].life <= 0 || gp->current_tick >= (gp->song_audio->length + gp->song->offset) * 60.0)
	{
		lss_set_song_audio_playing(gp->song_audio, false);
		lss_game_get_player_results(gp, 0);
		gp->done = true;
		t3f_key[ALLEGRO_KEY_ESCAPE] = 0;
		t3f_key[ALLEGRO_KEY_BACK] = 0;
	}
	if(gp->current_tick == 0)
	{
		al_stop_timer(t3f_timer);
		if(!lss_set_song_audio_playing(gp->song_audio, true))
		{
//			return false;
		}
		al_start_timer(t3f_timer);
	}
	gp->current_tick++;
	gp->camera_z += gp->camera_vz;

	/* update camera speed if we crossed over beat line */
	if(gp->current_beat < gp->song->beats - 1)
	{
		if(gp->current_tick >= gp->song->beat[gp->current_beat + 1]->tick)
		{
			gp->current_beat++;
			gp->camera_vz = (float)LSS_SONG_PLACEMENT_SCALE * (gp->board_speed * (gp->song->beat[gp->current_beat]->BPM / 120.0));
		}
	}
}

void lss_game_render(LSS_GAME * gp, LSS_RESOURCES * rp)
{
	double completed;
	int n;
	int progress_height = 2;

	al_draw_bitmap(gp->studio_image, 0, 0, 0);
	lss_player_render_board(gp, 0);
	completed = lss_get_song_audio_position(gp->song_audio) / lss_get_song_audio_length(gp->song_audio);
	al_draw_filled_rectangle(0, t3f_default_view->bottom - progress_height, t3f_virtual_display_width * completed, t3f_default_view->bottom, al_map_rgba_f(1.0, 1.0, 0.0, 1.0));
	al_hold_bitmap_drawing(true);
	n = gp->song->track[gp->player[0].selected_track][gp->player[0].selected_difficulty].notes;
	al_draw_textf(rp->font[LSS_FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 2, 0 + 2, 0, "Streak: %d", gp->player[0].streak);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 0, 0, "Streak: %d", gp->player[0].streak);
	al_draw_textf(rp->font[LSS_FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 2, 24 + 2, 0, "Multiplier: %d", gp->player[0].multiplier);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 24, 0, "Multiplier: %d", gp->player[0].multiplier);
	al_draw_textf(rp->font[LSS_FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 0 + 2, 48 + 2, 0, "Life: %d", gp->player[0].life);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 0, 48, 0, "Life: %d", gp->player[0].life);
	al_draw_textf(rp->font[LSS_FONT_SMALL], al_map_rgba_f(0.0, 0.0, 0.0, 0.5), 480 + 2, 0 + 2, ALLEGRO_ALIGN_CENTRE, "Score: %d", gp->player[0].score);
	al_draw_textf(rp->font[LSS_FONT_SMALL], t3f_color_white, 480, 0, ALLEGRO_ALIGN_CENTRE, "Score: %d", gp->player[0].score);
	al_hold_bitmap_drawing(false);
}
