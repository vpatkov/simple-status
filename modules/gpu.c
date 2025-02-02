#include "simple-status.h"
#include "gpu.h"

static char hwmon_path[64];

/* in degrees Celsius */
static int gpu_temperature(void) {
	long t;
	if (pscanf(hwmon_path, "%ld", &t) != 1) {
		error("gpu: pscanf() on %s failed.", hwmon_path);
		return 0;
	}
	return (t+500)/1000;
}

struct block *gpu_update(void) {
	const int temperature_threshold = 70;

	static char full_text[16];
	static struct block block = {
		.full_text = full_text,
	};

	int t = gpu_temperature();
	block.urgent = t >= temperature_threshold;
	snprintf(full_text, size(full_text), "GPU %2d°C", t);
	return &block;
}

void gpu_init(void) {
	static const char *hwmon_name = "amdgpu";
	int n = find_hwmon(hwmon_name);
	if (n < 0) {
		error("gpu: can't find hwmon for %s.", hwmon_name);
		exit(EXIT_FAILURE);
	}
	snprintf(hwmon_path, size(hwmon_path),
		"/sys/class/hwmon/hwmon%d/temp1_input", n);
}
