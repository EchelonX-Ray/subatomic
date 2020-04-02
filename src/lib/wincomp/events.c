#include "./events.h"

// KeyEvent MouseBtnEvent MouseMoveEvent LeaveEvent ExposeEvent
// ExposureMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask|LeaveWindowMask
long mtk_gem(int setting){
	if (setting == KeyEvent) {
		return KeyPressMask | KeyReleaseMask;
	}
	if (setting == MouseBtnEvent) {
		return ButtonPressMask | ButtonReleaseMask;
	}
	if (setting == MouseMoveEvent) {
		return PointerMotionMask;
	}
	if (setting == LeaveEvent) {
		return LeaveWindowMask;
	}
	if (setting == ExposeEvent) {
		return ExposureMask;
	}
	return 0;
}

void window_close(struct MTK_WinBase *window) {
	window->loop_running = 0;
	XFreeGC(window->dis, window->gc);
	XDestroyWindow(window->dis, window->win);
	XCloseDisplay(window->dis);
	return;
}

void cue_window_close(struct MTK_WinBase *window, XEvent *event) {
	int abort_window_close = 0;
	
	int (*closing_func)(struct MTK_WinBase*, XEvent*);
	if (window->event_handles[CloseEvent] != 0) {
		closing_func = window->event_handles[CloseEvent];
		abort_window_close = closing_func(window, event);
	}
	
	if (abort_window_close == 1) {
		return;
	}
	
	window_close(window);
	
	return;
}

void event_handler(struct MTK_WinBase *window, XEvent *event){
	void (*key_func)(int, int, XEvent*, struct MTK_WinBase*);
	void (*mouse_btn_func)(int, unsigned int, int, int, XEvent*, struct MTK_WinBase*);
	void (*mouse_move_func)(int, int, XEvent*, struct MTK_WinBase*);
	void (*leave_func)(int, int, XEvent*, struct MTK_WinBase*);
	void (*expos_func)(XEvent*, struct MTK_WinBase*);
	
	if (event->type == Expose && event->xexpose.count == 0) {
		expos_func = window->event_handles[ExposeEvent];
		expos_func(event, window);
		return;
	}
	if (event->type == KeyPress) {
		// Ignore key repeat if a key repeat was detected and
		// ignore_key_repeat was set
		if (window->_internal_ignore_next_ke == 1) {
			window->_internal_ignore_next_ke = 0;
			return;
		}
		
		key_func = window->event_handles[KeyEvent];
		key_func(1, event->xkey.keycode, event, window);
		return;
		
		/* Disabled for reference
		if (event.xkey.keycode == 0xFF) {
			close_x(vals);
		} else {
			key_func(2, &event, vals);
		}
		*/
	}
	if (event->type == KeyRelease && window->event_handles[KeyEvent] != 0) {
		// If ignore key repeat set, check to see if this is a key repeat 
		// If this is a key repeat, disable the next cued key event and 
		// then return from this key event
		if (window->ignore_key_repeat == 1) {
			// This could be combined with the previous if statment using
			// && logic.  But I decided against it just in case the compiler
			// doesn't optimize away the 2nd condition.  I didn't want it to run 
			// if ignore_key_repeat is not set.  It probably wouldn't hurt 
			// anything, but if for no other reason than to improve preformance.
			if (XEventsQueued(window->dis, QueuedAfterReading) > 0) {
				XEvent nev;
				XPeekEvent(window->dis, &nev);
				if (nev.type == KeyPress && nev.xkey.time == event->xkey.time && nev.xkey.keycode == event->xkey.keycode) {
					window->_internal_ignore_next_ke = 1;
					return;
				}
			}
		}
		
		key_func = window->event_handles[KeyEvent];
		key_func(2, event->xkey.keycode, event, window);
		return;
	}
	if (event->type == ButtonPress && window->event_handles[MouseBtnEvent] != 0) {
		mouse_btn_func = window->event_handles[MouseBtnEvent];
		mouse_btn_func(1, event->xbutton.button, event->xbutton.x, event->xbutton.y, event, window);
		return;
	}
	if (event->type == ButtonRelease && window->event_handles[MouseBtnEvent] != 0) {
		mouse_btn_func = window->event_handles[MouseBtnEvent];
		mouse_btn_func(2, event->xbutton.button, event->xbutton.x, event->xbutton.y, event, window);
		return;
	}
	if (event->type == MotionNotify && window->event_handles[MouseMoveEvent] != 0) {
		mouse_move_func = window->event_handles[MouseMoveEvent];
		mouse_move_func(event->xcrossing.x, event->xcrossing.y, event, window);
		return;
	}
	if (event->type == LeaveNotify && window->event_handles[LeaveEvent] != 0) {
		leave_func = window->event_handles[LeaveEvent];
		leave_func(event->xbutton.x, event->xbutton.y, event, window);
		return;
	}
	
	if (event->type == ClientMessage) {
		if (event->xclient.data.l[0] == window->wmDM) {
			cue_window_close(window, event);
		}
	}
	
	return;
}
