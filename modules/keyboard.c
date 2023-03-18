#include "simple-status.h"
#include "keyboard.h"
#include <X11/XKBlib.h>

static Display *dpy;

static size_t layout_index(void) {
	XkbStateRec state;
	if (XkbGetState(dpy, XkbUseCoreKbd, &state) != 0) {
		error("keyboard: XkbGetState() failed.");
		return SIZE_MAX;
	}
	return state.group;
}

struct block *keyboard_update(void) {
	static const char *layouts[] = {"EN", "RU"};
	static struct block block;

	size_t l = layout_index();
	block.full_text = l < size(layouts) ? layouts[l] : "";
	block.urgent = l > 0;
	return &block;
}

void keyboard_init(void) {
	dpy = XOpenDisplay(NULL);
	if (dpy == NULL) {
		error("keyboard: XOpenDisplay() failed.");
		exit(EXIT_FAILURE);
	}
}

void keyboard_deinit(void) {
	if (dpy != NULL)
		XCloseDisplay(dpy);
}
