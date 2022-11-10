#include "simple-status.h"
#include "gpu.h"

/* in degrees Celsius */
static int gpu_temperature(void) {
        static const char *path = "/sys/class/hwmon/hwmon2/temp1_input";
        long t;
        if (pscanf(path, "%ld", &t) != 1) {
                error("gpu: pscanf() on %s failed.", path);
                return 0;
        }
        return (t+500)/1000;
}

struct block *gpu_update(void) {
        const int temperature_threshold = 60;

        static char full_text[16];
        static struct block block = {
                .full_text = full_text,
        };

        int t = gpu_temperature();
        block.urgent = t >= temperature_threshold;
        if (snprintf(full_text, size(full_text), "GPU %2d°C", t) < 0)
                *full_text = 0;

        return &block;
}
