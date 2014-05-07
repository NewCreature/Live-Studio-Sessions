#ifndef LIBRTK_IO_H
#define LIBRTK_IO_H

#include <stdio.h>

int rtk_io_mgetw(FILE * f);
int rtk_io_mgetl(FILE * f);
int rtk_io_mputw(int w, FILE * f);
long rtk_io_mputl(long l, FILE * f);

#endif
