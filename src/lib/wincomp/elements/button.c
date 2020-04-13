#include "./button.h"
#include "./../drawing.h"
#include "./../text.h"

/*
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
*/

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	struct EL_button_t *type_spec;
	uint32_t bgcolor;
	uint32_t text_color;
	uint32_t border_color;
	signed int x;
	signed int y;
	signed int width;
	signed int height;
	signed int text_centering_x_offset;
	signed int text_centering_y_offset;
	
	type_spec = element->type_spec;
	if (type_spec == 0) {
		return;
	}
	x = element->_internal_computed_xoffset;
	y = element->_internal_computed_yoffset;
	width = element->_internal_computed_width;
	height = element->_internal_computed_height;
	bgcolor = type_spec->bg_color;
	text_color = type_spec->text_color;
	border_color = type_spec->border_color;
	if (element->mouse_state == EL_MS_HOVER) {
		bgcolor = type_spec->bg_hover_color;
		text_color = type_spec->text_hover_color;
	}
	if (element->mouse_state == EL_MS_DOWN) {
		bgcolor = type_spec->bg_down_color;
		text_color = type_spec->text_down_color;
	}
	
	set_pixel_element_map(x, y, width, height, window, element);
	fill_rect(x, y, width, height, bgcolor, window);
	draw_rect(x, y, width, height, 1, border_color, window);
	
	text_centering_x_offset = ((signed int)width - (signed int)(get_text_width(type_spec->text, type_spec->fontmap))) / 2 - 1;
	text_centering_y_offset = ((signed int)height - (signed int)(type_spec->fontmap->max_height)) / 2 - 1;
	if (text_centering_x_offset < 0) {
		text_centering_x_offset = 0;
	}
	if (text_centering_y_offset < 0) {
		text_centering_y_offset = 0;
	}
	
	if(width - 2 >= 0 && height - 2 >= 0) {
		draw_text(x + text_centering_x_offset + 1, y + text_centering_y_offset + 1, width - 2, height - 2, type_spec->text, text_color, type_spec->fontmap, window);
	}
	return;
}
