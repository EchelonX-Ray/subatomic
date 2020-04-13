#include "./pthreading.h"
#include "./../lib/wincomp/drawing.h"
#include "./../lib/wincomp/elements.h"
#include <time.h>

void* thread0_entry_blink_cursor(void *thread_param) {
	struct MTK_Thread_Param *param;
	struct MTK_WinBase *window;
	
	struct timespec req;
	req.tv_sec = 1;
	req.tv_nsec = 0;
	
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
	pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, 0);
	param = (struct MTK_Thread_Param*)thread_param;
	pthread_mutex_lock(param->lock);
	window = param->window;
	while(window->loop_running == 1) {
		if (window->cursor_blink == 1) {
			window->cursor_blink = 0;
		} else {
			window->cursor_blink = 1;
		}
		draw_element(window->root_element, window);
		draw_bm(0, 0, window->width, window->height, window);
		pthread_mutex_unlock(param->lock);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
		nanosleep(&req, 0);
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		pthread_mutex_lock(param->lock);
	}
	pthread_mutex_unlock(param->lock);
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, 0);
	
	return 0;
}
