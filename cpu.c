#include "simple-status.h"
#include "cpu.h"

/* in percents */
static int cpu_usage(void) {
        static const char *path = "/proc/stat";

        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("cpu: can't open %s: %s.", path, strerror(errno));
                return 0;
        }

        unsigned long long user, nice, system, idle;
        if (fscanf(f, "cpu %llu %llu %llu %llu", &user, &nice, &system, &idle) != 4) {
                error("cpu: fscanf() on %s failed.", path);
                fclose(f);
                return 0;
        }
        fclose(f);

        static unsigned long long total_prev, idle_prev;
        unsigned long long total = user + nice + system + idle,
                           diff_total = total - total_prev,
                           diff_idle = idle - idle_prev;

        total_prev = total;
        idle_prev = idle;

        int usage = (diff_total == 0) ? 100 :
                100 * (diff_total-diff_idle) / diff_total;

        return usage;
}

/* in degrees Celsius */
static int cpu_temperature(void) {
        static const char *path = "/sys/devices/platform/coretemp.0/hwmon/hwmon0/temp1_input";

        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("cpu: can't open %s: %s.", path, strerror(errno));
                return 0;
        }

        int t;
        if (fscanf(f, "%d", &t) != 1) {
                error("cpu: fscanf() on %s failed.", path);
                t = 0;
        }

        fclose(f);
        return t/1000;
}

char *cpu_update(void) {
        //const int usage_threshold = 50;
        //const int temperature_threshold = 60;
        static char text[16];
        if (snprintf(text, size(text), "CPU %2d%% %2d°C", cpu_usage(), cpu_temperature()) < 0) {
                error("cpu: snprintf() failed.");
                return "";
        }
        return text;
}
