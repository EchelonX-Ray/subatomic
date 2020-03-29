#ifndef _insertion_lib_wincomp_wincomp_h
#define _insertion_lib_wincomp_wincomp_h

//#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/Xos.h>
#include <stdio.h>

//#include <X11/Xatom.h>

#define KeyEvent 0
#define MouseBtnEvent 1
#define MouseMoveEvent 2
#define LeaveEvent 3
#define ExposeEvent 4
#define CloseEvent 5

#define MTKEvent_Count 6

struct MTK_WinBase {
	// Common GFX Values
	Display *dis;
	Window win;
	Window root_win;
	GC gc;
	Atom wmDM;
	char *title;
	
	// Program Values
	int width;
	int height;
	char *bitmap;
	long events;
	int loop_running;
	
	// Common Events
	//void *closing_event;
	//void *closed_event;
	//void *key_event;
	//void *mouse_event;
	//void *window_event;
	//void *redraw_event;
	
	void* event_handles[MTKEvent_Count];
	
	//struct MTK_CharBM **char_maps; // Array of Pointers to Font Map Arrays of lengths of 256.  Bit-0: Bold, Bit-1: Italic, Bit-2: Underline, Bit-3: Strike-through
	//struct MTK_MenuBlock *menu_bar;
	
	int _internal_ignore_key_repeat;			//Flag checked in key_event function
	int _internal_ignore_next_ke;			//Used as part of the ignore key repeat feature
};

void window_struct_init(struct MTK_WinBase *window);
void create_window(struct MTK_WinBase *vals);

#include "./events.h"

#endif