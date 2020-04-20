#include "./events.h"

void exposure_event(XEvent* event, struct MTK_WinBase* window){
	XWindowAttributes window_attributes_return;
	XGetWindowAttributes(window->dis, window->win, &window_attributes_return);
	if (window->width != window_attributes_return.width || window->height != window_attributes_return.height || window->loop_running == 2) {
		window->width = window_attributes_return.width;
		window->height = window_attributes_return.height;
		free(window->bitmap);
		free(window->mouse_state.pixel_element_map);
		window->bitmap = calloc(window->width * window->height, sizeof(uint32_t));
		window->mouse_state.pixel_element_map = calloc(window->width * window->height, sizeof(struct MTK_WinElement**));
		compute_element_internals(window);
		window->loop_running = 1;
	}
	draw_element(window->root_element, window);
	draw_bm(0, 0, window->width, window->height, window);
	return;
}
void button_event(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinBase* window){
	reset_the_cursor(window);
	draw_element(window->focused_element, window);
	draw_bm(	window->focused_element->_internal_computed_xoffset, \
				window->focused_element->_internal_computed_yoffset, \
				window->focused_element->_internal_computed_width, \
				window->focused_element->_internal_computed_height, \
				window	);
	//draw_element(window->root_element, window);
	//draw_bm(0, 0, window->width, window->height, window);
	return;
}
void key_event(int state, int keycode, XEvent* event, struct MTK_WinBase* window){
	if (state == 2 && keycode == 9) {
		cue_window_close(window, event);
		return;
	}
	if (window->focused_element != 0 && state == 1) {
		if (window->focused_element->type == EL_TEXTBOX) {
			textbox_event_key(state, keycode, event, window->focused_element, window);
			return;
		}
	}
	return;
}
void pointer_motion_event(int x, int y, XEvent* event, struct MTK_WinBase* window){
	if (x < 0 || x >= window->width || y < 0 || y >= window->height) {
		return;
	}
	
	element_mousemotion_event(x, y, event, window);
	return;
}
void leave_window_event(int x, int y, XEvent* event, struct MTK_WinBase* window){
	element_mousemotion_event(x, y, event, window);
	return;
}
int before_closing(struct MTK_WinBase* window, XEvent* event){
	return 0;
}
