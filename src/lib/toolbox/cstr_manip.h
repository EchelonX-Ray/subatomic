//insertion mtkfunc itoa.h file
#ifndef _insertion_cstr_manip_h
#define _insertion_cstr_manip_h

#include <stdint.h>

char* cstrdup(const char *str);
void cdelbstr(char **heap_buffer_ptr, signed int offset, signed int length);
void cdelstr(char **heap_buffer_ptr, signed int offset, signed int length);
void cinsstr(const char *payload, char **heap_buffer_ptr, signed int offset);
void cmemset(void* to, const unsigned char value, unsigned int length);
void cmemcpy(const void* from, void* to, const signed int length);
unsigned int cstrlen(const char *src);
unsigned int cstrcpy(const char *src, char *dest);
unsigned int cstrcmp(const char *string1, const char *string2);
void citoa(int32_t, char*, int, int);
int catoi(char*, unsigned int);

#endif
