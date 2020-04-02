#include "./element_parsing.h"
#include "./../toolbox/cstr_manip.h"
#include <libxml/parser.h>

void extend_child_ptrs_by_1(struct MTK_WinElement*** ptr_to_array, unsigned int current_length){
	if (ptr_to_array == 0) {
		return;
	}
	unsigned int i = 0;
	struct MTK_WinElement** tmp_ptr_oldarray;
	struct MTK_WinElement** tmp_ptr_newarray;
	tmp_ptr_oldarray = *ptr_to_array;
	tmp_ptr_newarray = calloc(sizeof(struct MTK_WinElement*), current_length + 1);
	while (i < current_length) {
		tmp_ptr_newarray[i] = tmp_ptr_oldarray[i];
		i++;
	}
	free(tmp_ptr_oldarray);
	tmp_ptr_newarray[i] = calloc(sizeof(struct MTK_WinElement), 1);
	*ptr_to_array = tmp_ptr_newarray;
	return;
}
void extend_anchor_ptrs_by_1(struct MTK_WinElAnchor*** ptr_to_array, unsigned int current_length){
	if (ptr_to_array == 0) {
		return;
	}
	unsigned int i = 0;
	struct MTK_WinElAnchor** tmp_ptr_oldarray;
	struct MTK_WinElAnchor** tmp_ptr_newarray;
	tmp_ptr_oldarray = *ptr_to_array;
	tmp_ptr_newarray = calloc(sizeof(struct MTK_WinElAnchor*), current_length + 1);
	while (i < current_length) {
		tmp_ptr_newarray[i] = tmp_ptr_oldarray[i];
		i++;
	}
	free(tmp_ptr_oldarray);
	tmp_ptr_newarray[i] = calloc(sizeof(struct MTK_WinElAnchor), 1);
	*ptr_to_array = tmp_ptr_newarray;
	return;
}
char* has_child_text(xmlNode *parent_node){
	if (parent_node->children != 0) {
		if (parent_node->children->type == 3 && cstrcmp(parent_node->children->name, "text")) {
			return parent_node->children->content;
		}
	}
	return 0;
}

void parse_events(xmlNode *node, unsigned int level, struct MTK_WinElement* root_element) {
}

void parse_values(xmlNode *parent_node, unsigned int level, struct MTK_WinElement* element) {
	char *node_content;
	for (xmlNode *node = parent_node->children; node != 0; node = node->next) {
		if (cstrcmp(node->name, "id")) {
			if (node_content = has_child_text(node)) {
				element->id = catoi(node_content, 10);
			}
		}
		if (cstrcmp(node->name, "x")) {
			if (node_content = has_child_text(node)) {
				element->x = catoi(node_content, 10);
			}
		}
		if (cstrcmp(node->name, "y")) {
			if (node_content = has_child_text(node)) {
				element->y = catoi(node_content, 10);
			}
		}
		if (cstrcmp(node->name, "width")) {
			if (node_content = has_child_text(node)) {
				element->width = catoi(node_content, 10);
			}
		}
		if (cstrcmp(node->name, "height")) {
			if (node_content = has_child_text(node)) {
				element->height = catoi(node_content, 10);
			}
		}
		if (cstrcmp(node->name, "text")) {
			if (node_content = has_child_text(node)) {
				if (element->text != 0) {
					free(element->text);
				}
				element->text = calloc(cstrlen(node_content), sizeof(char));
				cstrcpy(node_content, element->text);
			}
		}
		if (cstrcmp(node->name, "bgcolor")) {
			if (node_content = has_child_text(node)) {
				element->bgcolor = catoi(node_content, 16);
			}
		}
		if (cstrcmp(node->name, "fgcolor")) {
			if (node_content = has_child_text(node)) {
				element->fgcolor = catoi(node_content, 16);
			}
		}
		if (cstrcmp(node->name, "event")) {
		}
	}
}

void parse_anchors(xmlNode *node, unsigned int level, struct MTK_WinElement* root_element) {
}

void parse_elements(xmlNode *parent_node, unsigned int level, struct MTK_WinElement* element) {
	if (cstrcmp(parent_node->name, "button")) {
		element->type = EL_BUTTON;
		element->type_spec = calloc(sizeof(struct EL_button_t), 1);
	}
	if (cstrcmp(parent_node->name, "checkbox")) {
		element->type = EL_CHECKBOX;
		element->type_spec = calloc(sizeof(struct EL_checkbox_t), 1);
	}
	if (cstrcmp(parent_node->name, "container")) {
		element->type = EL_CONTAINER;
		element->type_spec = calloc(sizeof(struct EL_container_t), 1);
	}
	if (cstrcmp(parent_node->name, "ml_textbox")) {
		element->type = EL_ML_TEXTBOX;
		element->type_spec = calloc(sizeof(struct EL_ml_textbox_t), 1);
	}
	if (cstrcmp(parent_node->name, "radiobutton")) {
		element->type = EL_RADIOBUTTON;
		element->type_spec = calloc(sizeof(struct EL_radiobutton_t), 1);
	}
	if (cstrcmp(parent_node->name, "tab")) {
		element->type = EL_TAB;
		element->type_spec = calloc(sizeof(struct EL_tab_t), 1);
	}
	if (cstrcmp(parent_node->name, "textbox")) {
		element->type = EL_TEXTBOX;
		element->type_spec = calloc(sizeof(struct EL_textbox_t), 1);
	}
	for (xmlNode *node = parent_node->children; node != 0; node = node->next) {
		if (cstrcmp(node->name, "button")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "checkbox")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "container")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "ml_textbox")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "radiobutton")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "tab")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "textbox")) {
			extend_child_ptrs_by_1(&(element->children), element->child_count);
			parse_elements(node, level + 1, element->children[element->child_count]);
			element->child_count++;
		}
		if (cstrcmp(node->name, "values")) {
			parse_values(node, level + 1, element);
		}
		if (cstrcmp(node->name, "anchors")) {
			parse_anchors(node, level + 1, element);
		}
	}
}

void parse_node(xmlNode *node, unsigned int level, struct MTK_WinElement* root_element){
	if (cstrcmp(node->name, "button")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "checkbox")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "container")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "ml_textbox")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "radiobutton")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "tab")) {
		parse_elements(node, level + 1, root_element);
	}
	if (cstrcmp(node->name, "textbox")) {
		parse_elements(node, level + 1, root_element);
	}
	return;
}

void load_element_tree(struct MTK_WinElement* root_element) {
	if (root_element == 0) {
		return;
	}
	xmlDoc *document;
	xmlNode *root_node;
	char *filename = "./window.xml";
	document = xmlReadFile(filename, NULL, 0);
	root_node = xmlDocGetRootElement(document);
	parse_node(root_node, 0, root_element);
	xmlFreeDoc(document);
	xmlCleanupParser();
	return;
}
