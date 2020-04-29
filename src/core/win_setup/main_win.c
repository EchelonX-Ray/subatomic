#include "./../../lib/wincomp/elements.h"
#include <stdlib.h>

/*
The General Element struct definition.
Defined in: src/lib/wincomp/elements.h

struct MTK_WinElement {
	unsigned int id;
	signed int width;
	signed int height;
	unsigned int mouse_state;
	unsigned int type;
	void* type_spec;
	void* parent;
	struct MTK_WinElAnchor *anchors;
	unsigned int anchor_count;
	struct MTK_WinElement **children;
	unsigned int child_count;
	
	signed int _internal_computed_xoffset;
	signed int _internal_computed_yoffset;
	signed int _internal_computed_width;
	signed int _internal_computed_height;
};
*/

/*
The Element Anchor struct definition.
Defined in: src/lib/wincomp/elements.h

struct MTK_WinElAnchor {
	signed int x_offset;
	signed int y_offset;
	unsigned int flags;
	struct MTK_WinElement *relative_to;
};
*/

/*
The Element Anchor bit flag definitions.
Defined in: src/lib/wincomp/elements.h

#define EL_AC_XAXIS 1 // This Anchor is used to align on the X-Axis
#define EL_AC_YAXIS 2 // This Anchor is used to align on the Y-Axis
#define EL_AC_RELATIVE_LEFT 0 // This Anchor's x offset is relative to the left side of the element it is anchored to
#define EL_AC_RELATIVE_RIGHT 4 // This Anchor's x offset is relative to the right side of the element it is anchored to
#define EL_AC_RELATIVE_TOP 0 // This Anchor's y offset is relative to the top side of the element it is anchored to
#define EL_AC_RELATIVE_BOTTOM 8 // This Anchor's y offset is relative to the bottom side of the element it is anchored to
#define EL_AC_ELEMENT_LEFT 0 // This Anchor's x offset is relative from the left side of the element it is assigned to
#define EL_AC_ELEMENT_RIGHT 16 // This Anchor's x offset is relative from the right side of the element it is assigned to
#define EL_AC_ELEMENT_TOP 0 // This Anchor's y offset is relative from the top side of the element it is assigned to
#define EL_AC_ELEMENT_BOTTOM 32 // This Anchor's y offset is relative from the bottom side of the element it is assigned to
*/

// This function sets up and initializes the window element tree.
// I put this setup in this seperate function to keep the code clean and organized.
// This function is only called once by from the main function at the beginning of the program.
void setup_main_win_elements(struct MTK_WinElement **element, struct MTK_WinFontPack *font_pack) {
	// Setup Easy Pointer Reference
	struct MTK_WinElement *root_cont = *element;
	
	// Setup Root Container
	struct EL_container_t *root_cont_spec = calloc(1, sizeof(struct EL_container_t)); // Allocate the memory for the type specific struct.
	root_cont->type_spec = root_cont_spec; // Set the pointer to the type specific struct
	root_cont->id = 0; // Set the element ID
	root_cont->width = -1; // Set the width to "scale-to-fit"
	root_cont->height = -1; // Set the height to "scale-to-fit"
	root_cont->mouse_state = EL_MS_NORMAL; // Initialize the mouse state to "Normal"
	root_cont->type = EL_CONTAINER; // Set the type
	root_cont->parent = 0; // Set the parent element pointer.  This is the parent element, so set it to NULL.
	root_cont->children = 0; // Initialize the pointer to the array of pointers of this element's children to NULL.  This is unnecessary because this value will be updated below.
	root_cont->child_count = 0; // Initialize the child element count to NULL.  This is unnecessary because this value will be updated below.
	root_cont->anchors = 0; // Initialize the pointer to the array of this element's anchors to NULL.  This is unnecessary because this value will be updated below.
	root_cont->anchor_count = 0; // Initialize the element's anchor count to NULL.  This is unnecessary because this value will be updated below.
	
	// Setup Root Container Elements
	struct MTK_WinElement **root_cont_child_list = calloc(2, sizeof(struct MTK_WinElement*)); // Allocate the memory for the array of pointers to child elements.
	root_cont->child_count = 2; // Set how many children this element has.
	root_cont->children = root_cont_child_list; // Set the pointer to the array.
	struct MTK_WinElement *textbox = calloc(1, sizeof(struct MTK_WinElement)); // Allocate the memory for the Textbox element.
	textbox->type_spec = calloc(1, sizeof(struct EL_textbox_t)); // Allocate the memory for the Textbox type specific element.
	struct MTK_WinElement *button = calloc(1, sizeof(struct MTK_WinElement)); // Allocate the memory for the Button element.
	button->type_spec = calloc(1, sizeof(struct EL_button_t)); // Allocate the memory for the Button type specific element.
	// Setup the array of children
	root_cont_child_list[0] = button;
	root_cont_child_list[1] = textbox;
	
	/*
	The type specific struct definition of Textbox elements.
	Defined in: src/lib/wincomp/elements/textbox.h
	
	struct EL_textbox_t {
		uint32_t bg_color;
		uint32_t text_color;
		uint32_t def_text_color;
		uint32_t border_color;
		char *text;
		char *def_text;
		unsigned int alloc_unit;
		unsigned int current_alloc_unit_count;
		unsigned int current_bytes_used;
		unsigned int cursor_position;
		unsigned int cursor_selection_base_position;
		unsigned int text_drawing_offset;
		struct MTK_WinFontMap *fontmap;
	};
	*/
	
	// Setup the Textbox type specific information
	struct EL_textbox_t *textbox__type_spec;
	textbox__type_spec = textbox->type_spec;
	textbox__type_spec->bg_color = 0x00FFFFFF; // Background Color
	textbox__type_spec->text_color = 0x00000000; // Text Color
	textbox__type_spec->def_text_color = 0x66000000; // Default Text Color
	textbox__type_spec->border_color = 0x00FF0000; // Border Color
	// Allocate and set the memory for an empty textbox cstr.  
	// This must be done on the heap because it will be automatically 
	// freed and reallocated by the wincomp library.
	textbox__type_spec->alloc_unit = 5;
	textbox__type_spec->current_alloc_unit_count = 1;
	textbox__type_spec->current_bytes_used = 1;
	textbox__type_spec->text = calloc(textbox__type_spec->alloc_unit * textbox__type_spec->current_alloc_unit_count, sizeof(char));
	textbox__type_spec->def_text = "Default Text"; // Set the Default Text
	textbox__type_spec->cursor_position = 0; // Initialize the cursor position to 0.
	textbox__type_spec->cursor_selection_base_position = textbox__type_spec->cursor_position; // Initialize the selection base cursor position to match the cursor position.
	textbox__type_spec->text_drawing_offset = 0; // Initialize the textbox drawing offset to 0.
	textbox__type_spec->fontmap = &(font_pack->font_style[0]); // Set the font that the textbox should use
	// Setup the Textbox general information
	textbox->id = 1; // Set the element ID
	textbox->width = -1; // Set the width to "scale-to-fit"
	textbox->height = 50; // Set the height to 50 pixels
	textbox->mouse_state = EL_MS_NORMAL; // Initialize the mouse state to "Normal"
	textbox->type = EL_TEXTBOX; // Set the type
	textbox->parent = root_cont; // Set the pointer to the parent element
	textbox->children = 0; // This element has no children.  Set the array pointer to NULL.
	textbox->child_count = 0; // This element has no children.  Set the child count to 0.
	// Setup the Textbox anchors
	textbox->anchor_count = 1; // This element has 1 anchor.
	struct MTK_WinElAnchor *textbox_anchors = calloc(textbox->anchor_count, sizeof(struct MTK_WinElAnchor)); // Allocate the memory for the anchor array
	textbox_anchors[0].x_offset = 0; // Set the anchor position to 0 pixels on the x coordinate.
	textbox_anchors[0].y_offset = 100; // Set the anchor position to 100 pixels on the y coordinate.
	textbox_anchors[0].relative_to = root_cont; // Set the element that this anchor is relative to
	// Set the anchor point to have the following traits: 
	//  - Applies to the X-AXIS
	//  - Applies to the Y-AXIS
	//  - Is relative to the left side of this element
	//  - Is relative to the left side of the element it is relative to
	//  - Is relative to the top side of this element
	//  - Is relative to the top side of the element it is relative to
	textbox_anchors[0].flags = EL_AC_XAXIS | EL_AC_YAXIS | EL_AC_RELATIVE_LEFT | EL_AC_RELATIVE_TOP | EL_AC_ELEMENT_LEFT | EL_AC_ELEMENT_TOP;
	// Set the pointer to the anchor array to the Textbox element
	textbox->anchors = textbox_anchors;
	
	/*
	The type specific struct definition of Button elements.
	Defined in: src/lib/wincomp/elements/button.h
	
	struct EL_button_t {
		uint32_t bg_color;
		uint32_t bg_hover_color;
		uint32_t bg_down_color;
		uint32_t text_color;
		uint32_t text_hover_color;
		uint32_t text_down_color;
		uint32_t border_color;
		char *text;
		struct MTK_WinFontMap *fontmap;
	};
	*/
	
	// Setup the Button type specific information
	struct EL_button_t *button__type_spec;
	button__type_spec = button->type_spec;
	button__type_spec->bg_color = 0xAAFFFFFF; // Background Color
	button__type_spec->bg_hover_color = 0x00FFFFFF; // Background Mouse-Hover Color
	button__type_spec->bg_down_color = 0x0000FF00; // Background Mouse-Down Color
	button__type_spec->text_color = 0x00000000; // Text Color
	button__type_spec->text_hover_color = 0x000000FF; // Text Mouse-Hover Color
	button__type_spec->text_down_color = 0x00000000; // Text Mouse-Down Color
	button__type_spec->border_color = 0x00FF0000; // Border Color
	button__type_spec->text = "Button Text"; // Button Text
	button__type_spec->fontmap = &(font_pack->font_style[0]); // Set the font that the button should use
	// Setup the Button general information
	button->id = 2; // Set the element ID
	button->width = 300; // Set the width to 300 pixels
	button->height = 75; // Set the height to 75 pixels
	button->mouse_state = EL_MS_NORMAL; // Initialize the mouse state to "Normal"
	button->type = EL_BUTTON; // Set the type
	button->parent = root_cont; // Set the pointer to the parent element
	button->children = 0; // This element has no children.  Set the array pointer to NULL.
	button->child_count = 0; // This element has no children.  Set the child count to 0.
	// Setup the Button anchors
	button->anchor_count = 1; // This element has 1 anchor.
	struct MTK_WinElAnchor *button_anchors = calloc(1, sizeof(struct MTK_WinElAnchor)); // Allocate the memory for the anchor array
	button_anchors[0].x_offset = 15; // Set the anchor position to 15 pixels on the x coordinate.
	button_anchors[0].y_offset = 5; // Set the anchor position to 5 pixels on the y coordinate.
	button_anchors[0].relative_to = root_cont; // Set the element that this anchor is relative to
	// Set the anchor point to have the following traits: 
	//  - Applies to the X-AXIS
	//  - Applies to the Y-AXIS
	//  - Is relative to the left side of this element
	//  - Is relative to the left side of the element it is relative to
	//  - Is relative to the top side of this element
	//  - Is relative to the top side of the element it is relative to
	button_anchors[0].flags = EL_AC_XAXIS | EL_AC_YAXIS | EL_AC_RELATIVE_LEFT | EL_AC_RELATIVE_TOP | EL_AC_ELEMENT_LEFT | EL_AC_ELEMENT_TOP;
	// Set the pointer to the anchor array to the Button element
	button->anchors = button_anchors;
	return;
}

// Free up the memory that was allocated for the element tree in the above function.
// This should be called before the program exits.
// Since we don't actually care about setting up specific values per element, like above, 
// we can just parse the tree and free the memory.
void free_element_tree(struct MTK_WinElement *root_cont) {
	if (root_cont == 0) { // Check for null pointer
		return;
	}
	
	// Child parsing
	unsigned int i;
	if (root_cont->children != 0) { // Check for null pointer
		i = 0;
		while (root_cont->child_count > 0) { // Iterate through children
			// Recur to parse the branch of the tree
			free_element_tree(root_cont->children[i]);
			root_cont->child_count--; // Remove the freed child from count
			i++; // Increment the index
		}
		free(root_cont->children); // Free the array of pointers
	}
	
	// Free general element values
	// Free the array of anchors
	if (root_cont->anchors != 0 && root_cont->anchor_count > 0) { // Check for null pointer and positive count
		free(root_cont->anchors);
	}
	
	// Free type specific element values
	if (root_cont->type_spec != 0) { // Check for null pointer
		// Deterine the element type and free any applicable type specific values
		if (root_cont->type == EL_TEXTBOX) { // This is a textbox
			// Get the type specific values
			struct EL_textbox_t *textbox_t;
			textbox_t = (struct EL_textbox_t*)root_cont->type_spec;
			
			// Free the text value if non-null.
			if (textbox_t->text != 0) {
				free(textbox_t->text);
			}
		}
		
		// Free the type specific struct
		free(root_cont->type_spec);
	}
	
	// Free the general element struct
	free(root_cont);
	
	return;
}
