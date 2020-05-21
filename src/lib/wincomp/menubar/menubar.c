#include "./menubar.h"
#include "./../text.h"

void draw_menubaritem(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	struct EL_menuitem_t *menuitem;
	menuitem = element->type_spec;
	
	// Draw Menu Item
	unsigned int item_width;
	item_width = get_text_width(menuitem->text, menuitem->fontmap);
	item_width += menuitem->left_padding;
	item_width += menuitem->right_padding;
	
	uint32_t bg_color;
	uint32_t text_color;
	if (element->mouse_state == EL_MS_NORMAL) {
		bg_color = menuitem->bg_color;
		text_color = menuitem->text_color;
	} else if (element->mouse_state == EL_MS_HOVER) {
		bg_color = menuitem->bg_color;
		text_color = menuitem->text_color;
	} else if (element->mouse_state == EL_MS_DOWN) {
		bg_color = menuitem->bg_color;
		text_color = menuitem->text_color;
	} else {
		goto skip_item_because_state_is_invalid;
	}
	
	signed int vertical_pad;
	vertical_pad = element->_internal_computed_height - get_char_height('A', menuitem->fontmap);
	if (vertical_pad < 0) {
		vertical_pad = 0;
	} else {
		vertical_pad /= 2;
	}
	
	if (element->_internal_computed_xoffset + item_width < element->parent->_internal_computed_width + element->parent->_internal_computed_xoffset) {
		set_pixel_element_map(	element->_internal_computed_xoffset, \
										element->_internal_computed_yoffset, \
										item_width, \
										element->_internal_computed_height, \
										window, element	);
		fill_rect(	element->_internal_computed_xoffset, \
						element->_internal_computed_yoffset, \
						item_width, \
						element->_internal_computed_height, \
						bg_color, window	);
		draw_text(	element->_internal_computed_xoffset + menuitem->left_padding, \
						element->_internal_computed_yoffset + vertical_pad, \
						0, 0, \
						item_width, \
						element->_internal_computed_height, \
						menuitem->text, \
						text_color, menuitem->fontmap, window	);
	} else {
		if (element->_internal_computed_xoffset >= element->parent->_internal_computed_width + element->parent->_internal_computed_xoffset) {
			goto skip_item_because_state_is_invalid;
		} else {
			signed int useful_width;
			useful_width = element->_internal_computed_width - element->parent->_internal_computed_xoffset;
			set_pixel_element_map(	element->_internal_computed_xoffset, \
											element->_internal_computed_yoffset, \
											useful_width, \
											element->_internal_computed_height, \
											window, element	);
			fill_rect(	element->_internal_computed_xoffset, \
							element->_internal_computed_yoffset, \
							useful_width, \
							element->_internal_computed_height, \
							bg_color, window	);
			useful_width -= menuitem->left_padding;
			if (useful_width > 0) {
				draw_text(	element->_internal_computed_xoffset + menuitem->left_padding, \
								element->_internal_computed_yoffset + vertical_pad, \
								0, 0, \
								useful_width, \
								element->_internal_computed_height, \
								menuitem->text, \
								text_color, menuitem->fontmap, window	);
			}
		}
	}
	
	// Draw SubMenu if applicable
	if (menuitem->is_selected && menuitem->type == MenuT_Branch) {
		// TODO
	}
	
	skip_item_because_state_is_invalid:
	
	return;
}

void draw_menubar(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	struct EL_menubar_t *menubar;
	menubar = element->type_spec;
	
	// Draw Menubar Background
	fill_rect(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					menubar->bg_color, window);
	
	return;
}

unsigned int menubar_event_button(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if        (state == MS_DOWN) {
		element->mouse_state = EL_MS_DOWN;
		draw_element(element, window);
		redraw_required |= 0x1;
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
		redraw_required |= 0x1;
	}
	return redraw_required;
}

unsigned int menubar_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
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

unsigned int menubar_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if (element->mouse_state != EL_MS_NORMAL) {
		element->mouse_state = EL_MS_NORMAL;
		draw_element(window->root_element, window);
		redraw_required |= 0x1;
	}
	return redraw_required;
}
