#include "./textbox.h"
#include "./../drawing.h"
#include "./../text.h"
#include "./../input/mouse.h"
#include "./../../toolbox/cstr_manip.h"
#include <stdlib.h>
#include <X11/cursorfont.h>

/*
struct EL_textbox_t {
	uint32_t bg_color;
	uint32_t text_color;
	uint32_t def_text_color;
	uint32_t border_color;
	char *text;
	char *def_text;
	struct MTK_WinFontMap *fontmap;
};
*/

void move_textbox_cursor(struct MTK_WinElement *element, signed int offset, unsigned int dont_move_sel_pos) {
	if (element == 0) { // Probably unnecessary error checking / Consider removing
		return;
	}
	if (element->type_spec == 0) { // Probably unnecessary error checking / Consider removing
		return;
	}
	
	struct EL_textbox_t *type_spec;
	unsigned int text_length;
	unsigned int cursor_position;
	
	type_spec = element->type_spec;
	if (type_spec->text == 0) { // Probably unnecessary error checking / Consider removing
		return;
	}
	
	text_length = cstrlen(type_spec->text);
	cursor_position = type_spec->cursor_position + offset;
	
	// Check to make sure the desired cursor_position does not overflow the bounderies of the text.
	// Because of unsigned Two's complement integer wrapping, this check will also work if offset is negative and has a greater absolute value than text_length
	// So it is equivalent to a signed operation of ((cursor_position > text_length) && (cursor_position < 0))
	if (cursor_position < text_length) {
		// Move the cursor
		type_spec->cursor_position = cursor_position;
		if (!dont_move_sel_pos) { // Should the selection base position be moved in sync?
			type_spec->cursor_selection_base_position = cursor_position;
		}
		
		// Does the new cursor position exist outside of the current textbox display window?
		// If so, adjust the display offset so that the cursor is displayed in the visible textbox
		char *tmp_text;
		unsigned int tmp_width_cursor;
		unsigned int tmp_width_text;
		unsigned int tmp_max_width;
		tmp_max_width = element->_internal_computed_width;
		if (tmp_max_width <= 4) {
			tmp_max_width = 0;
		} else {
			tmp_max_width -= 4;
		}
		tmp_text = malloc(sizeof(char) * text_length);
		cstrcpy(type_spec->text, tmp_text);
		tmp_text[cursor_position] = 0;
		tmp_width_cursor = get_text_width(tmp_text, type_spec->fontmap);
		tmp_width_text = get_text_width(type_spec->text, type_spec->fontmap);
		free(tmp_text);
		if (tmp_width_text <= tmp_max_width) {
			type_spec->text_drawing_offset = 0;
		} else {
			if (type_spec->text_drawing_offset + tmp_max_width > tmp_width_text) {
				type_spec->text_drawing_offset = tmp_width_text - tmp_max_width;
			}
			if (tmp_width_cursor < type_spec->text_drawing_offset) {
				if (tmp_width_cursor == 0) {
					type_spec->text_drawing_offset = tmp_width_cursor;
				} else {
					type_spec->text_drawing_offset = tmp_width_cursor - 1;
				}
			} else if (tmp_width_cursor > type_spec->text_drawing_offset + tmp_max_width) {
				type_spec->text_drawing_offset = tmp_width_cursor - tmp_max_width;
			}
		}
	}
	return;
}

void draw_textbox(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	struct EL_textbox_t *type_spec;
	struct MTK_WinFontMap *fontmap;
	struct MTK_WinElement *focus;
	char *text;
	char *def_text;
	uint32_t bgcolor;
	uint32_t text_color;
	uint32_t def_text_color;
	uint32_t border_color;
	signed int x;
	signed int y;
	signed int width;
	signed int height;
	unsigned char cursor_blink;
	unsigned int cursor_position;
	unsigned int cursor_selection_base_position;
	unsigned int text_drawing_offset;
	
	type_spec = element->type_spec;
	if (type_spec == 0) { // Probably unnecessary error checking / Consider removing
		return;
	}
	text = type_spec->text;
	def_text = "";
	bgcolor = type_spec->bg_color;
	text_color = type_spec->text_color;
	def_text_color = type_spec->def_text_color;
	border_color = type_spec->border_color;
	fontmap = type_spec->fontmap;
	cursor_position = type_spec->cursor_position;
	cursor_selection_base_position = type_spec->cursor_selection_base_position;
	text_drawing_offset = type_spec->text_drawing_offset;
	focus = window->focused_element;
	cursor_blink = window->cursor_blink;
	x = element->_internal_computed_xoffset;
	y = element->_internal_computed_yoffset;
	width = element->_internal_computed_width;
	height = element->_internal_computed_height;
	
	set_pixel_element_map(x, y, width, height, window, element);
	fill_rect(x, y, width, height, bgcolor, window);
	draw_rect(x, y, width, height, 1, border_color, window);
	
	if (width > 4 && height > 4) {
		if (type_spec->text_drawing_offset) {
			x += 1;
			width -= 2;
		} else {
			x += 2;
			width -= 3;
		}
		y += 2;
		height -= 4;
		
		if (text == 0) {
			def_text = type_spec->def_text;
		} else if(text[0] == 0) {
			def_text = type_spec->def_text;
		}
		draw_text(x, y, 0, 0, width, height, def_text, def_text_color, fontmap, window);
		
		unsigned int cursor_width;
		unsigned int cursor_height;
		unsigned int tmp_text_width;
		unsigned int tmp_width;
		char *tmp_text;
		cursor_width = 1;
		cursor_height = get_char_height('A', fontmap);
		tmp_width = 0;
		tmp_text = malloc(sizeof(char) * cstrlen(text));
		cstrcpy(text, tmp_text);
		if        (cursor_position > cursor_selection_base_position) {
			char tmp_char;
			tmp_char = tmp_text[cursor_selection_base_position];
			tmp_text[cursor_position] = 0;
			tmp_text[cursor_selection_base_position] = 0;
			tmp_width += draw_text(x, y, text_drawing_offset, 0, width, height, tmp_text, text_color, fontmap, window);
			tmp_text_width = get_text_width(tmp_text, fontmap);
			if (tmp_text_width > text_drawing_offset) {
				text_drawing_offset = 0;
			} else {
				text_drawing_offset -= tmp_text_width;
			}
			tmp_text[cursor_selection_base_position] = tmp_char;
			tmp_text_width = get_text_width(tmp_text + cursor_selection_base_position, fontmap);
			if (text_drawing_offset < tmp_text_width) {
				if (x + tmp_width < x + width && y < y + height) {
					if (x + tmp_width + (tmp_text_width - text_drawing_offset) > x + tmp_width + width) {
						fill_rect(x + tmp_width, y, width - (x + tmp_width), cursor_height, (bgcolor & 0xFF000000) | (~bgcolor & 0x00FFFFFF), window);
					} else {
						fill_rect(x + tmp_width, y, tmp_text_width - text_drawing_offset, cursor_height, (bgcolor & 0xFF000000) | (~bgcolor & 0x00FFFFFF), window);
					}
				}
			}
			tmp_width += draw_text(x + tmp_width, y, text_drawing_offset, 0, width, height, tmp_text + cursor_selection_base_position, (text_color & 0xFF000000) | (~text_color & 0x00FFFFFF), fontmap, window);
			if (tmp_text_width > text_drawing_offset) {
				text_drawing_offset = 0;
			} else {
				text_drawing_offset -= tmp_text_width;
			}
			if (focus == element && cursor_blink == 1) {
				if (cursor_width > width) {
					cursor_width = width;
				}
				if (cursor_height > height) {
					cursor_height = height;
				}
				fill_rect(x + tmp_width, y, cursor_width, cursor_height, type_spec->text_color, window);
			}
			draw_text(x + tmp_width, y, text_drawing_offset, 0, width, height, text + cursor_position, text_color, fontmap, window);
		} else if (cursor_position < cursor_selection_base_position) {
			char tmp_char;
			unsigned int tmp_text_width;
			unsigned int tmp_width2;
			tmp_char = tmp_text[cursor_position];
			tmp_text[cursor_position] = 0;
			tmp_text[cursor_selection_base_position] = 0;
			tmp_width += draw_text(x, y, text_drawing_offset, 0, width, height, tmp_text, text_color, fontmap, window);
			tmp_text_width = get_text_width(tmp_text, fontmap);
			if (tmp_text_width > text_drawing_offset) {
				text_drawing_offset = 0;
			} else {
				text_drawing_offset -= tmp_text_width;
			}
			tmp_text[cursor_position] = tmp_char;
			tmp_text_width = get_text_width(tmp_text + cursor_position, fontmap);
			if (text_drawing_offset < tmp_text_width) {
				if (x + tmp_width < x + width && y < y + height) {
					if (x + tmp_width + (tmp_text_width - text_drawing_offset) > x + tmp_width + width) {
						fill_rect(x + tmp_width, y, width - (x + tmp_width), cursor_height, (bgcolor & 0xFF000000) | (~bgcolor & 0x00FFFFFF), window);
					} else {
						fill_rect(x + tmp_width, y, tmp_text_width - text_drawing_offset, cursor_height, (bgcolor & 0xFF000000) | (~bgcolor & 0x00FFFFFF), window);
					}
				}
			}
			tmp_width2 = tmp_width;
			tmp_width += draw_text(x + tmp_width, y, text_drawing_offset, 0, width, height, tmp_text + cursor_position, (text_color & 0xFF000000) | (~text_color & 0x00FFFFFF), fontmap, window);
			if (focus == element && cursor_blink == 1) {
				if (cursor_width > width) {
					cursor_width = width;
				}
				if (cursor_height > height) {
					cursor_height = height;
				}
				fill_rect(x + tmp_width2, y, cursor_width, cursor_height, ((type_spec->text_color) & 0xFF000000) | (~(type_spec->text_color) & 0x00FFFFFF), window);
			}
			if (tmp_text_width > text_drawing_offset) {
				text_drawing_offset = 0;
			} else {
				text_drawing_offset -= tmp_text_width;
			}
			cursor_position = cursor_selection_base_position;
			draw_text(x + tmp_width, y, text_drawing_offset, 0, width, height, text + cursor_position, text_color, fontmap, window);
		} else {
			tmp_text[cursor_position] = 0;
			tmp_width += draw_text(x, y, text_drawing_offset, 0, width, height, tmp_text, text_color, fontmap, window);
			text_drawing_offset = 0;
			draw_text(x + tmp_width, y, text_drawing_offset, 0, width, height, text + cursor_position, text_color, fontmap, window);
			if (focus == element && cursor_blink == 1) {
				if (cursor_width > width) {
					cursor_width = width;
				}
				if (cursor_height > height) {
					cursor_height = height;
				}
				fill_rect(x + tmp_width, y, cursor_width, cursor_height, type_spec->text_color, window);
			}
		}
		free(tmp_text);
	}
	return;
}

unsigned int textbox_leave(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	redraw_required = 0;
	if (element->mouse_state != EL_MS_NORMAL) {
		element->mouse_state = EL_MS_NORMAL;
		draw_textbox(element, window);
		redraw_required |= 0x1;
	}
	return redraw_required;
}
unsigned int textbox_event_move(int x, int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	unsigned int redraw_required;
	struct EL_textbox_t *type_spec;
	redraw_required = 0;
	type_spec = element->type_spec;
#ifndef DEVEL_STRIP_MCURSOR
	if (window->_internal_cursor_index != CS_Text) {
		window->_internal_cursor_index = CS_Text;
		XDefineCursor(window->dis, window->win, window->_internal_cursor[CS_Text]);
	}
#endif
	if (window->_internal_mouse_state.mouse_state == MS_DOWN) {
		// The pointer has moved and is button-[TODO]-down, but the location could be anywhere, including outside of the window.
		// Use it's location to determine a new text cursor position and set it.
		
		// Declare local-scope variables
		signed int x_pos;
		unsigned int i;
		
		// We need to determine where the textbox is starting the lodgical drawing of it's text
		x_pos = 2 + element->_internal_computed_xoffset - type_spec->text_drawing_offset;
		
		// Do this because the starting x-position where the text is drawn inside the 
		// textbox changes by 1 pixel depending on if the text_drawing_offset is non-zero
		if (type_spec->text_drawing_offset) {
			x_pos -= 1;
		}
		
		// Find the charactor that crosses the pointer's x coordinate
		i = 0;
		while(x_pos < x && type_spec->text[i] != 0) {
			x_pos += get_char_width(type_spec->text[i], type_spec->fontmap);
			i++;
		}
		
		// Verify that the loop ran at least one time
		if (i) {
			if (x_pos < x) {
				// The while loop ran out of charactors before crossing this threashold, set the cursor to the end of the text
				type_spec->cursor_position = cstrlen(type_spec->text) - 1;
			} else {
				// Determine if the pointer is on the left or right side of the character
				i--;
				x_pos -= get_char_width(type_spec->text[i], type_spec->fontmap) / 2;
				if (x < x_pos) {
					// The pointer is on the left side of this charactor.  The cursor should be set before this charactor.
					type_spec->cursor_position = i;
				} else {
					// The pointer is on the right side of this charactor.  The cursor should be set after this charactor.
					type_spec->cursor_position = i + 1;
				}
			}
		} else {
			// If the loop didn't run, ether:
			//  - There is no text content
			// OR
			//  - The pointer's x coordinate is at or before the start of the text
			// Ether way, the cursor position should be set to the start of the text
			type_spec->cursor_position = 0;
		}
		
		draw_textbox(element, window); // Redraw the element to the bitmap
		redraw_required |= 0x1; // Set the bit flags to indicate that the region of the bitmap occupied by this textbox should be redrawn to the screen.
	}
	return redraw_required;
}
unsigned int textbox_event_button(unsigned int state, unsigned int button, signed int x, signed int y, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	// Declare function-scope variables
	unsigned int redraw_required;
	struct EL_textbox_t *type_spec;
	redraw_required = 0; // Zero the bits of this variable
	type_spec = element->type_spec; // Get the element's type-specific values
	
	if (button == MOUSE_BTN_LEFT) {
		if (state == MS_DOWN) {
			if (  x >= element->_internal_computed_xoffset && 
			      x  < element->_internal_computed_xoffset + element->_internal_computed_width && 
			      y >= element->_internal_computed_yoffset && 
			      y  < element->_internal_computed_yoffset + element->_internal_computed_height  ) {
				// The pointer is button-left-down inside the textbox.
				// Use it's location to determine a new text cursor_position and set it.
				// Also use it's location to determine a new text cursor_selection_base_position and set it.
				
				// Declare local-scope variables
				signed int x_pos;
				unsigned int i;
				
				// Is the textbox currently in focus?
				if (element == window->focused_element) {
					// If so, we need to determine where the textbox is starting the lodgical drawing of it's text
					x_pos = 2 + element->_internal_computed_xoffset - type_spec->text_drawing_offset;
					
					// Do this because the starting x-position where the text is drawn inside the 
					// textbox changes by 1 pixel depending on if the text_drawing_offset is non-zero
					if (type_spec->text_drawing_offset) {
						x_pos -= 1;
					}
				} else {
					// If not, the textbox will default to drawing text at 2 pixels + it's x corrdinate position
					// The textbox should also be set in-focus
					x_pos = 2 + element->_internal_computed_xoffset;
					window->focused_element = element;
				}
				
				// Find the charactor that crosses the pointer's x coordinate
				i = 0;
				while(x_pos < x && type_spec->text[i] != 0) {
					x_pos += get_char_width(type_spec->text[i], type_spec->fontmap);
					i++;
				}
				
				// Verify that the loop ran at least one time
				if (i) {
					if (x_pos < x) {
						// The while loop ran out of charactors before crossing this threashold, set the cursor to the end of the text
						type_spec->cursor_position = cstrlen(type_spec->text) - 1;
						type_spec->cursor_selection_base_position = cstrlen(type_spec->text) - 1;
					} else {
						// Determine if the pointer is on the left or right side of the character
						i--;
						x_pos -= get_char_width(type_spec->text[i], type_spec->fontmap) / 2;
						if (x < x_pos) {
							// The pointer is on the left side of this charactor.  The cursor should be set before this charactor.
							type_spec->cursor_position = i;
							type_spec->cursor_selection_base_position = i;
						} else {
							// The pointer is on the right side of this charactor.  The cursor should be set after this charactor.
							type_spec->cursor_position = i + 1;
							type_spec->cursor_selection_base_position = i + 1;
						}
					}
				} else {
					// If the loop didn't run, ether:
					//  - There is no text content
					// OR
					//  - The pointer's x coordinate is at or before the start of the text
					// Ether way, the cursor position should be set to the start of the text
					type_spec->cursor_position = 0;
					type_spec->cursor_selection_base_position = 0;
				}
				
				stop_the_cursor(window, 1); // Stop the cursor blink loop so that the cursor is always displayed while the mouse button is down.
				
				draw_textbox(element, window); // Redraw the element to the bitmap
				redraw_required |= 0x1; // Set the bit flags to indicate that the region of the bitmap occupied by this textbox should be redrawn to the screen.
			}
		} else if (state == MS_UP) {
			// The pointer is button-left-up, but it's location could be anywhere, including outside of the window.
			start_the_cursor(window, 1); // Restart the cursor blink loop.
			
			draw_textbox(element, window); // Redraw the element to the bitmap
			redraw_required |= 0x1; // Set the bit flags to indicate that the region of the bitmap occupied by this textbox should be redrawn to the screen.
		}
	}
	return redraw_required;
}
void textbox_event_key(int state, int keycode, XEvent* event, struct MTK_WinElement* element, struct MTK_WinBase* window) {
	KeySym keysym;
	KeySym keysym_num;
	unsigned int num_mask;
	KeySym keysym_lower;
	KeySym keysym_upper;
	keysym = XkbKeycodeToKeysym(window->dis, keycode, 0, event->xkey.state & ShiftMask);
	num_mask = 0;
	num_mask |= event->xkey.state & ShiftMask;
	if (event->xkey.state & Mod2Mask) {
		num_mask ^= ShiftMask;
	}
	keysym_num = XkbKeycodeToKeysym(window->dis, keycode, 0, num_mask);
	char payload[2];
	payload[1] = 0;
	struct EL_textbox_t *textbox_type_spec_ptr;
	textbox_type_spec_ptr = (struct EL_textbox_t*)window->focused_element->type_spec;
	if (keysym == XK_BackSpace) {
		if (textbox_type_spec_ptr->cursor_position != textbox_type_spec_ptr->cursor_selection_base_position) {
			if (textbox_type_spec_ptr->cursor_position > textbox_type_spec_ptr->cursor_selection_base_position) {
				cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position);
				move_textbox_cursor(element, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position, 0);
			} else {
				cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position);
				move_textbox_cursor(element, 0, 0); // Call this even though the cursor is not moving because it will adjust the display offsets if necessary
			}
		} else {
			cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, -1);
			move_textbox_cursor(element, -1, 0);
		}
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Delete) {
		if (textbox_type_spec_ptr->cursor_position != textbox_type_spec_ptr->cursor_selection_base_position) {
			if (textbox_type_spec_ptr->cursor_position > textbox_type_spec_ptr->cursor_selection_base_position) {
				cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position);
				move_textbox_cursor(element, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position, 0);
			} else {
				cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position);
				move_textbox_cursor(element, 0, 0); // Call this even though the cursor is not moving because it will adjust the display offsets if necessary
			}
		} else {
			cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, 1);
			move_textbox_cursor(element, 0, 0); // Call this even though the cursor is not moving because it will adjust the display offsets if necessary
		}
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Left) {
		if (event->xkey.state & ShiftMask) {
			move_textbox_cursor(element, -1, 1);
		} else {
			if (textbox_type_spec_ptr->cursor_selection_base_position != textbox_type_spec_ptr->cursor_position) {
				if (textbox_type_spec_ptr->cursor_selection_base_position > textbox_type_spec_ptr->cursor_position) {
					move_textbox_cursor(element, 0, 0);
				} else {
					move_textbox_cursor(element, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position, 0);
				}
			} else {
				move_textbox_cursor(element, -1, 0);
			}
		}
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Right) {
		if (event->xkey.state & ShiftMask) {
			move_textbox_cursor(element, +1, 1);
		} else {
			if (textbox_type_spec_ptr->cursor_selection_base_position != textbox_type_spec_ptr->cursor_position) {
				if (textbox_type_spec_ptr->cursor_selection_base_position > textbox_type_spec_ptr->cursor_position) {
					move_textbox_cursor(element, textbox_type_spec_ptr->cursor_selection_base_position - textbox_type_spec_ptr->cursor_position, 0);
				} else {
					move_textbox_cursor(element, 0, 0);
				}
			} else {
				move_textbox_cursor(element, +1, 0);
			}
		}
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	}
	if ((event->xkey.state & LockMask) > 0) {
		XConvertCase(keysym, &keysym_lower, &keysym_upper);
		if ((event->xkey.state & ShiftMask) > 0) {
			keysym = keysym_lower;
		} else {
			keysym = keysym_upper;
		}
	}
	if (keysym >= 0x20 && keysym < 0x7F) {
		payload[0] = keysym;
		cinsstr(payload, &(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position);
		move_textbox_cursor(element, +1, 0);
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	} else if((keysym_num >= 0xffaa && keysym_num <= 0xffb9) || keysym_num == 0xffbd) {
		payload[0] = keysym_num - 0xff80;
		cinsstr(payload, &(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position);
		move_textbox_cursor(element, +1, 0);
		reset_the_cursor(window, 1);
		draw_element(element, window);
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
		//draw_element(window->root_element, window);
		//draw_bm(0, 0, window->width, window->height, window);
	}
	return;
}
