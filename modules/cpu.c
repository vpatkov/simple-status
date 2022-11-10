#include "simple-status.h"
#include "cpu.h"

/* in percents */
static int cpu_usage(void) {
        unsigned long long user, nice, system, idle;
        if (pscanf("/proc/stat", "cpu %llu %llu %llu %llu",
                        &user, &nice, &system, &idle) != 4) {
                error("cpu: pscanf() on /proc/stat failed.");
                return 0;
        }

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
        static const char *path = "/sys/class/hwmon/hwmon0/temp1_input";
        long t;
        if (pscanf(path, "%ld", &t) != 1) {
                error("cpu: pscanf() on %s failed.", path);
                return 0;
        }
        return (t+500)/1000;
}

struct block *cpu_update(void) {
        const int usage_threshold = 50;
        const int temperature_threshold = 60;

        static char full_text[16];
        static struct block block = {
                .full_text = full_text,
        };

        int u = cpu_usage();
        int t = cpu_temperature();
        block.urgent = u >= usage_threshold || t >= temperature_threshold;
        if (snprintf(full_text, size(full_text), "CPU %2d%% %2d°C", u, t) < 0)
                *full_text = 0;

        return &block;
}
