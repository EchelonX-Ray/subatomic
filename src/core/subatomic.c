#include "./subatomic.h"

// KeyEvent
// MouseBtnEvent
// MouseMoveEvent
// LeaveEvent
// ExposeEvent
// CloseEvent

int main(int argc, char *argv[]) {
	char *title = "Window Title";
	struct MTK_WinBase window;
	window_struct_init(&window);
	
	window.dis = XOpenDisplay((char *)0);
	window.root_win = DefaultRootWindow(window.dis);
	window.title = title;
	//mtk_vals.key_event = key_event;
	//mtk_vals.mouse_event = mouse_event;
	//mtk_vals.closing_event = close_event;
	//mtk_vals.redraw_event = redraw;
	window.events = mtk_gem(KeyEvent) | mtk_gem(MouseBtnEvent) | mtk_gem(MouseMoveEvent) | mtk_gem(LeaveEvent) | mtk_gem(ExposeEvent);
	window.event_handles[KeyEvent] = (void*)&key_event;
	window.event_handles[MouseBtnEvent] = (void*)&button_event;
	window.event_handles[MouseMoveEvent] = (void*)&pointer_motion_event;
	window.event_handles[LeaveEvent] = (void*)&leave_window_event;
	window.event_handles[ExposeEvent] = (void*)&exposure_event;
	window.width = 640;
	window.height = 480;
	window.bitmap = calloc(window.width * window.height, 4);
	//window.ignore_next_ke = 0;
	//window.ignore_key_repeat = 1;
	create_window(&window);
	
	XEvent event;
	while(window.loop_running == 1) {
		XNextEvent(window.dis, &event);
		event_handler(&window, &event);
	}
	
	free(window.bitmap);
	
	return 100;
}
