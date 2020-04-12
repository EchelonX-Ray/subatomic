#include "./events.h"

void exposure_event(XEvent* event, struct MTK_WinBase* window){
	//printf("Expose Event.\n");
}
void button_event(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinBase* window){
	//printf("Button Event: State-%d", state);
	//printf(" button-%d ", button);
	//printf(" x-%d ", x);
	//printf(" y-%d\n", y);
	draw_bm(0, 0, 640, 480, window);
}
void key_event(int state, int keycode, XEvent* event, struct MTK_WinBase* window){
	if (state == 2 && keycode == 9) {
		cue_window_close(window, event);
	} else {
		//printf("Key Event: keycode-%d", keycode);
		//printf(" state-%d\n", state);
	}
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
