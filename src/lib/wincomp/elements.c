#include "./elements.h"
#include <stdlib.h>

#include "./../toolbox/cstr_manip.h"

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

// Find the x divisor
unsigned int walk_x_chain_get_div(struct MTK_WinElement *element, struct MTK_WinElement *from) {
	unsigned int count = 0;
	unsigned int i = 0;
	if (element->_internal_computed_width == -1) {
		count++;
	}
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0 && from != element->anchors[i].relative_to) {
			count += walk_x_chain_get_div(element->anchors[i].relative_to, element);
		}
		i++;
	}
	return count;
}
// Find the y divisor
unsigned int walk_y_chain_get_div(struct MTK_WinElement *element, struct MTK_WinElement *from) {
	unsigned int count = 0;
	unsigned int i = 0;
	if (element->_internal_computed_height == -1) {
		count++;
	}
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0 && from != element->anchors[i].relative_to) {
			count += walk_y_chain_get_div(element->anchors[i].relative_to, element);
		}
		i++;
	}
	return count;
}
// Find the used width
signed int walk_x_chain_get_width(struct MTK_WinElement *element, struct MTK_WinElement *from, signed int starting_width, signed int scale_factor) {
	unsigned int i = 0;
	printf("START: walk_x_chain_get_width\n");
	printf("element->type: %d\n", element->type);
	if (element->_internal_computed_width != -1) {
		starting_width -= element->_internal_computed_width * scale_factor;
	}
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0 && from != element->anchors[i].relative_to) {
			if ((element->anchors[i].flags & EL_AC_ELEMENT_RIGHT) > 0) {
				starting_width += element->_internal_computed_width;
			}
			starting_width -= element->anchors[i].x_offset;
			if (element->anchors[i].relative_to != element->parent) {
				if ((element->anchors[i].flags & EL_AC_RELATIVE_RIGHT) > 0) {
					starting_width = walk_x_chain_get_width(element->anchors[i].relative_to, element, starting_width, 1);
				} else {
					starting_width = walk_x_chain_get_width(element->anchors[i].relative_to, element, starting_width, 0);
				}
			}
		}
		i++;
	}
	printf("starting_width: %d\n", starting_width);
	printf("END: walk_x_chain_get_width\n\n");
	return starting_width;
}
// Find the used height
signed int walk_y_chain_get_height(struct MTK_WinElement *element, struct MTK_WinElement *from, signed int starting_height, signed int scale_factor) {
	unsigned int i = 0;
	if (element->_internal_computed_height != -1) {
		starting_height -= element->_internal_computed_height * scale_factor;
	}
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0 && from != element->anchors[i].relative_to) {
			if ((element->anchors[i].flags & EL_AC_ELEMENT_BOTTOM) > 0) {
				starting_height += element->_internal_computed_height;
			}
			starting_height -= element->anchors[i].y_offset;
			if (element->anchors[i].relative_to != element->parent) {
				if ((element->anchors[i].flags & EL_AC_RELATIVE_BOTTOM) > 0) {
					starting_height = walk_y_chain_get_height(element->anchors[i].relative_to, element, starting_height, 1);
				} else {
					starting_height = walk_y_chain_get_height(element->anchors[i].relative_to, element, starting_height, 0);
				}
			}
		}
		i++;
	}
	return starting_height;
}
// Find the used x
signed int walk_x_chain_get_x(struct MTK_WinElement *element, struct MTK_WinElement *from, signed int starting_x, signed int scale_factor) {
	unsigned int i = 0;
	printf("START: walk_x_chain_get_x\n");
	printf("starting_x: %d\n", starting_x);
	printf("element->type: %d\n", element->type);
	printf("scale_factor: %d\n", scale_factor);
	if (element->_internal_computed_xoffset != -1) {
		printf("IF\n");
		starting_x -= element->_internal_computed_xoffset * scale_factor;
	}
	while (i < element->anchor_count) {
		printf("loop");
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0 && from != element->anchors[i].relative_to) {
			printf(" 1st");
			if ((element->anchors[i].flags & EL_AC_ELEMENT_RIGHT) > 0) {
				printf(" A");
				starting_x -= element->anchors[i].x_offset;
				starting_x -= element->_internal_computed_width;
			} else {
				printf(" B");
				starting_x += element->anchors[i].x_offset;
			}
			if (element->anchors[i].relative_to != element->parent) {
				printf(" C");
				if ((element->anchors[i].flags & EL_AC_RELATIVE_RIGHT) > 0) {
					printf(" D");
					starting_x = walk_x_chain_get_x(element->anchors[i].relative_to, element, starting_x, 1);
				} else {					
					printf(" E");
					starting_x = walk_x_chain_get_x(element->anchors[i].relative_to, element, starting_x, 0);
				}
			}
		}
		printf("\n");
		i++;
	}
	printf("starting_x: %d\n", starting_x);
	printf("END: walk_x_chain_get_x\n\n");
	return starting_x;
}
// Find the used y
signed int walk_y_chain_get_y(struct MTK_WinElement *element, struct MTK_WinElement *from, signed int starting_y, signed int scale_factor) {
	unsigned int i = 0;
	if (element->_internal_computed_yoffset != -1) {
		starting_y -= element->_internal_computed_yoffset * scale_factor;
	}
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0 && from != element->anchors[i].relative_to) {
			if ((element->anchors[i].flags & EL_AC_ELEMENT_RIGHT) > 0) {
				starting_y -= element->anchors[i].y_offset;
				starting_y -= element->_internal_computed_height;
			} else {
				starting_y += element->anchors[i].y_offset;
			}
			if (element->anchors[i].relative_to != element->parent) {
				if ((element->anchors[i].flags & EL_AC_RELATIVE_RIGHT) > 0) {
					starting_y = walk_y_chain_get_y(element->anchors[i].relative_to, element, starting_y, 1);
				} else {
					starting_y = walk_y_chain_get_y(element->anchors[i].relative_to, element, starting_y, 0);
				}
			}
		}
		i++;
	}
	return starting_y;
}

struct MTK_WinEl_ChainList {
	unsigned int chain_count;
	unsigned int max_independence_level;
	struct MTK_WinEl_Chain *element_chains;
};
struct MTK_WinEl_Chain {
	unsigned int independence_level;
	unsigned int element_count;
	struct MTK_WinElement **elements_in_chain;
};

unsigned int is_element_already_in_chainlist(struct MTK_WinElement *element, struct MTK_WinEl_ChainList *chainlist) {
	unsigned int i;
	unsigned int j;
	i = 0;
	while (i < chainlist->chain_count) {
		j = 0;
		while (j < chainlist->element_chains[i].element_count) {
			if (chainlist->element_chains[i].elements_in_chain[j] == element) {
				return chainlist->element_chains[i].independence_level;
			}
			j++;
		}
		i++;
	}
	return 0;
}
unsigned int get_chain_index(struct MTK_WinElement *element, struct MTK_WinEl_ChainList *chainlist){
	unsigned int i;
	unsigned int j;
	i = 0;
	while (i < chainlist->chain_count) {
		j = 0;
		while (j < chainlist->element_chains[i].element_count) {
			if (chainlist->element_chains[i].elements_in_chain[j] == element) {
				return i;
			}
			j++;
		}
		i++;
	}
	return -1;
}
unsigned int add_new_chain(struct MTK_WinEl_ChainList *chainlist) {
	struct MTK_WinEl_Chain *tmp_element_chains = chainlist->element_chains;
	chainlist->element_chains = malloc((chainlist->chain_count + 1) * sizeof(struct MTK_WinEl_Chain));
	cmemcpy(tmp_element_chains, chainlist->element_chains, chainlist->chain_count * sizeof(struct MTK_WinEl_Chain));
	chainlist->element_chains[chainlist->chain_count].independence_level = 0;
	chainlist->element_chains[chainlist->chain_count].element_count = 0;
	chainlist->element_chains[chainlist->chain_count].elements_in_chain = 0;
	chainlist->chain_count++;
	free(tmp_element_chains);
	return chainlist->chain_count - 1;
}
void add_element_to_chain(struct MTK_WinElement *element, struct MTK_WinEl_ChainList *chainlist, unsigned int chain_index) {
	struct MTK_WinElement **tmp_elements_in_chain = chainlist->element_chains[chain_index].elements_in_chain;
	chainlist->element_chains[chain_index].elements_in_chain = malloc((chainlist->element_chains[chain_index].element_count + 1) * sizeof(struct MTK_WinElement*));
	cmemcpy(tmp_elements_in_chain, chainlist->element_chains[chain_index].elements_in_chain, chainlist->element_chains[chain_index].element_count * sizeof(struct MTK_WinElement*));
	chainlist->element_chains[chain_index].elements_in_chain[chainlist->element_chains[chain_index].element_count] = element;
	chainlist->element_chains[chain_index].element_count++;
	free(tmp_elements_in_chain);
	return;
}
void update_x_chains(struct MTK_WinElement *element, struct MTK_WinElement *from, struct MTK_WinEl_ChainList *chainlist, unsigned int independence_level) {
	unsigned int new_chain_required;
	unsigned int element_chain_independence_level;
	unsigned int chain_index;
	unsigned int i;
	
	// Check if this element is anchored to the previous element.  If not, it means that a one-way link was followed.
	// In the case of a one-way link, increment the independence_level because this element is not bound to an element of the previous chain.
	new_chain_required = 1;
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0) { // This function is only looking for anchors which deal with the X-Axis.
			if (element->anchors[i].relative_to == from) {
				new_chain_required = 0;
			}
		}
		i++;
	}
	// Increment the independence_level if a one-way link was followed.
	if (new_chain_required) {
		independence_level++;
		if (chainlist->max_independence_level < independence_level) {
			chainlist->max_independence_level = independence_level;
		}
	}
	
	// Update the chain
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0) { // This function is only looking for anchors which deal with the X-Axis.
			// Only update existing elements
			element_chain_independence_level = is_element_already_in_chainlist(element, chainlist);
			if (element_chain_independence_level > 0) {
				chain_index = get_chain_index(element, chainlist);
				chainlist->element_chains[chain_index].independence_level = independence_level; // Update the independence_level
				if (element->anchors[i].relative_to != element->parent && element->anchors[i].relative_to != from) { // Don't recur into elements which aren't siblings and don't backtrack
					update_x_chains(element->anchors[i].relative_to, element, chainlist, independence_level);
				}
			}
		}
		i++;
	}
	return;
}

// This function works through relatively anchored elements on the x-axis and builds a chainlist.
// The chainlist contains a list of chains.  The chains describe elements who's positioning depends on each other.
// Some elements can depend on the positioning of elements in other chains.  However, the relationship is non-reciprocal.
// This means that there is a hierarchy of chains.  This relationship is refered to in the comments of this function as a one-way link.
// Chains with a higher independence_level are higher in the hierarchy.  The chains are then parsed from high to low independence_level.
// This allows elements that depend on the positioning of elements in the higher independence_level chains to refer to them.
// The independence_level grows upward during this function's execution.  The maximum value reached is stored in max_independence_level.
// The lowest valid value of independence_level is 1.  That is where it starts.
void find_x_chains(struct MTK_WinElement *element, struct MTK_WinElement *from, struct MTK_WinEl_ChainList *chainlist, unsigned int chain_index, unsigned int independence_level) {
	unsigned int new_chain_required;
	unsigned int element_chain_independence_level;
	unsigned int i;
	
	// Check if this element is anchored to the previous element.  If not, it means that a one-way link was followed.
	// In the case of a one-way link, a new chain should be created because this element is not bound to an element of the previous chain.
	// This check will also be tripped if from == 0, such as when this function is first called from compute_element_internals(), to create the first chain.
	new_chain_required = 1;
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0) { // This function is only looking for anchors which deal with the X-Axis.
			if (element->anchors[i].relative_to == from) {
				new_chain_required = 0;
			}
		}
		i++;
	}
	// Create a new chain if a one-way link was followed.
	if (new_chain_required) {
		independence_level++;
		chain_index = add_new_chain(chainlist);
		chainlist->element_chains[chain_index].independence_level = independence_level;
		if (chainlist->max_independence_level < independence_level) {
			chainlist->max_independence_level = independence_level;
		}
	}
	
	// Build the chain
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_XAXIS) > 0) { // This function is only looking for anchors which deal with the X-Axis.
			// Don't duplicate entries to the chainlist and update independence_levels as needed
			element_chain_independence_level = is_element_already_in_chainlist(element, chainlist);
			if (element_chain_independence_level == 0) {
				add_element_to_chain(element, chainlist, chain_index);
				if (element->anchors[i].relative_to != element->parent && element->anchors[i].relative_to != from) { // Don't recur into elements which aren't siblings and don't backtrack
					find_x_chains(element->anchors[i].relative_to, element, chainlist, chain_index, independence_level);
				}
			} else {
				// Do does the element in the chainlist have a lower chain independence_level than the currently computed independence_level
				if (element_chain_independence_level < independence_level) {
					// If so, increase all the relevant chain independence_levels to match the currently computed one.
					// This requires that the element be recomputed and parsed with the current independence_level
					update_x_chains(element, from, chainlist, independence_level);
				}
			}
		}
		i++;
	}
	return;
}

// Print DEBUGGING Info - Remove on production release
void print_chainlist(struct MTK_WinEl_ChainList *chainlist) {
	unsigned int i;
	unsigned int j;
	printf("\n");
	printf("ChainList chain_count: %d\n", chainlist->chain_count);
	printf("ChainList max_independence_level: %d\n\n", chainlist->max_independence_level);
	i = 0;
	while (i < chainlist->chain_count) {
		printf("Chain Number: %d\n", i);
		printf("Chain independence_level: %d\n", chainlist->element_chains[i].independence_level);
		printf("Chain element_count: %d\n\n", chainlist->element_chains[i].element_count);
		j = 0;
		while (j < chainlist->element_chains[i].element_count) {
			printf("Element Number: %d\n", j);
			printf("Element id: %d\n", chainlist->element_chains[i].elements_in_chain[j]->id);
			printf("Element _internal_computed_xoffset: %d\n", chainlist->element_chains[i].elements_in_chain[j]->_internal_computed_xoffset);
			printf("Element _internal_computed_yoffset: %d\n", chainlist->element_chains[i].elements_in_chain[j]->_internal_computed_yoffset);
			printf("Element _internal_computed_width: %d\n", chainlist->element_chains[i].elements_in_chain[j]->_internal_computed_width);
			printf("Element _internal_computed_height: %d\n\n", chainlist->element_chains[i].elements_in_chain[j]->_internal_computed_height);
			j++;
		}
		i++;
	}
	return;
}

void parse_chain_widthheight(struct MTK_WinEl_Chain *chain, unsigned int parent_width, unsigned int parent_height) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	signed int x_var_el_count = parent_width;
	signed int y_var_el_count = parent_height;
	unsigned int x_divisor = 0;
	unsigned int y_divisor = 0;
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		if (element->width >= 0) {
			element->_internal_computed_width = element->width;
			x_var_el_count -= element->width;
		} else {
			x_divisor++;
		}
		j = 0;
		while (j < element->anchor_count) {
			if ((element->anchors[i].flags & EL_AC_XAXIS) > 0) {
				x_var_el_count -= element->anchors[i].x_offset;
			}
			j++;
		}
		
		/*
		if (element->height >= 0) {
			element->_internal_computed_height = element->height;
			y_var_el_count -= element->height;
		} else {
			y_divisor++;
		}
		j = 0;
		while (j < element->anchor_count) {
			if ((element->anchors[i].flags & EL_AC_YAXIS) > 0) {
				y_var_el_count -= element->anchors[i].y_offset;
			}
			j++;
		}
		*/
		i++;
	}
	if (x_var_el_count < 0) {
		x_var_el_count = 0;
	}
	if (x_divisor > 0) {
		x_var_el_count /= x_divisor;
	}
	/*
	if (y_var_el_count < 0) {
		y_var_el_count = 0;
	}
	if (y_divisor > 0) {
		y_var_el_count /= y_divisor;
	}
	*/
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		if (element->width < 0) {
			element->_internal_computed_width = x_var_el_count;
		}
		/*
		if (element->height < 0) {
			element->_internal_computed_height = y_var_el_count;
		}
		*/
		i++;
	}
	
	return;
}
void parse_chain_xy(struct MTK_WinEl_Chain *chain, unsigned int parent_width, unsigned int parent_height) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	unsigned int k;
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		k = 0;
		//while(k < (element->anchor_count * 2)) {
		while(k < (element->anchor_count * 1)) {
			j = 0;
			while (j < element->anchor_count) {
				if (element->_internal_computed_xoffset == -1) {
					if ((element->anchors[i].flags & EL_AC_XAXIS) > 0 && element->anchors[i].relative_to->_internal_computed_xoffset != -1) {
						// Relative
						if ((element->anchors[i].flags & EL_AC_RELATIVE_RIGHT) > 0) {
							if ((element->anchors[i].flags & EL_AC_ELEMENT_RIGHT) > 0) {
								element->_internal_computed_xoffset = \
									element->anchors[i].relative_to->_internal_computed_xoffset + \
									element->anchors[i].relative_to->_internal_computed_width - \
									element->_internal_computed_width - \
									element->anchors[i].x_offset;
							} else {
								element->_internal_computed_xoffset = \
									element->anchors[i].relative_to->_internal_computed_xoffset + \
									element->anchors[i].relative_to->_internal_computed_width + \
									element->anchors[i].x_offset;
							}
						} else {
							if ((element->anchors[i].flags & EL_AC_ELEMENT_RIGHT) > 0) {
								element->_internal_computed_xoffset = \
									element->anchors[i].relative_to->_internal_computed_xoffset - \
									element->_internal_computed_width - \
									element->anchors[i].x_offset;
							} else {
								element->_internal_computed_xoffset = \
									element->anchors[i].relative_to->_internal_computed_xoffset + \
									element->anchors[i].x_offset;
							}
						}
						k++;
					}
				}
				if (element->_internal_computed_yoffset == -1 && 0) {
				//if (element->_internal_computed_yoffset == -1) {
					if ((element->anchors[i].flags & EL_AC_YAXIS) > 0 && element->anchors[i].relative_to->_internal_computed_yoffset != -1) {
						// Relative
						if ((element->anchors[i].flags & EL_AC_RELATIVE_BOTTOM) > 0) {
							if ((element->anchors[i].flags & EL_AC_ELEMENT_BOTTOM) > 0) {
								element->_internal_computed_yoffset = \
									element->anchors[i].relative_to->_internal_computed_yoffset + \
									element->anchors[i].relative_to->_internal_computed_height - \
									element->_internal_computed_height - \
									element->anchors[i].y_offset;
							} else {
								element->_internal_computed_yoffset = \
									element->anchors[i].relative_to->_internal_computed_yoffset + \
									element->anchors[i].relative_to->_internal_computed_height + \
									element->anchors[i].y_offset;
							}
						} else {
							if ((element->anchors[i].flags & EL_AC_ELEMENT_BOTTOM) > 0) {
								element->_internal_computed_yoffset = \
									element->anchors[i].relative_to->_internal_computed_yoffset - \
									element->_internal_computed_height - \
									element->anchors[i].y_offset;
							} else {
								element->_internal_computed_yoffset = \
									element->anchors[i].relative_to->_internal_computed_yoffset + \
									element->anchors[i].y_offset;
							}
						}
						k++;
					}
				}
				j++;
			}
			i++;
		}
	}
	
	return;
}

void compute_element_internals(struct MTK_WinBase *window) {
	struct MTK_WinEl_ChainList x_chainlist;
	struct MTK_WinEl_ChainList y_chainlist;
	unsigned int i;
	unsigned int j;
	x_chainlist.chain_count = 0;
	x_chainlist.max_independence_level = 0;
	x_chainlist.element_chains = 0;
	y_chainlist.chain_count = 0;
	y_chainlist.max_independence_level = 0;
	y_chainlist.element_chains = 0;
	i = 0;
	while (i < window->root_element->child_count) {
		window->root_element->children[i]->_internal_computed_xoffset = -1;
		//window->root_element->children[i]->_internal_computed_yoffset = -1;
		find_x_chains(window->root_element->children[i], 0, &x_chainlist, 0, 0);
		//find_y_chains(window->root_element->children[i], 0, &y_chainlist, 0, 0);
		// TODO: find_y_chains()
		i++;
	}
	j = x_chainlist.max_independence_level;
	while (j > 0) {
		i = 0;
		while (i < x_chainlist.chain_count) {
			if (x_chainlist.element_chains[i].independence_level == j) {
				parse_chain_widthheight(x_chainlist.element_chains + i, window->width, window->height);
				parse_chain_xy(x_chainlist.element_chains + i, window->width, window->height);
			}
			i++;
		}
		j--;
	}
	// TODO: Recur into the tree of children
	print_chainlist(&x_chainlist);
	exit(3);
	return;
}

void draw_parent_element(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y) {
	unsigned int use_width;
	unsigned int use_height;
	if (element->width != -1) {
		use_width = element->width;
	} else {
		use_width = element->_internal_computed_width;
	}
	if (element->height != -1) {
		use_height = element->height;
	} else {
		use_height = element->_internal_computed_height;
	}
	
	unsigned int i;
	unsigned int dev_unit;
	signed int dev_long;
	i = 0;
	while (i < element->child_count) {
		if (element->children[i]->width != -1) {
			element->children[i]->_internal_computed_width = element->children[i]->width;
		} else {
			element->children[i]->_internal_computed_width = -1;
			dev_long = walk_x_chain_get_width(element->children[i], 0, use_width, 1);
			if (dev_long < 0) {
				dev_long = 0;
			}
			dev_unit = walk_x_chain_get_div(element->children[i], 0);
			element->children[i]->_internal_computed_width = dev_long / dev_unit;
		}
		if (element->children[i]->height != -1) {
			element->children[i]->_internal_computed_height = element->children[i]->height;
		} else {
			element->children[i]->_internal_computed_height = -1;
			dev_long = walk_y_chain_get_height(element->children[i], 0, use_height, 1);
			if (dev_long < 0) {
				dev_long = 0;
			}
			dev_unit = walk_y_chain_get_div(element->children[i], 0);
			element->children[i]->_internal_computed_height = dev_long / dev_unit;
		}
		i++;
	}
	i = 0;
	while (i < element->child_count) {
		element->children[i]->_internal_computed_xoffset = walk_x_chain_get_x(element->children[i], 0, x, 1);
		element->children[i]->_internal_computed_yoffset = walk_y_chain_get_y(element->children[i], 0, y, 1);
		i++;
	}
	
	if (element->type == EL_CONTAINER) {
		draw_container(element, window, x, y);
	}
	if (element->type == EL_BUTTON) {
		draw_button(element, window, x, y);
	}
	if (element->type == EL_CHECKBOX) {
		draw_checkbox(element, window, x, y);
	}
	if (element->type == EL_ML_TEXTBOX) {
		draw_ml_textbox(element, window, x, y);
	}
	if (element->type == EL_RADIOBUTTON) {
		draw_radiobutton(element, window, x, y);
	}
	if (element->type == EL_TAB) {
		draw_tab(element, window, x, y);
	}
	if (element->type == EL_TEXTBOX) {
		draw_textbox(element, window, x, y);
	}
	
	i = 0;
	while (i < element->child_count) {
		draw_parent_element(element->children[i], window, element->children[i]->_internal_computed_xoffset, element->children[i]->_internal_computed_yoffset);
		i++;
	}
}

void draw_element(struct MTK_WinElement *element, struct MTK_WinBase *window, signed int x, signed int y) {	
	if (element->parent == 0) {
		// Flush the pixel element map
		set_pixel_element_map(0, 0, window->width, window->height, window, 0);
		if (element->width == -1) {
			element->_internal_computed_width = window->width;
		} else {
			element->_internal_computed_width = element->width;
		}
		if (element->height == -1) {
			element->_internal_computed_height = window->height;
		} else {
			element->_internal_computed_height = element->height;
		}
		draw_parent_element(element, window, x, y);
		return;
	}
	
	/*
	unsigned int use_width;
	unsigned int use_height;
	if (element->width != -1) {
		use_width = element->width;
	} else {
		use_width = element->_internal_computed_width;
	}
	if (element->height != -1) {
		use_height = element->height;
	} else {
		use_height = element->_internal_computed_height;
	}
	*/
	
	if (element->type == EL_CONTAINER) {
		draw_container(element, window, x, y);
	}
	if (element->type == EL_BUTTON) {
		draw_button(element, window, x, y);
	}
	if (element->type == EL_CHECKBOX) {
		draw_checkbox(element, window, x, y);
	}
	if (element->type == EL_ML_TEXTBOX) {
		draw_ml_textbox(element, window, x, y);
	}
	if (element->type == EL_RADIOBUTTON) {
		draw_radiobutton(element, window, x, y);
	}
	if (element->type == EL_TAB) {
		draw_tab(element, window, x, y);
	}
	if (element->type == EL_TEXTBOX) {
		draw_textbox(element, window, x, y);
	}
	
	unsigned int i = 0;
	while (i < element->child_count) {
		draw_element(element->children[i], window, element->children[i]->_internal_computed_xoffset, element->children[i]->_internal_computed_yoffset);
		i++;
	}
	return;
}
