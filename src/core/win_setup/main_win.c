#include "./../../lib/wincomp/elements.h"
#include <stdlib.h>

/*
struct MTK_WinElement {
	unsigned int id;
	signed int x;
	signed int y;
	unsigned int width;
	unsigned int height;
	unsigned int mouse_state;
	unsigned int type;
	void* type_spec;
	void* parent;
	struct MTK_WinElAnchor *anchors;
	unsigned int anchor_count;
	struct MTK_WinElement **children;
	unsigned int child_count;
	
	signed int computed_x;
	signed int computed_y;
	signed int computed_width;
	signed int computed_height;
	
	char *text;
	uint32_t fgcolor;
	uint32_t bgcolor;
};

struct MTK_WinElAnchor {
	signed int x_offset;
	signed int y_offset;
	unsigned int flags;
	struct MTK_WinElement *relative_to;
};
*/

//#define EL_AC_XAXIS 1; // This Anchor is used to align on the X-Axis
//#define EL_AC_YAXIS 2; // This Anchor is used to align on the Y-Axis
//#define EL_AC_RELATIVE_LEFT 0; // This Anchor's x offset is relative to the left side of the element it is anchored to
//#define EL_AC_RELATIVE_RIGHT 4; // This Anchor's x offset is relative to the right side of the element it is anchored to
//#define EL_AC_RELATIVE_TOP 0; // This Anchor's y offset is relative to the top side of the element it is anchored to
//#define EL_AC_RELATIVE_BOTTOM 8; // This Anchor's y offset is relative to the bottom side of the element it is anchored to
//#define EL_AC_ELEMENT_LEFT 0; // This Anchor's x offset is relative from the left side of the element it is assigned to
//#define EL_AC_ELEMENT_RIGHT 16; // This Anchor's x offset is relative from the right side of the element it is assigned to
//#define EL_AC_ELEMENT_TOP 0; // This Anchor's y offset is relative from the top side of the element it is assigned to
//#define EL_AC_ELEMENT_BOTTOM 32; // This Anchor's y offset is relative from the bottom side of the element it is assigned to

void setup_main_win_elements(struct MTK_WinElement **element, struct MTK_WinFontPack *font_pack) {
	// Setup Easy Pointer Reference
	struct MTK_WinElement *root_cont = *element;
	
	// Setup Root Container
	struct EL_container_t *root_cont_spec = calloc(1, sizeof(struct EL_container_t));
	root_cont->type_spec = root_cont_spec;
	root_cont->id = 0;
	root_cont->width = -1;
	root_cont->height = -1;
	root_cont->mouse_state = EL_MS_NORMAL;
	root_cont->type = EL_CONTAINER;
	root_cont->parent = 0;
	root_cont->children = 0;
	root_cont->child_count = 0;
	root_cont->anchors = 0;
	root_cont->anchor_count = 0;
	
	// Setup Root Container Elements
	struct MTK_WinElement **root_cont_child_list = calloc(2, sizeof(struct MTK_WinElement*));
	root_cont->child_count = 2;
	root_cont->children = root_cont_child_list;
	struct MTK_WinElement *textbox = calloc(1, sizeof(struct MTK_WinElement));
	textbox->type_spec = calloc(1, sizeof(struct EL_textbox_t));
	struct MTK_WinElement *button = calloc(1, sizeof(struct MTK_WinElement));
	button->type_spec = calloc(1, sizeof(struct EL_button_t));
	root_cont_child_list[0] = button;
	root_cont_child_list[1] = textbox;
	textbox->id = 1;
	textbox->width = 0;
	textbox->height = 0;
	textbox->mouse_state = EL_MS_NORMAL;
	textbox->type = EL_TEXTBOX;
	textbox->parent = root_cont;
	textbox->children = 0;
	textbox->child_count = 0;
	textbox->anchors = 0;
	textbox->anchor_count = 0;
	/*
	struct EL_button_t {
		uint32_t color;
		uint32_t hover_color;
		uint32_t down_color;
		uint32_t text_color;
		uint32_t text_hover_color;
		uint32_t text_down_color;
		uint32_t border_color;
		char *text;
		struct MTK_WinFontMap *fontmap;
	};
		type_spec_default.color = 0xAAFFFFFF;
		type_spec_default.hover_color = 0x00FFFFFF;
		type_spec_default.down_color = 0x0000FF00;
		type_spec_default.text_color = 0x00000000;
		type_spec_default.text_hover_color = 0x000000FF;
		type_spec_default.text_down_color = 0x00000000;
		type_spec_default.border_color = 0x00FF0000;
		type_spec_default.text = "Default Text";
		type_spec_default.fontmap = 0;
	*/
	struct EL_button_t *button__type_spec;
	button__type_spec = button->type_spec;
	button__type_spec->color = 0xAAFFFFFF;
	button__type_spec->hover_color = 0x00FFFFFF;
	button__type_spec->down_color = 0x0000FF00;
	button__type_spec->text_color = 0x00000000;
	button__type_spec->text_hover_color = 0x000000FF;
	button__type_spec->text_down_color = 0x00000000;
	button__type_spec->border_color = 0x00FF0000;
	button__type_spec->text = "Button Text";
	button__type_spec->fontmap = &(font_pack->font_style[0]);
	button->id = 2;
	button->width = 300;
	button->height = 75;
	button->mouse_state = EL_MS_NORMAL;
	button->type = EL_BUTTON;
	button->parent = root_cont;
	button->children = 0;
	button->child_count = 0;
	struct MTK_WinElAnchor *button_anchors = calloc(1, sizeof(struct MTK_WinElAnchor));
	button_anchors->x_offset = 15;
	button_anchors->y_offset = 5;
	button_anchors->flags = EL_AC_XAXIS | EL_AC_YAXIS | EL_AC_RELATIVE_LEFT | EL_AC_RELATIVE_TOP | EL_AC_ELEMENT_LEFT | EL_AC_ELEMENT_TOP;
	button_anchors->relative_to = root_cont;
	button->anchors = button_anchors;
	button->anchor_count = 1;
	return;
}

void free_element_tree(struct MTK_WinElement *root_cont){
	if (root_cont == 0) {
		return;
	}
	unsigned int i = 0;
	if (root_cont->children != 0) {
		while (root_cont->child_count > 0) {
			free_element_tree(root_cont->children[i]);
			root_cont->child_count--;
			i++;
		}
		free(root_cont->children);
	}
	if (root_cont->anchors != 0 && root_cont->anchor_count > 0) {
		free(root_cont->anchors);
	}
	if (root_cont->type_spec != 0) {
		free(root_cont->type_spec);
	}
	free(root_cont);
	return;
}
