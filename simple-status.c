#include "simple-status.h"

#include "modules/clock.h"
#include "modules/loadavg.h"
#include "modules/cpu.h"
#include "modules/memory.h"
#include "modules/network.h"
#include "modules/sound.h"

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

int pscanf(const char *path, const char *format, ...) {
        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("can't open %s: %s.", path, strerror(errno));
                return -1;
        }

        va_list args;
        va_start(args, format);
        int r = vfscanf(f, format, args);
        va_end(args);

        fclose(f);
        return r;
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
