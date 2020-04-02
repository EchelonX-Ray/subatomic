#include "./button.h"
#include "./../drawing.h"

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y, signed int computed_width) {
	//printf("BUTTON: x-%d", x);
	//printf(" element->x-%d", element->x);
	//printf(" y-%d", y);
	//printf(" element->y-%d", element->y);
	//printf(" computed_width-%d", computed_width);
	//printf(" element->width-%d\n", element->width);
	if (element->width != -1) {
		computed_width = element->width;
	}
	element->computed_x = x;
	element->computed_y = y;
	element->computed_width = computed_width;
	uint32_t color = 0xAAFFFFFF;
	if (element->mouse_state == EL_MS_HOVER) {
		color = 0x00FFFFFF;
	}
	if (element->mouse_state == EL_MS_DOWN) {
		color = 0x0000FF00;
	}
	x += element->x;
	y += element->y;
	set_pixel_element_map(x, y, computed_width, element->height, window, element);
	fill_rect(x, y, computed_width, element->height, color, window);
	return;
}
