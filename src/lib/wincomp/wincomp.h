#ifndef _insertion_lib_wincomp_wincomp_h
#define _insertion_lib_wincomp_wincomp_h

#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
//#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
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

#ifndef DEVEL_STRIP_MCURSOR
#define CS_COUNT 2
#define CS_Pointer 0
#define CS_Text 1
#endif

struct MTK_WinMouseStateTracking {
	struct MTK_WinElement **pixel_element_map;
	struct MTK_WinElement *previous_mouse_element;
	unsigned int mouse_state;
};
struct MTK_WinThreadReturn {
};
struct MTK_WinThreadParam {
	pthread_t *thread;
	pthread_mutex_t *lock;
	int fd;
	unsigned int millisec_increment;
	struct MTK_WinThreadReturn ret_val;
};

struct MTK_WinBase {
	// Common GFX Values
	Display *dis;
	Window win;
	Window root_win;
	GC gc;
	Atom wmDM;
	char *title;
	int fd;
	
	// Program Values
	int width;
	int height;
	uint32_t *bitmap;
	long events;
	volatile unsigned int loop_running;
	
	void* event_handles[MTKEvent_Count];
	
	struct MTK_WinElement *root_element;
	struct MTK_WinElement *focused_element;
	struct MTK_WinMouseStateTracking mouse_state;
	volatile unsigned char cursor_blink;
	struct MTK_WinThreadParam thread;
	
	//struct MTK_CharBM **char_maps; // Array of Pointers to Font Map Arrays of lengths of 256.  Bit-0: Bold, Bit-1: Italic, Bit-2: Underline, Bit-3: Strike-through
	//struct MTK_MenuBlock *menu_bar;
	
	int ignore_key_repeat;					// Set this to "1" disable Key Repeat
	int _internal_ignore_next_ke;			// Used as part of the ignore key repeat feature.  This is used internally.  It should not be touched by programs using this library.
#ifndef DEVEL_STRIP_MCURSOR
	Cursor *_internal_cursor;
	unsigned int _internal_cursor_index;
#endif
};

void window_struct_init(struct MTK_WinBase *window);
void create_window(struct MTK_WinBase *vals);
void free_window(struct MTK_WinBase *window);

#include "./events.h"

#endif
