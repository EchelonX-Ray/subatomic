#include "./container.h"
#include "./../drawing.h"

void draw_container(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y, signed int computed_width){	
	if (element->width != -1) {
		computed_width = element->width;
	}
	element->computed_x = x;
	element->computed_y = y;
	element->computed_width = computed_width;
	uint32_t color = 0x00000000;
	x += element->x;
	y += element->y;
	set_pixel_element_map(x, y, computed_width, element->height, window, 0);
	fill_rect(x, y, computed_width, element->height, color, window);
	return;
}
