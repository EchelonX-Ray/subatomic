#include "./events.h"
#include "./elements.h"
#include "./drawing.h"

// KeyEvent MouseBtnEvent MouseMoveEvent LeaveEvent ExposeEvent
// ExposureMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask|LeaveWindowMask
long mtk_gem(int setting) {
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
	
	window->loop_running = 0;
	
	return;
}

void element_mousebutton_event(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinBase* window) {
	if (event == 0 || window == 0) {
		return;
	}
	unsigned int element_redraw_required;
	struct MTK_WinElement *element;
	if (x >= 0 && x < window->width && y >= 0 && y < window->height) {
		element = window->_internal_mouse_state.pixel_element_map[y * window->width + x];
	} else {
		element = 0;
	}
	element_redraw_required = 0;
	
	if (state == MS_UP) {
		element = window->_internal_mouse_state.previous_mouse_element;
	}
	if (element != 0) {
		if        (element->type == EL_BUTTON) {
			element_redraw_required |= button_event_button(state, button, x, y, event, element, window);
		} else if (element->type == EL_TEXTBOX) {
			element_redraw_required |= textbox_event_button(state, button, x, y, event, element, window);
		}
	}
	if        (state == MS_DOWN) {
		window->_internal_mouse_state.previous_mouse_element = element;
		window->_internal_mouse_state.mouse_state[button] = MS_DOWN;
	} else if (state == MS_UP) {
		window->_internal_mouse_state.mouse_state[button] = MS_UP;
		if (window->_internal_mouse_state.mouse_moved_while_button_down[button] == 1) {
			window->_internal_mouse_state.mouse_moved_while_button_down[button] = 0;
			element_mousemotion_event(x, y, event, window);
		}
	}
	
	if (element_redraw_required & 0x2) {
		draw_bm(	0, \
					0, \
					window->width, \
					window->height, \
					window	);
	} else if (element_redraw_required & 0x1) {
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
	}
	return;
}

void element_mousemotion_event(int x, int y, XEvent* event, struct MTK_WinBase* window) {
	if (event == 0 || window == 0) {
		return;
	}
	unsigned int element_redraw_required;
	unsigned int prev_element_redraw_required;
	unsigned int i;
	struct MTK_WinElement *element;
	struct MTK_WinElement *previous_mouse_element;
	
	if (x >= 0 && x < window->width && y >= 0 && y < window->height) {
		element = window->_internal_mouse_state.pixel_element_map[y * window->width + x];
	} else {
		element = 0;
	}
	
	previous_mouse_element = window->_internal_mouse_state.previous_mouse_element;
	element_redraw_required = 0;
	prev_element_redraw_required = 0;
	i = 0;
	while (i < MOUSE_BUTTONS) {
		if (window->_internal_mouse_state.mouse_state_array[i] == MS_DOWN) {
			window->_internal_mouse_state.mouse_moved_while_button_down_array[i] = 1;
			element = previous_mouse_element;
		}
		i++;
	}
	if (element == 0) {
#ifndef DEVEL_STRIP_MCURSOR
		if (window->_internal_cursor_index != CS_Pointer) {
			window->_internal_cursor_index = CS_Pointer;
			XDefineCursor(window->dis, window->win, window->_internal_cursor[CS_Pointer]);
		}
#endif
		window->_internal_mouse_state.previous_mouse_element = 0;
		goto do_only_previous_element_operations;
	}
	if        (element->type == EL_BUTTON) {
		element_redraw_required |= button_event_move(x, y, event, element, window);
	} else if (element->type == EL_TEXTBOX) {
		element_redraw_required |= textbox_event_move(x, y, event, element, window);
	}
	if (previous_mouse_element != element) {
		window->_internal_mouse_state.previous_mouse_element = element;
		
		do_only_previous_element_operations:
		if (previous_mouse_element != 0) {
			if        (previous_mouse_element->type == EL_BUTTON) {
				prev_element_redraw_required |= button_leave(x, y, event, previous_mouse_element, window);
			} else if (previous_mouse_element->type == EL_TEXTBOX) {
				prev_element_redraw_required |= textbox_leave(x, y, event, previous_mouse_element, window);
			}
			
			if ((element_redraw_required & 0x2) == 0) {
				if (prev_element_redraw_required & 0x2) {
					element_redraw_required |= 0x2;
				} else if (prev_element_redraw_required & 0x1) {
					draw_bm(	previous_mouse_element->_internal_computed_xoffset, \
								previous_mouse_element->_internal_computed_yoffset, \
								previous_mouse_element->_internal_computed_width, \
								previous_mouse_element->_internal_computed_height, \
								window	);
				}
			}
		}
	}
	if (element_redraw_required & 0x2) {
		draw_bm(	0, \
					0, \
					window->width, \
					window->height, \
					window	);
	} else if (element_redraw_required & 0x1) {
		draw_bm(	element->_internal_computed_xoffset, \
					element->_internal_computed_yoffset, \
					element->_internal_computed_width, \
					element->_internal_computed_height, \
					window	);
	}
	return;
}

void event_handler(struct MTK_WinBase *window, XEvent *event) {
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
		// Ignore mouse buttons that aren't defined
		if (event->xbutton.button < MOUSE_BUTTON_START || event->xbutton.button >= MOUSE_BUTTON_START + MOUSE_BUTTONS) {
			return;
		}
		mouse_btn_func = window->event_handles[MouseBtnEvent];
		mouse_btn_func(MS_DOWN, event->xbutton.button, event->xbutton.x, event->xbutton.y, event, window);
		return;
	}
	if (event->type == ButtonRelease && window->event_handles[MouseBtnEvent] != 0) {
		// Ignore mouse buttons that aren't defined
		if (event->xbutton.button < MOUSE_BUTTON_START || event->xbutton.button >= MOUSE_BUTTON_START + MOUSE_BUTTONS) {
			return;
		}
		mouse_btn_func = window->event_handles[MouseBtnEvent];
		mouse_btn_func(MS_UP, event->xbutton.button, event->xbutton.x, event->xbutton.y, event, window);
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
