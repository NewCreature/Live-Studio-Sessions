#ifndef T3NET_INTERNAL_H
#define T3NET_INTERNAL_H

typedef struct
{

	char name[256];
	char data[256];

} T3NET_TEMP_ELEMENT;

extern int t3net_written;
size_t t3net_internal_write_function(void * ptr, size_t size, size_t nmemb, void * stream);
int t3net_read_line(const char * data, char * output, int data_max, int output_max, unsigned int * text_pos);
int t3net_get_element(const char * data, T3NET_TEMP_ELEMENT * element, int data_max);

#endif
