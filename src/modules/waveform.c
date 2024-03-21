#include "t3f/t3f.h"
#include "song_audio.h"
#include "waveform.h"

LSS_WAVEFORM * lss_create_waveform(int buf_size)
{
  LSS_WAVEFORM * wp;

  wp = malloc(sizeof(LSS_WAVEFORM));
  if(!wp)
  {
    goto fail;
  }
  memset(wp, 0, sizeof(LSS_WAVEFORM));
  wp->data = malloc(sizeof(float) * buf_size);
  if(!wp->data)
  {
    goto fail;
  }
  memset(wp->data, 0, sizeof(float) * buf_size);
  wp->data_size = buf_size;

  return wp;

  fail:
  {
    lss_destroy_waveform(wp);
    return NULL;
  }
}

void lss_destroy_waveform(LSS_WAVEFORM * wp)
{
  if(wp)
  {
    if(wp->data)
    {
      free(wp->data);
    }
    free(wp);
  }
}

void lss_waveform_callback(void * buf, unsigned int samples, void * data)
{
  LSS_SONG_AUDIO * ap = (LSS_SONG_AUDIO *)data;
  float * float_data = (float *)buf;
  int i;

  for(i = 0; i < samples && i < ap->waveform->data_size; i++)
  {
    ap->waveform->data[i] = float_data[i];
  }
}

void lss_render_waveform(LSS_WAVEFORM * wp, float ox, float oy, ALLEGRO_COLOR color, float (*transform)(LSS_WAVEFORM * wp, int pos))
{
  int i;
  float start_y, end_y;

  for(i = 0; i < wp->data_size - 1; i++)
  {
    if(transform)
    {
      start_y = transform(wp, i);
    }
    else
    {
      start_y = wp->data[i];
    }
    if(transform)
    {
      end_y = transform(wp, i + 1);
    }
    else
    {
      end_y = wp->data[i + 1];
    }
    al_draw_line(ox + i, oy + start_y, ox + i + 1, oy + end_y, color, 1.0);
  }
}
