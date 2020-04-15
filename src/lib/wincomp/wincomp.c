#include "./wincomp.h"

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

void window_struct_init(struct MTK_WinBase *window){
	window->loop_running = 1;
	window->ignore_key_repeat = 0;
	window->mouse_state.pixel_element_map = 0;
	window->mouse_state.previous_mouse_element = 0;
	window->mouse_state.previous_mouse_x = 0;
	window->mouse_state.previous_mouse_y = 0;
	window->mouse_state.mouse_down_x = 0;
	window->mouse_state.mouse_down_y = 0;
	window->mouse_state.mouse_state = MS_UP;
	window->cursor_blink = 1;
	window->root_element = 0;
	window->focused_element = 0;
	window->fd = 0;
	window->_internal_ignore_next_ke = 0;
	
	unsigned int i = 0;
	while (i < MTKEvent_Count) {
		window->event_handles[i] = 0;
		i++;
	}
	
	return;
}

void create_window(struct MTK_WinBase *vals){
	int screen;
	//Window win;
	//GC gc;
	
	//Atom wmDeleteMessage;
	
	/* get the colors black and white (see section for details) */
	unsigned long black;
	unsigned long white;
	
	//char running = 1;
	
	screen = DefaultScreen(vals->dis);
	black = BlackPixel(vals->dis, screen),	/* get color black */
	white = WhitePixel(vals->dis, screen);  /* get color white */
	
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
