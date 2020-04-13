//insertion mtkfunc itoa.h file
#ifndef _insertion_cstr_manip_h
#define _insertion_cstr_manip_h

#include <stdint.h>

void cmemset(void* to, const unsigned char value, const unsigned int length);
void cmemcpy(const void* from, void* to, const unsigned int length);
unsigned int cstrlen(const char *src);
unsigned int cstrcpy(const char *src, char *dest);
unsigned int cstrcmp(const char *string1, const char *string2);
void citoa(int32_t, char*, int, int);
int catoi(char*, unsigned int);

#endif
