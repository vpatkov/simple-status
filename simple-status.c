#include "simple-status.h"

#include "modules/clock.h"
#include "modules/loadavg.h"
#include "modules/cpu.h"
#include "modules/gpu.h"
#include "modules/memory.h"
#include "modules/network.h"
#include "modules/sound.h"

struct module {
        const char *name;
        struct block *(*update)(void);
        int interval;
        struct block *current;
};

static struct module modules[] = {
        { .name = "sound", .update = sound_update, .interval = 1 },
        { .name = "cpu", .update = cpu_update, .interval = 1 },
        { .name = "gpu", .update = gpu_update, .interval = 1 },
        { .name = "memory", .update = memory_update, .interval = 1 },
        { .name = "loadavg", .update = loadavg_update, .interval = 1 },
        { .name = "network", .update = network_update, .interval = 1 },
        { .name = "clock", .update = clock_update, .interval = 1 },
};

struct colors {
        long fg, bg;  /* 0xRRGGBB or -1 for i3bar's default */
};

static struct colors
        colors_normal = { .fg = -1, .bg = -1 },
        colors_urgent = { .fg = 0x000000, .bg = 0xffff00 };

static bool quit = false;

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

int main(void) {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = signal_handler;
        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGUSR1, &sa, NULL);  /* Break sleep(), force update */

        printf("{\"version\":1}\n[\n");

        for (int tick = 0; !quit; tick++) {
                printf("[{");
                for (size_t i = 0; i < size(modules); i++) {
                        struct module *m = &modules[i];
                        if (tick % m->interval == 0)
                                m->current = m->update();

                        if (i > 0)
                                printf("},{");

                        printf("\"name\":\"%s\"", m->name);
                        printf(",\"full_text\":\"%s\"", m->current->full_text);

                        if (m->current->urgent)
                                printf(",\"urgent\":true");

                        struct colors *c = m->current->urgent ? &colors_urgent : &colors_normal;
                        if (c->fg != -1)
                                printf(",\"color\":\"#%06lx\"", c->fg & 0xffffff);
                        if (c->bg != -1)
                                printf(",\"background\":\"#%06lx\"", c->bg & 0xffffff);
                }
                printf("}],\n");
                fflush(stdout);
                sleep(1);
        }

        return EXIT_SUCCESS;
}
