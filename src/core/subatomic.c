#include "./subatomic.h"
#include "./win_setup/main_win.c"

int main(int argc, char *argv[]) {
	struct MTK_WinFontPack font_pack;
	setup_font("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", 25, &font_pack);
	
	struct MTK_WinElement *root_cont = calloc(1, sizeof(struct MTK_WinElement));
	setup_main_win_elements(&root_cont, &font_pack);
	
	struct MTK_WinBase window;
	window_struct_init(&window);
	window.dis = XOpenDisplay((char *)0);
	window.root_win = DefaultRootWindow(window.dis);
	window.title = "Window Title";
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
	window.mouse_state.pixel_element_map = calloc(window.width * window.height, sizeof(struct MTK_WinElement**));
	window.root_element = root_cont;
	window.ignore_key_repeat = 0; // Allow Key Repeat
	
	create_window(&window);
	
	compute_element_internals(&window);
	draw_element(root_cont, &window);
	draw_bm(0, 0, 640, 480, &window);
		
	XEvent event;
	while(window.loop_running == 1) {
		XNextEvent(window.dis, &event);
		event_handler(&window, &event);
	}
	
	//TODO: Clean Window Exit
	//XDestroySubwindows(window.dis, window.win); -- Causes Errors, need to do more research
	//XDestroyWindow(window.dis, window.win); -- Causes Errors, need to do more research
	//XCloseDisplay(window.dis); -- Causes Errors, need to do more research
	
	free(window.mouse_state.pixel_element_map);
	free(window.bitmap);
	free_element_tree(root_cont);
	free_font(&font_pack);
	
	return 0;
}
