#include "./elements.h"
#include "./elements/button.h"
#include "./elements/container.h"
#include "./elements/checkbox.h"
#include "./elements/ml_textbox.h"
#include "./elements/radiobutton.h"
#include "./elements/tab.h"
#include "./elements/textbox.h"


/*
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
	struct MTK_WinElement *children;
	unsigned int child_count;
};

#define EL_BUTTON 1
#define EL_CHECKBOX 2
#define EL_CONTAINER 3
#define EL_ML_TEXTBOX 4
#define EL_RADIOBUTTON 5
#define EL_TAB 6
#define EL_TEXTBOX 7

void draw_button(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
void draw_checkbox(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
void draw_ml_textbox(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
void draw_radiobutton(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
void draw_tab(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
void draw_textbox(struct MTK_WinElement *element, struct MTK_WinBase *window) {
}
*/
void set_pixel_element_map(signed int x, signed int y, unsigned int width, unsigned int height, struct MTK_WinBase *window, struct MTK_WinElement *element){
	int i = x;
	width += x;
	height += y;
	//printf("Set the pixel element map: x-%d", x);
	//printf(" y-%d", y);
	//printf(" width-%d", width);
	//printf(" height-%d", height);
	//printf(" element-0x%LX\n", (long long int)element);
	while (y < height && y < window->height && y >= 0) {
		x = i;
		while (x < width && x < window->width && x >= 0) {
			//printf("pixel_set: x-%d", x);
			//printf(" y-%d", y);
			//printf(" element-0x%LX\n", (long long int)element);
			window->mouse_state.pixel_element_map[y * window->width + x] = element;
			x++;
		}
		y++;
	}
	return;
}

void draw_element(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y, signed int computed_width) {
	//printf("draw_element(element, window, %d", x);
	//printf(", %d", y);
	//printf(", %d)\n", computed_width);
	unsigned int i = 0;
	unsigned int var_width_element_count = 0;
	if (element->parent == 0) {
		// Flush the pixel element map
		set_pixel_element_map(0, 0, window->width, window->height, window, 0);
	}
	if (element->width != -1) {
		computed_width = element->width;
	}
	if (element->type == EL_CONTAINER) {
		draw_container(element, window, x, y, computed_width);
		while (i < element->child_count) {
			if (element->children[i]->width != -1) {
				computed_width -= element->children[i]->width;
			} else {
				var_width_element_count++;
			}
			i++;
		}
		if (computed_width < 0 || var_width_element_count == 0) {
			computed_width = 0;
		} else {
			computed_width /= var_width_element_count;
		}
		i = 0;
		while (i < element->child_count) {
			if (element->children[i]->width == -1) {
				draw_element(element->children[i], window, x + element->x, y + element->y, computed_width);
			} else {
				draw_element(element->children[i], window, x + element->x, y + element->y, element->children[i]->width);
			}
			i++;
		}
	} else {
		if (element->type == EL_BUTTON) {
			draw_button(element, window, x, y, computed_width);
		}
		if (element->type == EL_CHECKBOX) {
			draw_checkbox(element, window, x, y, computed_width);
		}
		if (element->type == EL_ML_TEXTBOX) {
			draw_ml_textbox(element, window, x, y, computed_width);
		}
		if (element->type == EL_RADIOBUTTON) {
			draw_radiobutton(element, window, x, y, computed_width);
		}
		if (element->type == EL_TAB) {
			draw_tab(element, window, x, y, computed_width);
		}
		if (element->type == EL_TEXTBOX) {
			draw_textbox(element, window, x, y, computed_width);
		}
	}
	return;
}
