#include "./container.h"
#include "./../drawing.h"

void draw_container(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y){
	uint32_t color = 0x00000000;
	x += element->x;
	y += element->y;
	set_pixel_element_map(x, y, element->_internal_computed_width, element->_internal_computed_height, window, 0);
	fill_rect(x, y, element->_internal_computed_width, element->_internal_computed_height, color, window);
	return;
}
