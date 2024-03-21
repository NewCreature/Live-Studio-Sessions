#ifndef LSS_WAVEFORM_H
#define LSS_WAVEFORM_H

#include "t3f/t3f.h"

typedef struct
{

  float * data;
  int data_size;

} LSS_WAVEFORM;

LSS_WAVEFORM * lss_create_waveform(int buf_size);
void lss_destroy_waveform(LSS_WAVEFORM * wp);
void lss_waveform_callback(void * buf, unsigned int samples, void * data);
void lss_render_waveform(LSS_WAVEFORM * wp, float ox, float oy, ALLEGRO_COLOR color, float (*transform)(LSS_WAVEFORM * wp, int pos));

#endif
