#include "./events.h"

void exposure_event(XEvent* event, struct MTK_WinBase* window){
	printf("Expose Event.\n");
}
void button_event(int state, unsigned int button, int x, int y, XEvent* event, struct MTK_WinBase* window){
	printf("Button Event: State-%d", state);
	printf(" button-%d ", button);
	printf(" x-%d ", x);
	printf(" y-%d\n", y);
}
void key_event(int state, int keycode, XEvent* event, struct MTK_WinBase* window){
	if (state == 2 && keycode == 9) {
		cue_window_close(window, event);
	} else {
		printf("Key Event: keycode-%d", keycode);
		printf(" state-%d\n", state);
	}
}
void pointer_motion_event(int x, int y, XEvent* event, struct MTK_WinBase* window){
	//printf("Motion Event.\n");
}
void leave_window_event(int x, int y, XEvent* event, struct MTK_WinBase* window){
	printf("Leave Event: x-%d", x);
	printf(" y-%d\n", y);
}
