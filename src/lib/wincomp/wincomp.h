#ifndef _insertion_lib_wincomp_wincomp_h
#define _insertion_lib_wincomp_wincomp_h

//#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/Xos.h>
#include <stdint.h>
#include <stdio.h>

//#include <X11/Xatom.h>

#define KeyEvent 0
#define MouseBtnEvent 1
#define MouseMoveEvent 2
#define LeaveEvent 3
#define ExposeEvent 4
#define CloseEvent 5

#define MTKEvent_Count 6

#define MS_UP 0
#define MS_DOWN 1

struct MTK_WinMouseStateTracking {
	struct MTK_WinElement **pixel_element_map;
	struct MTK_WinElement *previous_mouse_element;
	signed int previous_mouse_x;
	signed int previous_mouse_y;
	signed int mouse_down_x;
	signed int mouse_down_y;
	unsigned int mouse_state;
};

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
	uint32_t *bitmap;
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
	
	struct MTK_WinElement *root_element;
	struct MTK_WinMouseStateTracking mouse_state;
	
	//struct MTK_CharBM **char_maps; // Array of Pointers to Font Map Arrays of lengths of 256.  Bit-0: Bold, Bit-1: Italic, Bit-2: Underline, Bit-3: Strike-through
	//struct MTK_MenuBlock *menu_bar;
	
	int ignore_key_repeat;					// Set this to "1" disable Key Repeat
	int _internal_ignore_next_ke;			// Used as part of the ignore key repeat feature.  This is used internally.  It should not be touched by programs using this library.
};

void window_struct_init(struct MTK_WinBase *window);
void create_window(struct MTK_WinBase *vals);

#include "./events.h"

#endif
