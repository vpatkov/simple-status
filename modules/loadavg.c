#include "simple-status.h"
#include "loadavg.h"

char *loadavg_update(void) {
        const float threshold = 2.0;
        static char text[16];

        double loadavg;
        if (getloadavg(&loadavg, 1) != 1) {
                error("loadavg: getloadavg() failed.");
                return "";
        }

        if (snprintf(text, size(text), "LOAD %.1f", loadavg) < 0) {
                error("loadavg: snprintf() failed.");
                return "";
        }

        return text;
}
