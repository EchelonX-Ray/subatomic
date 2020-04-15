#include "./subatomic.h"

#include <poll.h>
#include <unistd.h>

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
	window.focused_element = root_cont->children[1];
	
	compute_element_internals(&window);
	
	create_window(&window);
	
	int pipefd_pair[2];
	if(pipe(pipefd_pair) != 0){
		return -1;
	}
	pthread_t threads[1];
	pthread_mutex_t thread_lock[1];
	window.thread.thread = threads + 0;
	window.thread.lock = thread_lock + 0;
	window.thread.fd = pipefd_pair[1];
	window.thread.millisec_increment = 1000;
	pthread_mutex_init(window.thread.lock, NULL);
	pthread_mutex_lock(window.thread.lock);
	
	char charbuf;
	struct pollfd fds[2];
	fds[0].fd = window.fd;
	fds[0].events = POLLIN;
	fds[1].fd = pipefd_pair[0];
	fds[1].events = POLLIN;
	int poll_ret;
	
	pthread_create(window.thread.thread, 0, blink_the_cursor_LOOP, &window);
	
	XEvent event;
	while(window.loop_running == 1) {
		pthread_mutex_unlock(window.thread.lock);
		poll_ret = poll(fds, 2, 1000);
		pthread_mutex_lock(window.thread.lock);
		if (poll_ret < 0) {
			window.loop_running = 0;
		} else if(poll_ret > 0) {
			if ((fds[0].revents & POLLIN) > 0) {
				if (XPending(window.dis) > 0) {
					XNextEvent(window.dis, &event);
					event_handler(&window, &event);
				}
			}
			if ((fds[1].revents & POLLIN) > 0) {
				charbuf = 0;
				if(read(fds[1].fd, &charbuf, 1) == 1) {
					if (charbuf == 1) {
						draw_element(window.root_element, &window);
						draw_bm(0, 0, window.width, window.height, &window);
						pthread_cancel(*(window.thread.thread));
						pthread_join(*(window.thread.thread), 0);
						pthread_create(window.thread.thread, 0, blink_the_cursor_LOOP, &window);
					}
				}
			}
		}
	}
	
	pthread_mutex_unlock(window.thread.lock);
	pthread_cancel(*(window.thread.thread));
	pthread_join(*(window.thread.thread), 0);
	
	//TODO: Clean Window Exit
	//XUnmapWindow(window.dis, window.win);
	//XDestroySubwindows(window.dis, window.win); // -- Causes Errors, need to do more research
	//XDestroyWindow(window.dis, window.win); // -- Causes Errors, need to do more research
	//XCloseDisplay(window.dis); // -- Causes Errors, need to do more research
	close(window.fd);
	
	close(window.thread.fd);
	close(fds[1].fd);
	close(fds[0].fd);
	
	free(window.mouse_state.pixel_element_map);
	free(window.bitmap);
	free_element_tree(root_cont);
	free_font(&font_pack);
	
	return 0;
}
