//insertion mtkfunc itoa.h file
#ifndef _insertion_cstr_manip_h
#define _insertion_cstr_manip_h

#include <stdint.h>

unsigned int cstrlen(const unsigned char *src);
unsigned int cstrcpy(const unsigned char *src, unsigned char *dest);
unsigned int cstrcmp(const unsigned char *string1, const unsigned char *string2);
void citoa(int32_t, char*, int, int);
int catoi(char*, unsigned int);

#endif
