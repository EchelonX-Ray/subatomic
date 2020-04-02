#include "./subatomic.h"

int main(int argc, char *argv[]) {
	char *title = "Window Title";
	struct MTK_WinFontPack font_pack;
	setup_font("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", 25, &font_pack);
	
	struct MTK_WinBase window;
	window_struct_init(&window);
	
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
		struct MTK_WinElAnchor *anchors;
		unsigned int anchors;
		struct MTK_WinElement *children;
		unsigned int child_count;
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
	
	struct MTK_WinElement textbox;
	struct MTK_WinElement button;
	struct MTK_WinElement root_cont;
	struct MTK_WinElement *root_cont_childrenlist[2];
	root_cont_childrenlist[0] = &textbox;
	root_cont_childrenlist[1] = &button;
	textbox.id = 1;
	textbox.x = 10;
	textbox.y = 0;
	textbox.width = -1;
	textbox.height = 30;
	textbox.mouse_state = EL_MS_NORMAL;
	textbox.type = EL_TEXTBOX;
	textbox.type_spec = 0; // TODO
	textbox.parent = &root_cont; // TODO
	textbox.children = 0; // TODO
	textbox.child_count = 0; // TODO
	button.id = 2;
	button.x = 10;
	button.y = 0;
	button.width = 70;
	button.height = 30;
	button.mouse_state = EL_MS_NORMAL;
	button.type = EL_BUTTON;
	button.type_spec = 0; // TODO
	button.parent = &root_cont;
	button.children = 0; // TODO
	button.child_count = 0; // TODO
	root_cont.id = 0;
	root_cont.x = 0;
	root_cont.y = 0;
	root_cont.width = 640;
	root_cont.height = 480;
	root_cont.mouse_state = EL_MS_NORMAL;
	root_cont.type = EL_CONTAINER;
	root_cont.type_spec = 0; // TODO
	root_cont.parent = 0; // TODO
	root_cont.children = root_cont_childrenlist; // TODO
	root_cont.child_count = 2; // TODO
	
	window.dis = XOpenDisplay((char *)0);
	window.root_win = DefaultRootWindow(window.dis);
	window.title = title;
	window.events = mtk_gem(KeyEvent) | mtk_gem(MouseBtnEvent) | mtk_gem(MouseMoveEvent) | mtk_gem(LeaveEvent) | mtk_gem(ExposeEvent);
	window.event_handles[KeyEvent] = (void*)&key_event;
	window.event_handles[MouseBtnEvent] = (void*)&button_event;
	window.event_handles[MouseMoveEvent] = (void*)&pointer_motion_event;
	window.event_handles[LeaveEvent] = (void*)&leave_window_event;
	window.event_handles[ExposeEvent] = (void*)&exposure_event;
	window.event_handles[CloseEvent] = (void*)&before_closing;
	window.width = 640;
	window.height = 480;
	window.bitmap = calloc(window.width * window.height, sizeof(uint32_t));	
	window.mouse_state.pixel_element_map = calloc(window.width * window.height, sizeof(struct MTK_WinElement**));
	window.root_element = &root_cont;
	window.ignore_key_repeat = 0; // Allow Key Repeat
	create_window(&window);
	
	draw_element(&root_cont, &window, 0, 0, window.width);
	
	fill_rect(200, 200, 100, 50, 0x0000FF00, &window);
	unsigned int x = 50;
	//printf("x: %d\n", x);
	x += draw_char(x, 50, 'A', 0x00FFFFFF, font_pack.font_style, &window);
	//printf("x: %d\n", x);
	x += draw_char(x, 50, 'B', 0x00FFFFFF, font_pack.font_style, &window);
	//printf("x: %d\n", x);
	x += draw_char(x, 50, 'C', 0x00FFFFFF, font_pack.font_style, &window);
	//printf("x: %d\n", x);
	fill_rect(50, 50, 1, 1, 0x00FF0000, &window);
	draw_bm(0, 0, 640, 480, &window);
	
	XEvent event;
	while(window.loop_running == 1) {
		XNextEvent(window.dis, &event);
		event_handler(&window, &event);
	}
	
	free_font(&font_pack);
	free(window.mouse_state.pixel_element_map);
	free(window.bitmap);
	
	return 0;
}
