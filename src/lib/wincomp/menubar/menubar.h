#ifndef _insertion_lib_wincomp_menubar_menubar_h
#define _insertion_lib_wincomp_menubar_menubar_h

#include "./menu.h"
#include "./../elements.h"

struct EL_menubar_t {
//	unsigned int item_count;
//	struct EL_menuitem_t *menu_item;
	uint32_t bg_color;
};

void draw_menubar(struct MTK_WinElement *element, struct MTK_WinBase *window);
unsigned int menubar_event_button(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int menubar_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int menubar_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);

#endif
