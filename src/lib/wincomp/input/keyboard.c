#include <stdlib.h>
#include "./keyboard.h"
#include "./../../toolbox/cstr_manip.h"

void clean_unused_blocks(register struct Working_Text *working_text) {
	register void *tmp_cstr;
	register unsigned int unused_block_count;
	
	unused_block_count = ((working_text->current_alloc_unit_count * working_text->alloc_unit) - working_text->current_bytes_used) / working_text->alloc_unit;
	if (0 < unused_block_count) {
		working_text->current_alloc_unit_count -= unused_block_count;
		tmp_cstr = malloc(working_text->current_alloc_unit_count * working_text->alloc_unit);
		cstrcpy(working_text->text, tmp_cstr);
		free(working_text->text);
		working_text->text = tmp_cstr;
	}
	
	return;
}

void delete_selection_substr(register struct Working_Text *working_text) {
	register unsigned int cursor_pos = working_text->cursor_position;
	register unsigned int cursor_base_pos = working_text->cursor_base_sel_position;
	register void *tmp_cstr = &(working_text->text);
	
	if (cursor_pos > cursor_base_pos) {
		cdelbstr(tmp_cstr, cursor_base_pos, cursor_pos - cursor_base_pos);
		working_text->current_bytes_used -= cursor_pos - cursor_base_pos;
		working_text->cursor_position = cursor_base_pos;
	} else if (cursor_pos < cursor_base_pos) {
		cdelbstr(tmp_cstr, cursor_pos, cursor_base_pos - cursor_pos);
		working_text->current_bytes_used -= cursor_base_pos - cursor_pos;
		working_text->cursor_base_sel_position = cursor_pos;
	}
	
	clean_unused_blocks(working_text);
	
	return;
}

unsigned int keytoaction(struct Working_Text *working_text, unsigned int keycode, unsigned int mod_flags, struct MTK_WinBase *window) {
	unsigned int redraw_required;
	unsigned int num_mod_flags;
	unsigned int key;
	unsigned int ml = 0; // Placeholder
	redraw_required = 0;
	
	KeySym keysym;
	KeySym num_keysym;
	num_mod_flags = mod_flags;
	if (num_mod_flags & Mod2Mask) {
		num_mod_flags ^= ShiftMask;
	}
	num_keysym = XkbKeycodeToKeysym(window->dis, keycode, 0, num_mod_flags & ShiftMask);
	keysym = XkbKeycodeToKeysym(window->dis, keycode, 0, mod_flags & ShiftMask);
	
	if (	(num_keysym >= XK_KP_F1 && num_keysym <= XK_KP_Delete) || \
			(num_keysym >= XK_KP_Multiply && num_keysym <= XK_KP_9) || \
			(num_keysym == XK_KP_Equal)	) {
		key = keytotext(num_keysym, 0);
	} else {
		key = keytotext(keysym, mod_flags);
	}
	
	if (key) {
		if (working_text->cursor_position != working_text->cursor_base_sel_position) {
			delete_selection_substr(working_text);
		}
		if (ml) {
			// TODO
			return 0;
		} else {
			char payload[2];
			payload[1] = 0;
			payload[0] = key;
			if (working_text->current_bytes_used == working_text->current_alloc_unit_count * working_text->alloc_unit) {
				char *tmp_cstr;
				working_text->current_alloc_unit_count++;
				tmp_cstr = malloc(working_text->current_alloc_unit_count * working_text->alloc_unit);
				cmemcpy(working_text->text, tmp_cstr, working_text->cursor_position);
				cmemcpy(payload, tmp_cstr + working_text->cursor_position, 1);
				cmemcpy(working_text->text + working_text->cursor_position, tmp_cstr + working_text->cursor_position + 1, (working_text->current_bytes_used - working_text->cursor_position));
				working_text->current_bytes_used++;
				free(working_text->text);
				working_text->text = tmp_cstr;
			} else {
				cmemcpy(	working_text->text + working_text->cursor_position, 
							working_text->text + working_text->cursor_position + 1,
							-((signed int)(working_text->current_bytes_used - working_text->cursor_position))	);
				working_text->text[working_text->cursor_position] = payload[0];
				working_text->current_bytes_used++;
			}
			working_text->cursor_position += +1;
			working_text->cursor_base_sel_position += +1;
			redraw_required |= 0x35;
		}
	} else {
		if (keysym == XK_BackSpace) {
			// If there is a selection, delete it.  If not, Backspace
			if (working_text->cursor_position != working_text->cursor_base_sel_position) {
				delete_selection_substr(working_text);
			} else {
				if (working_text->cursor_position > 0) {
					cdelbstr(&(working_text->text), working_text->cursor_position, -1);
					working_text->cursor_position -= 1;
					working_text->cursor_base_sel_position -= 1;
					working_text->current_bytes_used -= 1;
					clean_unused_blocks(working_text);
				}
			}
			redraw_required |= 0x35;
		} else if (keysym == XK_Delete || num_keysym == XK_KP_Delete) {
			// If there is a selection, delete it.  If not, Delete
			if (working_text->cursor_position != working_text->cursor_base_sel_position) {
				delete_selection_substr(working_text);
			} else {
				if (working_text->cursor_position + 1 < working_text->current_bytes_used) {
					cdelbstr(&(working_text->text), working_text->cursor_position, +1);
					working_text->current_bytes_used -= 1;
					clean_unused_blocks(working_text);
				}
			}
			redraw_required |= 0x35;
		} else if (keysym == XK_Left || num_keysym == XK_KP_Left) {
			if ((mod_flags & ShiftMask) && ((num_keysym != XK_KP_Left) || !((num_mod_flags ^ mod_flags) & ShiftMask))) {
				if (working_text->cursor_position > 0) {
					working_text->cursor_position += -1;
				}
			} else {
				if (working_text->cursor_base_sel_position != working_text->cursor_position) {
					if (working_text->cursor_base_sel_position < working_text->cursor_position) {
						working_text->cursor_position = working_text->cursor_base_sel_position;
					} else {
						working_text->cursor_base_sel_position = working_text->cursor_position;
					}
				} else {
					if (working_text->cursor_position > 0) {
						working_text->cursor_position += -1;
						working_text->cursor_base_sel_position += -1;
					}
				}
			}
			redraw_required |= 0x35;
		} else if (keysym == XK_Right || num_keysym == XK_KP_Right) {
			if ((mod_flags & ShiftMask) && ((num_keysym != XK_KP_Right) || !((num_mod_flags ^ mod_flags) & ShiftMask))) {
				if (working_text->cursor_position + 1 < working_text->current_bytes_used) {
					working_text->cursor_position += +1;
				}
			} else {
				if (working_text->cursor_base_sel_position != working_text->cursor_position) {
					if (working_text->cursor_base_sel_position > working_text->cursor_position) {
						working_text->cursor_position = working_text->cursor_base_sel_position;
					} else {
						working_text->cursor_base_sel_position = working_text->cursor_position;
					}
				} else {
					if (working_text->cursor_position + 1 < working_text->current_bytes_used) {
						working_text->cursor_position += +1;
						working_text->cursor_base_sel_position += +1;
					}
				}
			}
			redraw_required |= 0x35;
		} else if (keysym == XK_Home || num_keysym == XK_KP_Home) {
			if ((mod_flags & ShiftMask) && ((num_keysym != XK_KP_Home) || !((num_mod_flags ^ mod_flags) & ShiftMask))) {
				working_text->cursor_position = 0;
			} else {
				working_text->cursor_position = 0;
				working_text->cursor_base_sel_position = 0;
			}
			redraw_required |= 0x35;
		} else if (keysym == XK_End || num_keysym == XK_KP_End) {
			if ((mod_flags & ShiftMask) && ((num_keysym != XK_KP_End) || !((num_mod_flags ^ mod_flags) & ShiftMask))) {
				working_text->cursor_position = working_text->current_bytes_used - 1;
			} else {
				working_text->cursor_position = working_text->current_bytes_used - 1;
				working_text->cursor_base_sel_position = working_text->current_bytes_used - 1;
			}
			redraw_required |= 0x35;
		}
	}
	return redraw_required;
}

unsigned int keytotext(KeySym keysym, unsigned int mod_flags) {
	unsigned int capital;
	KeySym keysym_lower;
	KeySym keysym_upper;
	
	capital = 0;
	if (mod_flags & ShiftMask) {
		capital = 1;
	}
	if (mod_flags & LockMask) {
		capital = !capital;
	}
	
	XConvertCase(keysym, &keysym_lower, &keysym_upper);
	if (capital) {
		keysym = keysym_upper;
	} else {
		keysym = keysym_lower;
	}
	
	if (keysym >= ' ' && keysym <= '~') {
		// Correct by offset the keysym to the ascii text equivalent.  In this case, the offset is 0x0.
		return keysym;
	} else if ((keysym >= XK_KP_Multiply && keysym <= XK_KP_9) || keysym == XK_KP_Equal) {
		// Correct by offset the keysym to the ascii text equivalent.  In this case, the offset is 0xFF80.
		return keysym - 0xFF80;
	}
	
	return 0;
}
