#include "./button.h"
#include "./../drawing.h"

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y) {
	uint32_t color = 0xAAFFFFFF;
	if (element->mouse_state == EL_MS_HOVER) {
		color = 0x00FFFFFF;
	}
	if (element->mouse_state == EL_MS_DOWN) {
		color = 0x0000FF00;
	}
	//x += element->x;
	//y += element->y;
	set_pixel_element_map(element->_internal_computed_xoffset, element->_internal_computed_yoffset, element->_internal_computed_width, element->_internal_computed_height, window, element);
	fill_rect(element->_internal_computed_xoffset, element->_internal_computed_yoffset, element->_internal_computed_width, element->_internal_computed_height, color, window);
	return;
}
