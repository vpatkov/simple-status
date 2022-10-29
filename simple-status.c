#include "simple-status.h"
#include "clock.h"
#include "loadavg.h"

void error(const char *fmt, ...) {
        fprintf(stderr, "simple-status: error: ");

        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);

        fputc('\n', stderr);
}

int main(int argc, char **argv) {
        while (true) {
                printf("%s | %s\n", loadavg_update(), clock_update());
                fflush(stdout);
                sleep(1);
        }
        return EXIT_SUCCESS;
}
