#ifndef _insertion_lib_wincomp_text_h
#define _insertion_lib_wincomp_text_h

#include "./wincomp.h"
#include "./drawing.h"
#include <stdint.h>


struct Working_Text {
	char *text;
	unsigned int alloc_unit;
	unsigned int current_alloc_unit_count;
	unsigned int current_bytes_used;
	unsigned int cursor_position;
	unsigned int cursor_base_sel_position;
	unsigned char ml;
};

struct MTK_WinFontCharactorBM {
	unsigned int adv_width;
	unsigned int max_width_pad_left;
	unsigned int max_width_pad_right;
	unsigned int max_height_pad_top;
	unsigned int max_height_pad_bottom;
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
	char **files;
	struct MTK_WinFontMap *font_style;
};

unsigned int get_char_width(char charactor, struct MTK_WinFontMap *fontmap);
unsigned int get_char_height(char charactor, struct MTK_WinFontMap *fontmap);
unsigned int get_text_width(char *string, struct MTK_WinFontMap *fontmap);
unsigned int draw_text(	unsigned int x, unsigned int y, unsigned int clip_width, unsigned int clip_height, 
								unsigned int max_width, unsigned int max_height, char *string, uint32_t color, 
								struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window);
unsigned int draw_char(	unsigned int x, unsigned int y, unsigned int clip_width, unsigned int clip_height, 
								unsigned int max_width, unsigned int max_height, char charactor, uint32_t color, 
								struct MTK_WinFontMap *fontmap, struct MTK_WinBase *window);

#endif
