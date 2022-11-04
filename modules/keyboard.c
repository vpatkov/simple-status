#include "simple-status.h"
#include "keyboard.h"
#include <X11/XKBlib.h>

static size_t layout_index(void) {
        Display *dpy = XOpenDisplay(NULL);
        if (dpy == NULL) {
                error("keyboard: XOpenDisplay() failed.");
                return SIZE_MAX;
        }
        XkbStateRec state;
        if (XkbGetState(dpy, XkbUseCoreKbd, &state) != 0) {
                error("keyboard: XkbGetState() failed.");
                XCloseDisplay(dpy);
                return SIZE_MAX;
        }
        XCloseDisplay(dpy);
        return state.group;
}

struct block *keyboard_update(void) {
        static char *layouts[] = {"EN", "RU"};
        static struct block block = {
                .urgent = false,
        };

        size_t l = layout_index();
        block.full_text = l < size(layouts) ? layouts[l] : "";
        return &block;
}
