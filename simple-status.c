#include "simple-status.h"

#include "modules/clock.h"
#include "modules/loadavg.h"
#include "modules/cpu.h"
#include "modules/gpu.h"
#include "modules/memory.h"
#include "modules/network.h"
#include "modules/sound.h"
#include "modules/keyboard.h"

struct module {
        const char *name;
        struct block *(*update)(void);
        void (*init)(void);
        int interval;
        struct block *current;
};

static struct module modules[] = {
        { .name = "keyboard", .update = keyboard_update, .interval = 1 },
        { .name = "sound", .update = sound_update, .interval = 1 },
        { .name = "cpu", .update = cpu_update, .interval = 1, .init = cpu_init },
        { .name = "gpu", .update = gpu_update, .interval = 1, .init = gpu_init },
        { .name = "memory", .update = memory_update, .interval = 1 },
        { .name = "loadavg", .update = loadavg_update, .interval = 1 },
        { .name = "network", .update = network_update, .interval = 1 },
        { .name = "clock", .update = clock_update, .interval = 1 },
};

static bool quit = false;

static void signal_handler(int signum) {
        if (signum == SIGTERM || signum == SIGINT)
                quit = true;
}

static void setup_signals(void) {
        struct sigaction sa = {
                .sa_flags = SA_RESTART,
                .sa_handler = signal_handler,
        };

        sigaction(SIGTERM, &sa, NULL);
        sigaction(SIGINT, &sa, NULL);
        sigaction(SIGUSR1, &sa, NULL);
};

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

int find_hwmon(const char *name) {
        char cur_path[64], cur_name[16];

        for (int i = 0; ; i++) {
                snprintf(cur_path, size(cur_path), "/sys/class/hwmon/hwmon%d/name", i);
                if (pscanf(cur_path, "%15s", cur_name) == 1) {
                        if (strcmp(cur_name, name) == 0)
                                return i;
                        continue;
                }
                return -1;
        }
}

int main(void) {
        setup_signals();

        for (size_t i = 0; i < size(modules); i++) {
                struct module *m = &modules[i];
                if (m->init != NULL)
                        m->init();
        }

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
                }
                printf("}],\n");
                fflush(stdout);
                sleep(1);
        }

        return EXIT_SUCCESS;
}
