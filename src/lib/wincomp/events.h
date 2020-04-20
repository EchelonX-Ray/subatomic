#ifndef _insertion_lib_wincomp_events_h
#define _insertion_lib_wincomp_events_h

#include "./wincomp.h"

void event_handler(struct MTK_WinBase *window, XEvent *event);
void cue_window_close(struct MTK_WinBase *window, XEvent *event);
long mtk_gem(int setting);
void element_mousemotion_event(int x, int y, XEvent* event, struct MTK_WinBase* window);

#endif
