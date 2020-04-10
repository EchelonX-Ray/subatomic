#include "./container.h"
#include "./../drawing.h"

void draw_container(struct MTK_WinElement *element, struct MTK_WinBase *window){
	uint32_t color = 0x00000000;
	set_pixel_element_map(element->_internal_computed_xoffset, element->_internal_computed_yoffset, element->_internal_computed_width, element->_internal_computed_height, window, 0);
	fill_rect(element->_internal_computed_xoffset, element->_internal_computed_yoffset, element->_internal_computed_width, element->_internal_computed_height, color, window);
	return;
}
