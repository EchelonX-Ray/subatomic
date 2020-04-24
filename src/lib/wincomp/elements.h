#ifndef _insertion_elements_h
#define _insertion_elements_h

#include "./wincomp.h"
#include "./elements/button.h"
#include "./elements/container.h"
#include "./elements/checkbox.h"
#include "./elements/ml_textbox.h"
#include "./elements/radiobutton.h"
#include "./elements/tab.h"
#include "./elements/textbox.h"

#define EL_BUTTON 1
#define EL_CHECKBOX 2
#define EL_CONTAINER 3
#define EL_FILE_TEXT 4
#define EL_ML_TEXTBOX 5
#define EL_RADIOBUTTON 6
#define EL_TAB 7
#define EL_TEXTBOX 8

#define EL_MS_NORMAL 0
#define EL_MS_HOVER 1
#define EL_MS_DOWN 2

#define EL_AC_XAXIS 1 // This Anchor is used to align on the X-Axis
#define EL_AC_YAXIS 2 // This Anchor is used to align on the Y-Axis
#define EL_AC_RELATIVE_LEFT 0 // This Anchor's x offset is relative to the left side of the element it is anchored to
#define EL_AC_RELATIVE_RIGHT 4 // This Anchor's x offset is relative to the right side of the element it is anchored to
#define EL_AC_RELATIVE_TOP 0 // This Anchor's y offset is relative to the top side of the element it is anchored to
#define EL_AC_RELATIVE_BOTTOM 8 // This Anchor's y offset is relative to the bottom side of the element it is anchored to
#define EL_AC_ELEMENT_LEFT 0 // This Anchor's x offset is relative from the left side of the element it is assigned to
#define EL_AC_ELEMENT_RIGHT 16 // This Anchor's x offset is relative from the right side of the element it is assigned to
#define EL_AC_ELEMENT_TOP 0 // This Anchor's y offset is relative from the top side of the element it is assigned to
#define EL_AC_ELEMENT_BOTTOM 32 // This Anchor's y offset is relative from the bottom side of the element it is assigned to

struct MTK_WinElement {
	unsigned int id;
	signed int width;
	signed int height;
	unsigned int mouse_state;
	unsigned int type;
	void* type_spec;
	void* parent;
	struct MTK_WinElAnchor *anchors;
	unsigned int anchor_count;
	struct MTK_WinElement **children;
	unsigned int child_count;
	
	signed int _internal_computed_xoffset;
	signed int _internal_computed_yoffset;
	signed int _internal_computed_width;
	signed int _internal_computed_height;
};

struct MTK_WinElAnchor {
	signed int x_offset;
	signed int y_offset;
	unsigned int flags;
	struct MTK_WinElement *relative_to;
};

void start_the_cursor(struct MTK_WinBase *window, unsigned int blink_state);
void stop_the_cursor(struct MTK_WinBase *window, unsigned int blink_state);
void reset_the_cursor(struct MTK_WinBase *window, unsigned int blink_state);
void* blink_the_cursor(void* param_ptr);
void compute_element_internals(struct MTK_WinBase *window);
void set_pixel_element_map(signed int x, signed int y, unsigned int width, unsigned int height, struct MTK_WinBase *window, struct MTK_WinElement *element);
void draw_element(struct MTK_WinElement *element, struct MTK_WinBase *window);

#endif
