#include "simple-status.h"
#include "clock.h"
#include <time.h>

struct block *clock_update(void) {
        static char full_text[64];
        static struct block block = {
                .full_text = full_text,
                .urgent = false,
        };

        time_t now = time(NULL);
        if (strftime(full_text, size(full_text), "%a %F %R ", localtime(&now)) == 0)
                *full_text = 0;

        return &block;
}
