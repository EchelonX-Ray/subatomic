#include "./wincomp.h"
#include <X11/cursorfont.h>
#include <unistd.h>
#include <stdlib.h>

/*
struct MTK_WinMouseStateTracking {
	struct MTK_WinElement **pixel_element_map;
	struct MTK_WinElement *previous_mouse_element;
	signed int previous_mouse_x;
	signed int previous_mouse_y;
	signed int mouse_down_x;
	signed int mouse_down_y;
	unsigned int mouse_state;
};
*/

void window_struct_zero(struct MTK_WinBase *window){
	unsigned int i = 0;
	
	window->loop_running = 2;
	window->ignore_key_repeat = 0;
	window->_internal_mouse_state.pixel_element_map = 0;
	window->_internal_mouse_state.previous_mouse_element = 0;
	i = 0;
	while (i < MOUSE_BUTTONS) {
		window->_internal_mouse_state.mouse_moved_while_button_down_array[i] = 0;
		window->_internal_mouse_state.mouse_state_array[i] = MS_UP;
		i++;
	}
	window->_internal_mouse_state.mouse_moved_while_button_down = window->_internal_mouse_state.mouse_moved_while_button_down_array - MOUSE_BUTTON_START;
	window->_internal_mouse_state.mouse_state = window->_internal_mouse_state.mouse_state_array - MOUSE_BUTTON_START;
	window->cursor_blink = 1;
	window->root_element = 0;
	window->focused_element = 0;
	window->fd = 0;
	window->_internal_ignore_next_ke = 0;
	
	i = 0;
	while (i < MTKEvent_Count) {
		window->event_handles[i] = 0;
		i++;
	}
		
	return;
}

void window_struct_init(struct MTK_WinBase *window){	
	window->bitmap = calloc(window->width * window->height, sizeof(uint32_t));
	window->_internal_mouse_state.pixel_element_map = calloc(window->width * window->height, sizeof(struct MTK_WinElement**));
	
	return;
}

void create_window(struct MTK_WinBase *vals){
	int screen;
	//Window win;
	//GC gc;
	
	//Atom wmDeleteMessage;
	
	unsigned long black;
	unsigned long white;
	
	//char running = 1;
	vals->dis = XOpenDisplay((char *)0);
	vals->root_win = DefaultRootWindow(vals->dis);
	
	screen = DefaultScreen(vals->dis);
	black = BlackPixel(vals->dis, screen);
	white = WhitePixel(vals->dis, screen);
	
	vals->win = XCreateSimpleWindow(vals->dis, vals->root_win, 0, 0, vals->width, vals->height, 5, white, black);
	
	//Atom type = XInternAtom((*vals).dis, "_NET_WM_WINDOW_TYPE", False);
	//Atom value = XInternAtom((*vals).dis, "_NET_WM_WINDOW_TYPE_SPLASH", False);
	//XChangeProperty((*vals).dis, (*vals).win, type, XA_ATOM, 32, PropModeReplace, (char*)&value, 1);
	
	XSetStandardProperties(vals->dis, vals->win, vals->title, "icon", None, NULL, 0, NULL);
	//XFlush((*vals).dis);
	XMapWindow(vals->dis, vals->root_win);
	
	XSelectInput(vals->dis, vals->win, vals->events);
	vals->gc = XCreateGC(vals->dis, vals->win, 0, 0);
	
	XSetBackground(vals->dis, vals->gc, white);
	XSetForeground(vals->dis, vals->gc, black);
	
	XClearWindow(vals->dis, vals->win);
	XMapRaised(vals->dis, vals->win);
	
	vals->wmDM = XInternAtom(vals->dis, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(vals->dis, vals->win, &(vals->wmDM), 1);
	
	vals->fd = ConnectionNumber(vals->dis);
	
#ifndef DEVEL_STRIP_MCURSOR
	vals->_internal_cursor = malloc(CS_COUNT * sizeof(Cursor));
	
	vals->_internal_cursor[CS_Pointer] = XCreateFontCursor(vals->dis, XC_left_ptr);
	vals->_internal_cursor[CS_Text] = XCreateFontCursor(vals->dis, XC_xterm);
	
	vals->_internal_cursor_index = CS_Pointer;
	XDefineCursor(vals->dis, vals->win, vals->_internal_cursor[CS_Pointer]);
#endif
	
	//vals->_internal_cursor = XcursorLibraryLoadCursor(vals->dis, "xterm");
	//XDefineCursor(vals->dis, vals->win, vals->_internal_cursor);
	//XSetWindowAttributes attrib;
	//attrib.cursor = vals->_internal_cursor;
	//XChangeWindowAttributes(vals->dis, vals->win, CWCursor, &attrib);
	
	//XWindowAttributes ra;
	//XGetWindowAttributes(vals->dis, vals->root_win, &ra);
	//int wid = ra.width;
	//int hei = ra.height;
	//printf("Width: %d", wid);
	//printf(", ");
	//printf("Height: %d", hei);
	//printf("\n");
	
	//void (*event_func)(void**) = event_loop;
	//void (*close_func)(Display*, Window*, GC*, char*, void*) = close_x;
	//void *ptrs[10];
	
	//ptrs[0] = event_func;
	//ptrs[1] = close_func;
	//ptrs[2] = dis;
	//ptrs[3] = &win;
	//ptrs[4] = &gc;
	//ptrs[5] = &wmDeleteMessage;
	//ptrs[6] = &running;
	//ptrs[7] = key_event;
	//ptrs[8] = mouse_event;
	//ptrs[9] = close_event;
	
	//return ptrs;
	
	return;
}

void free_window(struct MTK_WinBase *window){
	XUnmapWindow(window->dis, window->win);
	XFree(window->gc);
	XDestroyWindow(window->dis, window->win);
#ifndef DEVEL_STRIP_MCURSOR
	XFreeCursor(window->dis, window->_internal_cursor[CS_Pointer]);
	XFreeCursor(window->dis, window->_internal_cursor[CS_Text]);
	free(window->_internal_cursor);
#endif
	XCloseDisplay(window->dis);
	close(window->fd);
	free(window->_internal_mouse_state.pixel_element_map);
	free(window->bitmap);
	return;
}
