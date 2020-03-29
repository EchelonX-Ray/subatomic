#include "./wincomp.h"

void window_struct_init(struct MTK_WinBase *window){
	window->loop_running = 1;
	
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
	
	XWindowAttributes ra;
	XGetWindowAttributes(vals->dis, vals->root_win, &ra);
	int wid = ra.width;
	int hei = ra.height;
	printf("Width: %d", wid);
	printf(", ");
	printf("Height: %d", hei);
	printf("\n");
	
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
