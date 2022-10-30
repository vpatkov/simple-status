#include "simple-status.h"
#include "sound.h"

/* in percents, negative if muted */
static int volume(void) {
        return -42;
}

char *sound_update(void) {
        static char text[16];
        int v = volume();
        if (snprintf(text, size(text), "🔊 %d%%%s", abs(v), v > 0 ? "" : " muted") < 0) {
                error("sound: snprintf() failed.");
                return "";
        }
        return text;
}
