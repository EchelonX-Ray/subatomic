#include "./subatomic.h"

// KeyEvent
// MouseBtnEvent
// MouseMoveEvent
// LeaveEvent
// ExposeEvent
// CloseEvent

int main(int argc, char *argv[]) {
	char *title = "Window Title";
	struct MTK_WinFontPack font_pack;
	setup_font("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", 25, &font_pack);
	
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
	window.event_handles[CloseEvent] = (void*)&before_closing;
	window.width = 640;
	window.height = 480;
	window.bitmap = calloc(window.width * window.height, sizeof(uint32_t));
	window.ignore_key_repeat = 0; // Allow Key Repeat
	create_window(&window);
	
	fill_rect(200, 200, 100, 50, 0x0000FF00, &window);
	unsigned int x = 50;
	printf("x: %d\n", x);
	x += draw_char(x, 50, 'A', 0x00FFFFFF, font_pack.font_style, &window);
	printf("x: %d\n", x);
	x += draw_char(x, 50, 'B', 0x00FFFFFF, font_pack.font_style, &window);
	printf("x: %d\n", x);
	x += draw_char(x, 50, 'C', 0x00FFFFFF, font_pack.font_style, &window);
	printf("x: %d\n", x);
	fill_rect(50, 50, 1, 1, 0x00FF0000, &window);
	draw_bm(0, 0, 640, 480, &window);
	
	XEvent event;
	while(window.loop_running == 1) {
		XNextEvent(window.dis, &event);
		event_handler(&window, &event);
	}
	
	free_font(&font_pack);
	free(window.bitmap);
	
	return 0;
}
