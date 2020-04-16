#include "./textbox.h"
#include "./../drawing.h"
#include "./../text.h"
#include "./../../toolbox/cstr_manip.h"
#include <stdlib.h>

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
	
	type_spec = element->type_spec;
	if (type_spec == 0) {
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
		if (def_text == 0) {
			def_text = "";
		}
		if (text == 0) {
			def_text = type_spec->def_text;
		} else if(text[0] == 0) {
			def_text = type_spec->def_text;
		}
		draw_text(x + 2, y + 2, width - 4, height - 4, def_text, def_text_color, fontmap, window);
		if (focus == element && cursor_blink == 1 && cstrlen(text) > cursor_position) {
			char *tmp_text;
			unsigned int tmp_width;
			tmp_text = malloc(sizeof(char) * cstrlen(text));
			cstrcpy(text, tmp_text);
			tmp_text[cursor_position] = 0;
			tmp_width = draw_text(x + 2, y + 2, width - 4, height - 4, tmp_text, text_color, fontmap, window);
			free(tmp_text);
			draw_text(x + tmp_width + 2, y + 2, width - 4, height - 4, text + cursor_position, text_color, fontmap, window);
			unsigned int cursor_width = 1;
			unsigned int cursor_height = get_char_height('A', fontmap);
			if (cursor_width > width - 4) {
				cursor_width = width - 4;
			}
			if (cursor_height > height - 4) {
				cursor_height = height - 4;
			}
			fill_rect(x + tmp_width + 2, y + 2, cursor_width, cursor_height, type_spec->text_color, window);
		} else {
			draw_text(x + 2, y + 2, width - 4, height - 4, text, text_color, fontmap, window);
		}
	}
	return;
}

void textbox_event_key(int state, int keycode, XEvent* event, struct MTK_WinBase* window) {
	KeySym keysym;
	KeySym keysym_num;
	unsigned int num_mask;
	KeySym keysym_lower;
	KeySym keysym_upper;
	keysym = XkbKeycodeToKeysym(window->dis, keycode, 0, event->xkey.state & ShiftMask);
	num_mask = 0;
	num_mask |= event->xkey.state & ShiftMask;
	if ((event->xkey.state & Mod2Mask) > 0) {
		num_mask ^= ShiftMask;
	}
	keysym_num = XkbKeycodeToKeysym(window->dis, keycode, 0, num_mask);
	char payload[2];
	payload[1] = 0;
	struct EL_textbox_t *textbox_type_spec_ptr;
	textbox_type_spec_ptr = (struct EL_textbox_t*)window->focused_element->type_spec;
	if (keysym == XK_BackSpace) {
		if (textbox_type_spec_ptr->cursor_position > 0) {
			cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, -1);
			textbox_type_spec_ptr->cursor_position--;
		}
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Delete) {
		cdelstr(&(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position, 1);
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Left) {
		if (textbox_type_spec_ptr->cursor_position > 0) {
			textbox_type_spec_ptr->cursor_position--;
		}
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
	}
	if (keysym == XK_Right) {
		if (textbox_type_spec_ptr->cursor_position + 1 < cstrlen(textbox_type_spec_ptr->text)) {
			textbox_type_spec_ptr->cursor_position++;
		}
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
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
		textbox_type_spec_ptr->cursor_position++;
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
	} else if((keysym_num >= 0xffaa && keysym_num <= 0xffb9) || keysym_num == 0xffbd) {
		payload[0] = keysym_num - 0xff80;
		cinsstr(payload, &(textbox_type_spec_ptr->text), textbox_type_spec_ptr->cursor_position);
		textbox_type_spec_ptr->cursor_position++;
		reset_the_cursor(window);
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
	}
	return;
}
