#ifndef _insertion_elements_h
#define _insertion_elements_h

#define el_button 1
#define el_checkbox 2
#define el_container 3
#define el_ml_textbox 4
#define el_radiobutton 5
#define el_tab 6
#define el_textbox 7

struct element{
	unsigned int id;
	int x;
	int y;
	int width;
	int height;
	int mouse_state;
	int type;
	void* type_spec;
	void* parent;
	void** children;
};

#endif
