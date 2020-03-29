#ifndef _insertion_core_events_h
#define _insertion_core_events_h

#include "./subatomic.h"
#include <stdio.h>

void exposure_event(XEvent* event, struct MTK_WinBase* window);
void button_event(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinBase* window);
void key_event(int state, int keycode, XEvent* event, struct MTK_WinBase* window);
void pointer_motion_event(int x, int y, XEvent* event, struct MTK_WinBase* window);
void leave_window_event(int x, int y, XEvent* event, struct MTK_WinBase* window);

#endif
