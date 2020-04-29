#ifndef _insertion_lib_wincomp_elements_textbox_h
#define _insertion_lib_wincomp_elements_textbox_h

#include "./../elements.h"

struct EL_textbox_t {
	uint32_t bg_color;
	uint32_t text_color;
	uint32_t def_text_color;
	uint32_t border_color;
	char *text;
	char *def_text;
	unsigned int alloc_unit;
	unsigned int current_alloc_unit_count;
	unsigned int current_bytes_used;
	unsigned int cursor_position;
	unsigned int cursor_selection_base_position;
	unsigned int text_drawing_offset;
	struct MTK_WinFontMap *fontmap;
};

void draw_textbox(struct MTK_WinElement *element, struct MTK_WinBase *window);
unsigned int textbox_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int textbox_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int textbox_event_button(unsigned int state, unsigned int button, signed int x, signed int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
void textbox_event_key(int state, int keycode, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);

#endif
