#include "./menu.h"
#include "./../text.h"

void draw_menuitem(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	struct EL_menuitem_t *type_spec;
	type_spec = element->type_spec;
	uint32_t bg_color;
	uint32_t text_color;
	if        (element->mouse_state == EL_MS_DOWN) {
		bg_color = type_spec->down_bg_color;
		text_color = type_spec->down_text_color;
	} else if (element->mouse_state == EL_MS_HOVER) {
		bg_color = type_spec->hover_bg_color;
		text_color = type_spec->hover_text_color;
	} else {
		bg_color = type_spec->bg_color;
		text_color = type_spec->text_color;
	}
	
	signed int x;
	signed int y;
	signed int width;
	signed int height;
	x = element->_internal_computed_xoffset;
	y = element->_internal_computed_yoffset;
	width = element->_internal_computed_width;
	height = element->_internal_computed_height;
	
	set_pixel_element_map(x, y, width, height, window, element);
	fill_rect(x, y, width, height, bg_color, window);
	if (width > type_spec->left_padding) {
		signed int vertical_pad;
		vertical_pad = element->_internal_computed_height - get_char_height('A', type_spec->fontmap);
		if (vertical_pad < 0) {
			vertical_pad = 0;
		} else {
			vertical_pad /= 2;
		}
		draw_text(x + type_spec->left_padding, y + vertical_pad, 0, 0, width - type_spec->left_padding, height, type_spec->text, text_color, type_spec->fontmap, window);
	}
	
	return;
}
unsigned int menuitem_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if (element->mouse_state != EL_MS_NORMAL) {
		element->mouse_state = EL_MS_NORMAL;
		draw_element(window->root_element, window);
		redraw_required |= 0x1;
	}
	return redraw_required;
}
unsigned int menuitem_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if (window->_internal_mouse_state.previous_mouse_element != element) {
		if (element->mouse_state == EL_MS_NORMAL) {
			element->mouse_state = EL_MS_HOVER;
			draw_element(element, window);
			redraw_required |= 0x1;
		}
	}
	return redraw_required;
}
unsigned int menuitem_event_button(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if        (state == MS_DOWN) {
		element->mouse_state = EL_MS_DOWN;
		draw_element(element, window);
	} else if (state == MS_UP) {
		// In the event of a mouse-up event, we need to double check if the mouse is over the element.
		// This function is called on mouse-up for the element to which the previous mouse-down event occured, 
		// even if the mouse is no longer over top.
		signed int x_max;
		signed int y_max;
		x_max = element->_internal_computed_xoffset + element->_internal_computed_width;
		y_max = element->_internal_computed_yoffset + element->_internal_computed_height;
		if (x >= element->_internal_computed_xoffset && x <= x_max && y >= element->_internal_computed_yoffset && y <= y_max) {
			element->mouse_state = EL_MS_HOVER;
		} else {
			element->mouse_state = EL_MS_NORMAL;
		}
		draw_element(window->root_element, window);
	}
	redraw_required |= 0x1;
	return redraw_required;
}
