#include <stdio.h>

int rtk_io_mgetw(FILE * f)
{
	int b1, b2;

	if((b1 = fgetc(f)) != EOF)
	{
		if((b2 = fgetc(f)) != EOF)
		{
			return ((b1 << 8) | b2);
		}
	}

	return EOF;
}

long rtk_io_mgetl(FILE * f)
{
	int b1, b2, b3, b4;

	if((b1 = fgetc(f)) != EOF)
	{
		if((b2 = fgetc(f)) != EOF)
		{
			if((b3 = fgetc(f)) != EOF)
			{
				if((b4 = fgetc(f)) != EOF)
				{
					return (((long)b1 << 24) | ((long)b2 << 16) | ((long)b3 << 8) | (long)b4);
				}
			}
		}
	}

	return EOF;
}

int rtk_io_mputw(int w, FILE * f)
{
	int b1, b2;

	b1 = (w & 0xFF00) >> 8;
	b2 = w & 0x00FF;

	if(fputc(b1, f) == b1)
	{
		if (fputc(b2, f) == b2)
		{
			return w;
		}
	}

	return EOF;
}

long rtk_io_mputl(long l, FILE * f)
{
	int b1, b2, b3, b4;

	b1 = (int)((l & 0xFF000000L) >> 24);
	b2 = (int)((l & 0x00FF0000L) >> 16);
	b3 = (int)((l & 0x0000FF00L) >> 8);
	b4 = (int)l & 0x00FF;

	if(fputc(b1, f) == b1)
	{
		if(fputc(b2, f) == b2)
		{
			if(fputc(b3, f) == b3)
			{
				if(fputc(b4, f) == b4)
				{
					return l;
				}
			}
		}
	}

	return EOF;
}
