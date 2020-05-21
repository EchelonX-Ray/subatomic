#ifndef _insertion_lib_wincomp_menubar_menu_h
#define _insertion_lib_wincomp_menubar_menu_h

#include <stdint.h>
#include "./../wincomp.h"
#include "./../elements.h"

#define MenuT_Branch 1
#define MenuT_Leaf 2
#define MenuT_Stub 3

struct EL_menuitem_t {
	unsigned int type;
	unsigned int is_selected;
	//unsigned int mouse_state;
	struct EL_menu_t *menu;
	void *callback;
	char *text;
	struct MTK_WinFontMap *fontmap;
	unsigned int left_padding;
	unsigned int right_padding;
	uint32_t bg_color;
	uint32_t text_color;
	uint32_t hover_bg_color;
	uint32_t hover_text_color;
	uint32_t down_bg_color;
	uint32_t down_text_color;
};

struct EL_menu_t {
	unsigned int item_count;
	struct EL_menuitem_t *menu_item;
};

void draw_menuitem(struct MTK_WinElement *element, struct MTK_WinBase *window);
unsigned int menuitem_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int menuitem_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);
unsigned int menuitem_event_button(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window);

#endif
