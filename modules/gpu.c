#include "simple-status.h"
#include "gpu.h"

/* in degrees Celsius */
static int gpu_temperature(void) {
        static const char *path = "/sys/bus/pci/drivers/amdgpu/0000:01:00.0/hwmon/hwmon2/temp1_input";
        long t;
        if (pscanf(path, "%ld", &t) != 1) {
                error("gpu: pscanf() on %s failed.", path);
                return 0;
        }
        return (t+500)/1000;
}

char *gpu_update(void) {
        //const int temperature_threshold = 60;
        static char text[16];
        if (snprintf(text, size(text), "GPU %2d°C", gpu_temperature()) < 0) {
                error("gpu: snprintf() failed.");
                return "";
        }
        return text;
}
