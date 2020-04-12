#ifndef _insertion_lib_wincomp_elements_button_h
#define _insertion_lib_wincomp_elements_button_h

#include "./../elements.h"

struct EL_button_t {
	uint32_t color;
	uint32_t hover_color;
	uint32_t down_color;
	uint32_t text_color;
	uint32_t text_hover_color;
	uint32_t text_down_color;
	uint32_t border_color;
	char *text;
	struct MTK_WinFontMap *fontmap;
};

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window);

#endif
