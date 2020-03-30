#ifndef _insertion_lib_wincomp_font_h
#define _insertion_lib_wincomp_font_h

#include "./wincomp.h"
#include "./text.h"
#include <stdlib.h>

void setup_font(char *font_file, unsigned int size, struct MTK_WinFontPack *font_pack);
void free_font(struct MTK_WinFontPack *font_pack);

#endif
