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
	struct EL_button_t type_spec_default;
	uint32_t color;
	uint32_t text_color;
	uint32_t border_color;
	signed int x;
	signed int y;
	signed int width;
	signed int height;
	signed int text_centering_x_offset;
	signed int text_centering_y_offset;
	x = element->_internal_computed_xoffset;
	y = element->_internal_computed_yoffset;
	width = element->_internal_computed_width;
	height = element->_internal_computed_height;
	type_spec = 0;
	type_spec = element->type_spec;
	if (type_spec == 0) {
		type_spec_default.color = 0xAAFFFFFF;
		type_spec_default.hover_color = 0x00FFFFFF;
		type_spec_default.down_color = 0x0000FF00;
		type_spec_default.text_color = 0x00000000;
		type_spec_default.text_hover_color = 0x000000FF;
		type_spec_default.text_down_color = 0x00000000;
		type_spec_default.border_color = 0x00FF0000;
		type_spec_default.text = "Default Text";
		type_spec_default.fontmap = 0;
		type_spec = &type_spec_default;
	}
	color = type_spec->color;
	text_color = type_spec->text_color;
	border_color = type_spec->border_color;
	if (element->mouse_state == EL_MS_HOVER) {
		color = type_spec->hover_color;
		text_color = type_spec->text_hover_color;
	}
	if (element->mouse_state == EL_MS_DOWN) {
		color = type_spec->down_color;
		text_color = type_spec->text_down_color;
	}
	set_pixel_element_map(x, y, width, height, window, element);
	fill_rect(x, y, width, height, color, window);
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
