#ifndef _insertion_lib_wincomp_drawing_h
#define _insertion_lib_wincomp_drawing_h

#include "./wincomp.h"
#include <stdint.h>

void fill_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint32_t color, struct MTK_WinBase *window);
void draw_bm(unsigned int x, unsigned int y, unsigned int width, unsigned int height, struct MTK_WinBase *window);

#endif
