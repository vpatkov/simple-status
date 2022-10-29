#include "simple-status.h"

static int cpu_load(void) {
        return 0;
}

static int cpu_temperature(void) {
        const char *path = "/sys/devices/platform/coretemp.0/hwmon/hwmon0/temp1_input";
        const int threshold = 60;

        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("cpu: can't open %s: %s.", path, strerror(errno));
                return 0;
        }

        int t;
        if (fscanf(f, "%d", &t) != 1) {
                error("cpu: fscanf() failed.");
                fclose(f);
                return 0;
        }

        fclose(f);
        return t/1000;
}

char *cpu_update(void) {
        static char text[16];
        if (snprintf(text, size(text), "CPU %2d%% %d°C", cpu_load(), cpu_temperature()) < 0) {
                error("cpu: snprintf() failed.");
                return "";
        }
        return text;
}
