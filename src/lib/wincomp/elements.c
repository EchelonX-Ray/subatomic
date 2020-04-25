#include "./elements.h"
#include "./../toolbox/cstr_manip.h"
#include <stdlib.h>
#include <unistd.h>

// Start the cursor blinking thread
void start_the_cursor(struct MTK_WinBase *window, unsigned int blink_state) {
	window->cursor_blink = blink_state;
	if (window->thread.thread_running == 0) {
		window->thread.thread_running = 1;
		pthread_create(window->thread.thread, 0, blink_the_cursor, window);
	}
	return;
}
// Stop the cursor blinking thread
void stop_the_cursor(struct MTK_WinBase *window, unsigned int blink_state) {
	if (window->thread.thread_running == 1) {
		pthread_cancel(*(window->thread.thread));
		pthread_join(*(window->thread.thread), 0);
		window->thread.thread_running = 0;
	}
	window->cursor_blink = blink_state;
	return;
}
// Stop and Restart the cursor blinking thread
void reset_the_cursor(struct MTK_WinBase *window, unsigned int blink_state) {
	window->cursor_blink = blink_state;
	if (window->thread.thread_running == 1) {
		stop_the_cursor(window, blink_state);
		start_the_cursor(window, blink_state);
	}
	return;
}
// Wait for the timeout and then toggle the cursor
// This is run as a separate thread.  It doesn't loop because 
// it should be restarted after the event loop notification it 
// generates, is handled.  This prevents events from backing up 
// on slow systems.
void* blink_the_cursor(void* param_ptr) {
	struct MTK_WinBase *window;
	pthread_mutex_t *lock;
	unsigned int millisec_increment;
	int fd;
	int write_ret;
	struct timespec req;
	window = (struct MTK_WinBase*)param_ptr;
	lock = window->thread.lock;
	fd = window->thread.fd;
	millisec_increment = window->thread.millisec_increment;
	
	req.tv_sec = millisec_increment / 1000;
	req.tv_nsec = (millisec_increment % 1000) * 1000;
	
	nanosleep(&req, 0); // nanosleep is a pthread deferred cancellation point per the manual pthreads(7).  This thread may be cancelled during the sleep.
	pthread_mutex_lock(lock);
	if (window->cursor_blink == 1) {
		window->cursor_blink = 0;
	} else {
		window->cursor_blink = 1;
	}
	write_ret = write(fd, "\x01", 1); // Send a byte of value 0x01 into a pipe.  This will notify the event loop to handle the event.
	if (write_ret != 1) {
		window->loop_running = 0;
	}
	pthread_mutex_unlock(lock);
	return 0;
}

// The the element that maps to a pixel in the window
// This is set by each element when they are drawn to the bitmap buffer and 
// is then used to determine how to handle mouse events per the window coordinates
// that they occur on.
void set_pixel_element_map(signed int x, signed int y, unsigned int width, unsigned int height, struct MTK_WinBase *window, struct MTK_WinElement *element) {
	int i = x;
	width += x;
	height += y;
	while (y < height && y < window->height && y >= 0) {
		x = i;
		while (x < width && x < window->width && x >= 0) {
			window->_internal_mouse_state.pixel_element_map[y * window->width + x] = element;
			x++;
		}
		y++;
	}
	return;
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
unsigned int get_chain_index(struct MTK_WinElement *element, struct MTK_WinEl_ChainList *chainlist) {
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
void update_y_chains(struct MTK_WinElement *element, struct MTK_WinElement *from, struct MTK_WinEl_ChainList *chainlist, unsigned int independence_level) {
	unsigned int new_chain_required;
	unsigned int element_chain_independence_level;
	unsigned int chain_index;
	unsigned int i;
	
	// Check if this element is anchored to the previous element.  If not, it means that a one-way link was followed.
	// In the case of a one-way link, increment the independence_level because this element is not bound to an element of the previous chain.
	new_chain_required = 1;
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0) { // This function is only looking for anchors which deal with the Y-Axis.
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
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0) { // This function is only looking for anchors which deal with the Y-Axis.
			// Only update existing elements
			element_chain_independence_level = is_element_already_in_chainlist(element, chainlist);
			if (element_chain_independence_level > 0) {
				chain_index = get_chain_index(element, chainlist);
				chainlist->element_chains[chain_index].independence_level = independence_level; // Update the independence_level
				if (element->anchors[i].relative_to != element->parent && element->anchors[i].relative_to != from) { // Don't recur into elements which aren't siblings and don't backtrack
					update_y_chains(element->anchors[i].relative_to, element, chainlist, independence_level);
				}
			}
		}
		i++;
	}
	return;
}

// This function works through relatively anchored elements on the y-axis and builds a chainlist.
// The chainlist contains a list of chains.  The chains describe elements who's positioning depends on each other.
// Some elements can depend on the positioning of elements in other chains.  However, the relationship is non-reciprocal.
// This means that there is a hierarchy of chains.  This relationship is refered to in the comments of this function as a one-way link.
// Chains with a higher independence_level are higher in the hierarchy.  The chains are then parsed from high to low independence_level.
// This allows elements that depend on the positioning of elements in the higher independence_level chains to refer to them.
// The independence_level grows upward during this function's execution.  The maximum value reached is stored in max_independence_level.
// The lowest valid value of independence_level is 1.  That is where it starts.
void find_y_chains(struct MTK_WinElement *element, struct MTK_WinElement *from, struct MTK_WinEl_ChainList *chainlist, unsigned int chain_index, unsigned int independence_level) {
	unsigned int new_chain_required;
	unsigned int element_chain_independence_level;
	unsigned int i;
	
	// Check if this element is anchored to the previous element.  If not, it means that a one-way link was followed.
	// In the case of a one-way link, a new chain should be created because this element is not bound to an element of the previous chain.
	// This check will also be tripped if from == 0, such as when this function is first called from compute_element_internals(), to create the first chain.
	new_chain_required = 1;
	i = 0;
	while (i < element->anchor_count) {
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0) { // This function is only looking for anchors which deal with the Y-Axis.
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
		if ((element->anchors[i].flags & EL_AC_YAXIS) > 0) { // This function is only looking for anchors which deal with the Y-Axis.
			// Don't duplicate entries to the chainlist and update independence_levels as needed
			element_chain_independence_level = is_element_already_in_chainlist(element, chainlist);
			if (element_chain_independence_level == 0) {
				add_element_to_chain(element, chainlist, chain_index);
				if (element->anchors[i].relative_to != element->parent && element->anchors[i].relative_to != from) { // Don't recur into elements which aren't siblings and don't backtrack
					find_y_chains(element->anchors[i].relative_to, element, chainlist, chain_index, independence_level);
				}
			} else {
				// Do does the element in the chainlist have a lower chain independence_level than the currently computed independence_level
				if (element_chain_independence_level < independence_level) {
					// If so, increase all the relevant chain independence_levels to match the currently computed one.
					// This requires that the element be recomputed and parsed with the current independence_level
					update_y_chains(element, from, chainlist, independence_level);
				}
			}
		}
		i++;
	}
	return;
}

// Print DEBUGGING Info - Remove on production release
/*
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
*/

void parse_chain_width(struct MTK_WinEl_Chain *chain, unsigned int parent_width) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	signed int x_var_el_count = parent_width;
	unsigned int x_divisor = 0;
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
		
		i++;
	}
	if (x_var_el_count < 0) {
		x_var_el_count = 0;
	}
	if (x_divisor > 0) {
		x_var_el_count /= x_divisor;
	}
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		if (element->width < 0) {
			element->_internal_computed_width = x_var_el_count;
		}
		i++;
	}
	
	return;
}
void parse_chain_height(struct MTK_WinEl_Chain *chain, unsigned int parent_height) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	signed int y_var_el_count = parent_height;
	unsigned int y_divisor = 0;
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		
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
		
		i++;
	}
	if (y_var_el_count < 0) {
		y_var_el_count = 0;
	}
	if (y_divisor > 0) {
		y_var_el_count /= y_divisor;
	}
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		if (element->height < 0) {
			element->_internal_computed_height = y_var_el_count;
		}
		i++;
	}
	
	return;
}
void parse_chain_x(struct MTK_WinEl_Chain *chain, unsigned int parent_width) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	unsigned int k;
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		k = 0;
		while(k < element->anchor_count) {
			j = 0;
			while (j < element->anchor_count) {
				if (element->_internal_computed_xoffset == -1) {
					if ((element->anchors[i].flags & EL_AC_XAXIS) > 0 && element->anchors[i].relative_to->_internal_computed_xoffset != -1) {
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
				j++;
			}
			i++;
		}
	}
	
	return;
}
void parse_chain_y(struct MTK_WinEl_Chain *chain, unsigned int parent_height) {
	struct MTK_WinElement *element;
	unsigned int i;
	unsigned int j;
	unsigned int k;
	i = 0;
	while (i < chain->element_count) {
		element = chain->elements_in_chain[i];
		k = 0;
		while(k < element->anchor_count) {
			j = 0;
			while (j < element->anchor_count) {
				if (element->_internal_computed_yoffset == -1) {
					if ((element->anchors[i].flags & EL_AC_YAXIS) > 0 && element->anchors[i].relative_to->_internal_computed_yoffset != -1) {
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

void recur_compute_element_internals(struct MTK_WinElement *element) {
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
	while (i < element->child_count) {
		element->children[i]->_internal_computed_xoffset = -1;
		element->children[i]->_internal_computed_yoffset = -1;
		find_x_chains(element->children[i], 0, &x_chainlist, 0, 0);
		find_y_chains(element->children[i], 0, &y_chainlist, 0, 0);
		i++;
	}
	j = x_chainlist.max_independence_level;
	while (j > 0) {
		i = 0;
		while (i < x_chainlist.chain_count) {
			if (x_chainlist.element_chains[i].independence_level == j) {
				parse_chain_width(x_chainlist.element_chains + i, element->_internal_computed_width);
				parse_chain_x(x_chainlist.element_chains + i, element->_internal_computed_width);
			}
			i++;
		}
		j--;
	}
	j = y_chainlist.max_independence_level;
	while (j > 0) {
		i = 0;
		while (i < y_chainlist.chain_count) {
			if (y_chainlist.element_chains[i].independence_level == j) {
				parse_chain_height(y_chainlist.element_chains + i, element->_internal_computed_height);
				parse_chain_y(y_chainlist.element_chains + i, element->_internal_computed_height);
			}
			i++;
		}
		j--;
	}
	
	// Free the memory
	i = 0;
	while (i < x_chainlist.chain_count) {
		free(x_chainlist.element_chains[i].elements_in_chain);
		i++;
	}
	free(x_chainlist.element_chains);
	i = 0;
	while (i < y_chainlist.chain_count) {
		free(y_chainlist.element_chains[i].elements_in_chain);
		i++;
	}
	free(y_chainlist.element_chains);
	
	// Recur to process children
	i = 0;
	while(i < element->child_count) {
		recur_compute_element_internals(element->children[i]);
		i++;
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
	
	if (window->root_element->width == -1) {
		window->root_element->_internal_computed_width = window->width;
	} else {
		window->root_element->_internal_computed_width = window->root_element->width;
	}
	if (window->root_element->height == -1) {
		window->root_element->_internal_computed_height = window->height;
	} else {
		window->root_element->_internal_computed_height = window->root_element->height;
	}
	
	// Stage #1: Find parsing order.  Highest to lowest.  This priority is stored in [chain].independence_level
	i = 0;
	while (i < window->root_element->child_count) {
		window->root_element->children[i]->_internal_computed_xoffset = -1;
		window->root_element->children[i]->_internal_computed_yoffset = -1;
		find_x_chains(window->root_element->children[i], 0, &x_chainlist, 0, 0);
		find_y_chains(window->root_element->children[i], 0, &y_chainlist, 0, 0);
		i++;
	}
	
	// Stage #2: Parse based on the order found.  Highest to lowest.  This priority is stored in [chain].independence_level
	j = x_chainlist.max_independence_level;
	while (j > 0) {
		i = 0;
		while (i < x_chainlist.chain_count) {
			if (x_chainlist.element_chains[i].independence_level == j) {
				parse_chain_width(x_chainlist.element_chains + i, window->root_element->_internal_computed_width);
				parse_chain_x(x_chainlist.element_chains + i, window->root_element->_internal_computed_width);
			}
			i++;
		}
		j--;
	}
	j = y_chainlist.max_independence_level;
	while (j > 0) {
		i = 0;
		while (i < y_chainlist.chain_count) {
			if (y_chainlist.element_chains[i].independence_level == j) {
				parse_chain_height(y_chainlist.element_chains + i, window->root_element->_internal_computed_height);
				parse_chain_y(y_chainlist.element_chains + i, window->root_element->_internal_computed_height);
			}
			i++;
		}
		j--;
	}
	
	// Free the memory
	i = 0;
	while (i < x_chainlist.chain_count) {
		free(x_chainlist.element_chains[i].elements_in_chain);
		i++;
	}
	free(x_chainlist.element_chains);
	i = 0;
	while (i < y_chainlist.chain_count) {
		free(y_chainlist.element_chains[i].elements_in_chain);
		i++;
	}
	free(y_chainlist.element_chains);
	
	// Recur to process children
	i = 0;
	while(i < window->root_element->child_count) {
		recur_compute_element_internals(window->root_element->children[i]);
		i++;
	}
	
	return;
}

// Draw the element to the bitmap buffer and draw any child elements recursively
void draw_element(struct MTK_WinElement *element, struct MTK_WinBase *window) {
	// If this is the root element, go ahead and just reset the pixel element map to all NULL pointers
	if (element->parent == 0) {
		set_pixel_element_map(0, 0, window->width, window->height, window, 0);
	}
	
	if (element->type == EL_CONTAINER) {
		draw_container(element, window);
	}
	if (element->type == EL_BUTTON) {
		draw_button(element, window);
	}
	if (element->type == EL_CHECKBOX) {
		draw_checkbox(element, window);
	}
	if (element->type == EL_ML_TEXTBOX) {
		draw_ml_textbox(element, window);
	}
	if (element->type == EL_RADIOBUTTON) {
		draw_radiobutton(element, window);
	}
	if (element->type == EL_TAB) {
		draw_tab(element, window);
	}
	if (element->type == EL_TEXTBOX) {
		draw_textbox(element, window);
	}
	
	unsigned int i = 0;
	while (i < element->child_count) {
		draw_element(element->children[i], window);
		i++;
	}
	return;
}
