#include "simple-status.h"
#include "network.h"

static long down_speed(void) {
        return 0;
}

static long up_speed(void) {
        return 0;
}

static char *print_with_suffix(char *buf, long n) {
        *buf = 0;
        return;
}

char *network_update(void) {
        const long down_threshold = 1024*1024;
        const long up_threshold = 100*1024;
        static char text[32], down_text[8], up_text[8];

        print_with_suffix(down_text, down_speed());
        print_with_suffix(up_text, up_speed());

        if (snprintf(text, size(text), "NET ↓%s ↑%s B/s", down_text, up_text) < 0) {
                error("network: snprintf() failed.");
                return "";
        }
        return text;
}
