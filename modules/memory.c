#include "simple-status.h"
#include "memory.h"

/* in percents */
static int memory_usage(void) {
        static const char *path = "/proc/meminfo";

        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("memory: can't open %s: %s.", path, strerror(errno));
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

        if (ferror(f)) {
                error("memory: fgets() on %s failed.", path);
                fclose(f);
                return 0;
        }
        fclose(f);

        if (read_counter < 4) {
                error("memory: got %d fields out of 4 from %s", read_counter, path);
                return 0;
        }

        unsigned long long used = memtotal - memfree - buffers - cached;
        int used_percents = (memtotal == 0) ? 100 :
                100 * used / memtotal;

        return used_percents;
}

char *memory_update(void) {
        //const int usage_threshold = 50;
        static char text[16];
        if (snprintf(text, size(text), "RAM %2d%%", memory_usage()) < 0) {
                error("memory: snprintf() failed.");
                return "";
        }
        return text;
}