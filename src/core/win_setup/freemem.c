#include "./../../lib/wincomp/elements.h"
#include <stdlib.h>

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
	if (root_cont->text != 0) {
		free(root_cont->text);
	}
	free(root_cont);
	return;
}
