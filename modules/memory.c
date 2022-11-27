#include "simple-status.h"
#include "memory.h"

/* in percents */
static int memory_usage(void) {
        FILE *f = fopen("/proc/meminfo", "r");
        if (f == NULL) {
                error("memory: can't open /proc/meminfo: %s.", strerror(errno));
                return 0;
        }

        char line[128];
        unsigned long long memtotal, memfree, buffers, cached;
        int read_counter = 0;
        while (fgets(line, size(line), f) != NULL && read_counter < 4) {
                if (
                        sscanf(line, "MemTotal: %llu", &memtotal) == 1 ||
                        sscanf(line, "MemFree: %llu", &memfree) == 1 ||
                        sscanf(line, "Buffers: %llu", &buffers) == 1 ||
                        sscanf(line, "Cached: %llu", &cached) == 1
                )
                        read_counter++;
        }
        fclose(f);
        if (read_counter < 4) {
                error("memory: got %d fields out of 4 from /proc/meminfo", read_counter);
                return 0;
        }

        unsigned long long used = memtotal - memfree - buffers - cached;
        int used_percents = (memtotal == 0) ? 100 : 100 * used / memtotal;
        return used_percents;
}

struct block *memory_update(void) {
        const int usage_threshold = 50;

        static char full_text[16];
        static struct block block = {
                .full_text = full_text,
        };

        int u = memory_usage();
        block.urgent = u >= usage_threshold;
        snprintf(full_text, size(full_text), "RAM %2d%%", u);
        return &block;
}
