#include "simple-status.h"
#include "clock.h"
#include "loadavg.h"
#include "cpu.h"

static bool quit = false;

static void signal_handler(int signum) {
        if (signum == SIGTERM || signum == SIGINT)
                quit = true;
}

void error(const char *fmt, ...) {
        fprintf(stderr, "simple-status: error: ");

        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);

        fputc('\n', stderr);
}

int main(int argc, char **argv) {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = signal_handler;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGUSR1, &sa, NULL);  /* Break sleep(), force update */

        while (!quit) {
                printf("%s | %s | %s\n", cpu_update(), loadavg_update(), clock_update());
                fflush(stdout);
                sleep(1);
        }

        return EXIT_SUCCESS;
}
