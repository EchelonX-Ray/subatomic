#include "./drawing.h"

void fill_rect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, uint32_t color, struct MTK_WinBase *window){
	int pixel_x = x;
	int pixel_y = y;
	
	// Convert width and height to the maximum boundaries of the rectangle to draw.
	width += x;
	height += y;
	
	// Alpha Channel Blending
	unsigned int alpha         = 0xFF & (color >>  24);
	unsigned int red           = 0xFF & (color >>  16);
	unsigned int green         = 0xFF & (color >>   8);
	unsigned int blue          = 0xFF & (color >>   0);
	unsigned int alpha_inverse = 0xFF & (0xFF - alpha);
	uint32_t working_color;
	red   = (red   * alpha_inverse) / 0xFF;
	green = (green * alpha_inverse) / 0xFF;
	blue  = (blue  * alpha_inverse) / 0xFF;
	color = 0;
	color |= (0xFF &   red) << 16;
	color |= (0xFF & green) <<  8;
	color |= (0xFF &  blue) <<  0;
	
	// Draw the rectangle
	while (pixel_y < height && pixel_y < window->height) {
		pixel_x = x;
		while (pixel_x < width && pixel_x < window->height) {
			// Alpha Channel Blending
			working_color = window->bitmap[pixel_y * window->width + pixel_x];
			red   = 0xFF & (working_color >>  16);
			green = 0xFF & (working_color >>   8);
			blue  = 0xFF & (working_color >>   0);
			red   = (red   * alpha) / 0xFF;
			green = (green * alpha) / 0xFF;
			blue  = (blue  * alpha) / 0xFF;
			working_color = 0;
			working_color |= (0xFF &   red) << 16;
			working_color |= (0xFF & green) <<  8;
			working_color |= (0xFF &  blue) <<  0;
			working_color += color;
			
			// Set the pixel
			window->bitmap[pixel_y * window->width + pixel_x] = working_color;
			
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
