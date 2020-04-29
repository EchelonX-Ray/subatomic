#ifndef _insertion_lib_wincomp_input_keyboard_h
#define _insertion_lib_wincomp_input_keyboard_h

#include "./../wincomp.h"
#include "./../text.h"

unsigned int keytoaction(struct Working_Text *working_text, unsigned int keycode, unsigned int mod_flags, struct MTK_WinBase *window);
unsigned int keytotext(KeySym keysym, unsigned int mod_flags);

#endif
