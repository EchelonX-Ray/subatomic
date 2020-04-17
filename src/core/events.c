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
	//printf("Button Event: State-%d", state);
	//printf(" button-%d ", button);
	//printf(" x-%d ", x);
	//printf(" y-%d\n", y);
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
	
	struct MTK_WinElement *element;
	struct MTK_WinElement *previous_element;
	unsigned int redraw_required = 0;
	element = window->mouse_state.pixel_element_map[y * window->width + x];
	previous_element = window->mouse_state.previous_mouse_element;
	if (previous_element != 0 && previous_element != element) {
		if (previous_element->mouse_state != EL_MS_NORMAL) {
			previous_element->mouse_state = EL_MS_NORMAL;
			redraw_required = 1;
		}
		window->mouse_state.previous_mouse_element = 0;
	}
	if (element != 0 && window->mouse_state.mouse_state != MS_DOWN) {
		if (element->mouse_state != EL_MS_HOVER) {
			element->mouse_state = EL_MS_HOVER;
			redraw_required = 1;
		}
		window->mouse_state.previous_mouse_element = element;
	}
	window->mouse_state.previous_mouse_x = x;
	window->mouse_state.previous_mouse_y = y;
	if (redraw_required == 1) {
		draw_element(window->root_element, window);
		draw_bm(0, 0, 640, 480, window);
	}
	return;
}
void leave_window_event(int x, int y, XEvent* event, struct MTK_WinBase* window){
	struct MTK_WinElement *previous_element;
	previous_element = window->mouse_state.previous_mouse_element;
	if (previous_element != 0) {
		if (previous_element->mouse_state != EL_MS_NORMAL) {
			previous_element->mouse_state = EL_MS_NORMAL;
			draw_element(window->root_element, window);
			draw_bm(0, 0, 640, 480, window);
		}
		window->mouse_state.previous_mouse_element = 0;
	}
	//printf("Leave Event: x-%d", x);
	//printf(" y-%d\n", y);
}
int before_closing(struct MTK_WinBase* window, XEvent* event){
	return 0;
}
