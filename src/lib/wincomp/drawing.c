#include "./drawing.h"

void fill_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint32_t color, struct MTK_WinBase *window){
	int pixel_x = x;
	int pixel_y = y;
	
	// Convert width and height to the maximum boundaries of the rectangle to draw.
	width += x;
	height += y;
	
	// Draw the rectangle
	while (pixel_y < height && pixel_y < window->height) {
		pixel_x = x;
		while (pixel_x < width && pixel_x < window->height) {
			window->bitmap[pixel_y * window->width + pixel_x] = color;
			pixel_x++;
		}
		pixel_y++;
	}
	return;
}

void draw_bm(unsigned int x, unsigned int y, unsigned int width, unsigned int height, struct MTK_WinBase *window){
	int pixel_x = x;
	int pixel_y = y;
	
	// Convert width and height to the maximum boundaries of the rectangle to draw.
	width += x;
	height += y;
	
	// Draw the rectangle
	while (pixel_y < height && pixel_y < window->height) {
		pixel_x = x;
		while (pixel_x < width && pixel_x < window->height) {
			XSetForeground(window->dis, window->gc, window->bitmap[pixel_y * window->width + pixel_x]);
			XFillRectangle(window->dis, window->win, window->gc, pixel_x, pixel_y, 1, 1);
			pixel_x++;
		}
		pixel_y++;
	}
	return;
}
