#ifndef _insertion_lib_wincomp_elements_button_h
#define _insertion_lib_wincomp_elements_button_h

#include "./../elements.h"

struct EL_button_t {
	uint32_t bg_color;
	uint32_t bg_hover_color;
	uint32_t bg_down_color;
	uint32_t text_color;
	uint32_t text_hover_color;
	uint32_t text_down_color;
	uint32_t border_color;
	char *text;
	struct MTK_WinFontMap *fontmap;
};

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window);
unsigned int button_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int button_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);

#endif
