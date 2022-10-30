#include "simple-status.h"
#include "clock.h"
#include "loadavg.h"
#include "cpu.h"
#include "memory.h"
#include "network.h"
#include "sound.h"

static bool quit = false;

struct colors colors_normal = { 0xffffff, 0x000000 },
              colors_highlight = { 0x000000, 0xffff00 };

static void signal_handler(int signum) {
        if (signum == SIGTERM || signum == SIGINT)
                quit = true;
}

void error(const char *format, ...) {
        fprintf(stderr, "simple-status: error: ");

        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
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
                printf("%s | %s | %s | %s | %s | %s\n", 
                        sound_update(),
                        cpu_update(), 
                        memory_update(), 
                        loadavg_update(),
                        network_update(),
                        clock_update());
                fflush(stdout);
                sleep(1);
        }

        return EXIT_SUCCESS;
}
