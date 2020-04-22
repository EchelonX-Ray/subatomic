#include "./subatomic.h"

#include <poll.h>
#include <unistd.h>

#include "./win_setup/main_win.c"

#ifndef TEST_FONT_DIR
#define TEST_FONT_DIR "/usr/share/fonts/dejavu/DejaVuSansMono.ttf"
#endif

int main(int argc, char *argv[]) {
	// Setup the font definitions
	struct MTK_WinFontPack font_pack;
	printf("%s\n", TEST_FONT_DIR);
	setup_font(TEST_FONT_DIR, 25, &font_pack);
	
	// Setup the window element definitions
	struct MTK_WinElement *root_cont = calloc(1, sizeof(struct MTK_WinElement));
	setup_main_win_elements(&root_cont, &font_pack);
	
	// Setup the global base window struct
	struct MTK_WinBase window; // Declare the window struct
	window_struct_zero(&window); // Zero the window struct values
	// Set the window values we need
	window.title = "Sub-Atomic Editor";
	window.events = mtk_gem(KeyEvent) | mtk_gem(MouseBtnEvent) | mtk_gem(MouseMoveEvent) | mtk_gem(LeaveEvent) | mtk_gem(ExposeEvent);
	window.event_handles[KeyEvent] = (void*)&key_event;
	window.event_handles[MouseBtnEvent] = (void*)&button_event;
	window.event_handles[MouseMoveEvent] = (void*)&pointer_motion_event;
	window.event_handles[LeaveEvent] = (void*)&leave_window_event;
	window.event_handles[ExposeEvent] = (void*)&exposure_event;
	window.event_handles[CloseEvent] = (void*)&before_closing;
	window.width = 640;
	window.height = 480;
	window.root_element = root_cont;
	window.ignore_key_repeat = 0; // Ignore Key Repeat?
	window.focused_element = root_cont->children[1];
	window_struct_init(&window); // Initialize the window struct values
	
	// Compute the element geometry
	// This must be done before any drawing
	// This also depends on the the window struct
	//   values of width, height, and root_element
	//   being setup
	compute_element_internals(&window);
	
	// Create the window
	create_window(&window);
	
	// Setup Threads and Pipe Notification of the Event Loop
	int pipefd_pair[2];
	if (pipe(pipefd_pair) != 0) {
		return -1;
	}
	pthread_t threads[1];
	pthread_mutex_t thread_lock[1];
	window.thread.thread = threads + 0; // Set pointer to thread
	window.thread.lock = thread_lock + 0; // Set pointer to mutex
	window.thread.fd = pipefd_pair[1]; // Set transmit file descriptor
	window.thread.millisec_increment = 1000; // Set cursor blink rate
	pthread_mutex_init(window.thread.lock, NULL);
	pthread_mutex_lock(window.thread.lock);
	char charbuf;
	struct pollfd fds[2];
	fds[0].fd = window.fd;
	fds[0].events = POLLIN;
	fds[1].fd = pipefd_pair[0]; // Set receive file descriptor
	fds[1].events = POLLIN;
	signed int poll_ret;
	
	// Start the Thread
	start_the_cursor(&window, 1);
	
	// Begin The Event Loop
	XEvent event;
	while (window.loop_running > 0) {
		pthread_mutex_unlock(window.thread.lock);
		poll_ret = poll(fds, 2, 1000); // Wait for Pipe Notification or Timeout after 1000 milliseconds
		pthread_mutex_lock(window.thread.lock);
		if (poll_ret < 0) { // Error
			window.loop_running = 0;
		} else if (poll_ret > 0) { // Pipe Notification Received
			if ((fds[0].revents & POLLIN) > 0) { // Xlib Pipe Notification Received
				while(XPending(window.dis) > 0) {
					XNextEvent(window.dis, &event);
					event_handler(&window, &event);
				}
			}
			if ((fds[1].revents & POLLIN) > 0) { // External Pipe Notification Received
				charbuf = 0;
				if (read(fds[1].fd, &charbuf, 1) == 1) {
					if (charbuf == 1) {
						draw_element(window.focused_element, &window);
						draw_bm( window.focused_element->_internal_computed_xoffset, \
									window.focused_element->_internal_computed_yoffset, \
									window.focused_element->_internal_computed_width, \
									window.focused_element->_internal_computed_height, \
									&window	);
						pthread_cancel(*(window.thread.thread));
						pthread_join(*(window.thread.thread), 0);
						pthread_create(window.thread.thread, 0, blink_the_cursor, &window);
					}
				}
			}
		}
	}
	
	// Rejoin the threads
	pthread_mutex_unlock(window.thread.lock);
	stop_the_cursor(&window, 0);
	
	// Close the opened file descriptors
	close(pipefd_pair[0]);
	close(pipefd_pair[1]);
	
	// Free up allocated memory
	free_window(&window);
	free_element_tree(root_cont);
	free_font(&font_pack);
	
	return 0;
}
