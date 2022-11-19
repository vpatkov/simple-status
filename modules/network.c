#include "simple-status.h"
#include "network.h"
#include <time.h>

static void print_with_suffix(char *buf, size_t buf_len, double d) {
        int err;
        double m = d/1e6, k = d/1e3;

        if (m >= 10.0)
                err = snprintf(buf, buf_len, "%3.0fM", m);
        else if (m >= 0.1)
                err = snprintf(buf, buf_len, "%2.1fM", m);
        else if (k >= 10.0)
                err = snprintf(buf, buf_len, "%3.0fk", k);
        else if (k >= 0.1)
                err = snprintf(buf, buf_len, "%2.1fk", k);
        else
                err = snprintf(buf, buf_len, "%4.0f", d);

        if (err < 0)
                *buf = 0;
}

struct block *network_update(void) {
        static char full_text[32];
        static struct block block = {
                .full_text = full_text,
                .urgent = false,
        };

        unsigned long long rx_bytes, tx_bytes;
        if (pscanf("/sys/class/net/enp6s0/statistics/rx_bytes", "%llu", &rx_bytes) != 1 ||
            pscanf("/sys/class/net/enp6s0/statistics/tx_bytes", "%llu", &tx_bytes) != 1) {
                error("network: pscanf() failed.");
                *full_text = 0;
                return &block;
        }

        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == -1) {
                error("network: clock_gettime() failed: %s.", strerror(errno));
                *full_text = 0;
                return &block;
        }

        static unsigned long long rx_bytes_prev, tx_bytes_prev;
        static struct timespec ts_prev;

        double interval = (double)(ts.tv_sec - ts_prev.tv_sec) +
                (double)(ts.tv_nsec - ts_prev.tv_nsec) / 1e9;
        double rx_speed = (double)(rx_bytes - rx_bytes_prev) / interval;
        double tx_speed = (double)(tx_bytes - tx_bytes_prev) / interval;

        rx_bytes_prev = rx_bytes;
        tx_bytes_prev = tx_bytes;
        ts_prev = ts;

        static char rx_speed_text[8], tx_speed_text[8];
        print_with_suffix(rx_speed_text, size(rx_speed_text), rx_speed);
        print_with_suffix(tx_speed_text, size(tx_speed_text), tx_speed);

        if (snprintf(full_text, size(full_text), "NET %s↓ %s↑ B/s",
                        rx_speed_text, tx_speed_text) < 0)
                *full_text = 0;

        return &block;
}
