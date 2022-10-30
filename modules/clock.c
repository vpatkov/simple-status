#include "simple-status.h"
#include "clock.h"
#include <time.h>

char *clock_update(void) {
        static char text[64];
        time_t now = time(NULL);
        if (strftime(text, size(text), "%a %F %R", localtime(&now)) == 0) {
                error("clock: strftime() failed.");
                return "";
        }
        return text;
}
