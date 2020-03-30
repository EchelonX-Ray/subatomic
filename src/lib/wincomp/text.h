#ifndef _insertion_lib_wincomp_text_h
#define _insertion_lib_wincomp_text_h

#include "./wincomp.h"
#include "./drawing.h"
#include <stdint.h>

// I am only going to deal with 1 font style for now.  I'll change this later.
#define MTK_FONT_STYLE_COUNT 1
// Set to the first printable ASCII charactor. (0x20 = [Space])
#define MTK_FONT_ASCII_CHAR_MIN 0x20
// Set to the last printable ASCII charactor. (0x7E = "~")
// There may be more ASCII charactors, but I don't want to deal with them for now
#define MTK_FONT_ASCII_CHAR_MAX 0x7E

struct MTK_WinFontCharactorBM {
	unsigned int width;
	unsigned int height;
	unsigned char *char_bitmap;
};

struct MTK_WinFontMap {
	unsigned int font_style;
	unsigned int font_size;
	unsigned int max_width;
	unsigned int max_height;
	struct MTK_WinFontCharactorBM *bitmaps;
	struct MTK_WinFontCharactorBM *offmaps;
};

struct MTK_WinFontPack {
	unsigned int count;
	unsigned int max_width;
	unsigned int max_height;
	struct MTK_WinFontMap *font_style;
};

unsigned int draw_char(unsigned int x, unsigned int y, char charactor, uint32_t color, struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window);

#endif
