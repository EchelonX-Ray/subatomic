#include "./subatomic.h"
#include "./win_setup/main_win.c"
#include "./pthreading.h"

int main(int argc, char *argv[]) {
	struct MTK_WinFontPack font_pack;
	setup_font("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", 25, &font_pack);
	
	struct MTK_WinElement *root_cont = calloc(1, sizeof(struct MTK_WinElement));
	setup_main_win_elements(&root_cont, &font_pack);
	XInitThreads();
	
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
	
	pthread_t threads[1];
	struct MTK_Thread_Param thread_param[1];
	thread_param[0].lock = &(thread_param[0].lock_val);
	pthread_mutex_init(thread_param[0].lock, NULL);
	thread_param[0].window = &window;
	
	pthread_create(threads + 0, 0, thread0_entry_blink_cursor, thread_param + 0);
	
	create_window(&window);
	
	pthread_mutex_lock(thread_param[0].lock);
	compute_element_internals(&window);
	draw_element(root_cont, &window);
	draw_bm(0, 0, 640, 480, &window);
	pthread_mutex_unlock(thread_param[0].lock);
		
	XEvent event;
	while(window.loop_running == 1) {
		XNextEvent(window.dis, &event);
		pthread_mutex_lock(thread_param[0].lock);
		event_handler(&window, &event);
		pthread_mutex_unlock(thread_param[0].lock);
	}
	
	//TODO: Clean Window Exit
	//XDestroySubwindows(window.dis, window.win); -- Causes Errors, need to do more research
	//XDestroyWindow(window.dis, window.win); -- Causes Errors, need to do more research
	//XCloseDisplay(window.dis); -- Causes Errors, need to do more research
	
	pthread_cancel(threads[0]);
	pthread_join(threads[0], 0);
	
	free(window.mouse_state.pixel_element_map);
	free(window.bitmap);
	free_element_tree(root_cont);
	free_font(&font_pack);
	
	return 0;
}
