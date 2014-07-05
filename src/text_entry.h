#ifndef LSS_TEXT_ENTRY_H
#define LSS_TEXT_ENTRY_H

bool lss_begin_text_entry(void * data, const char * initial, char * buffer, int max_length, bool (*proc)(void * data, int c));
void lss_end_text_entry(void);
bool lss_process_text_entry(void);

#endif
