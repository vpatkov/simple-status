#include "simple-status.h"
#include "loadavg.h"

struct block *loadavg_update(void) {
	const float threshold = 2.0;

	static char full_text[16];
	static struct block block = {
		.full_text = full_text,
	};

	double loadavg;
	if (getloadavg(&loadavg, 1) != 1) {
		error("loadavg: getloadavg() failed.");
		*full_text = 0;
		return &block;
	}

	block.urgent = loadavg >= threshold;
	snprintf(full_text, size(full_text), "LOAD %.1f", loadavg);
	return &block;
}
