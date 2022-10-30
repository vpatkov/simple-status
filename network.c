#include "simple-status.h"
#include "network.h"
#include <time.h>

static unsigned long long read_ull(const char *path) {
        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("network: can't open %s: %s.", path, strerror(errno));
                return 0;
        }
        unsigned long long n;
        if (fscanf(f, "%llu", &n) != 1) {
                error("network: fscanf() on %s failed.", path);
                n = 0;
        }
        fclose(f);
        return n;
}

static void print_with_suffix(char *buf, size_t buf_len, double d) {
        int err;
        if (d >= 10e6)
                err = snprintf(buf, buf_len, "%4.0fM", d/1e6);
        else if (d >= 0.1e6)
                err = snprintf(buf, buf_len, "%3.1fM", d/1e6);
        else if (d >= 10e3)
                err = snprintf(buf, buf_len, "%4.0fk", d/1e3);
        else if (d >= 0.1e3)
                err = snprintf(buf, buf_len, "%3.1fk", d/1e3);
        else
                err = snprintf(buf, buf_len, "%4.0f", d);

        if (err < 0) {
                error("network: snprintf() failed.");
                *buf = 0;
        }
}

static double seconds_between(struct timespec before, struct timespec after) {
        double sec = (double)(after.tv_sec - before.tv_sec);
        double nsec = (double)(after.tv_nsec - before.tv_nsec) / 1e9;
        if (nsec >= 0)
                return sec + nsec;
        else
                return sec + nsec + 1.0;
}

char *network_update(void) {
        const double rx_threshold = 1e6;
        const double tx_threshold = 100e3;

        unsigned long long rx_bytes = read_ull("/sys/class/net/enp6s0/statistics/rx_bytes"),
                           tx_bytes = read_ull("/sys/class/net/enp6s0/statistics/tx_bytes");

        struct timespec ts;
        if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == -1) {
                error("network: clock_gettime() failed: %s.", strerror(errno));
                return "";
        }

        static unsigned long long rx_bytes_prev, tx_bytes_prev;
        static struct timespec ts_prev;

        double diff_seconds = seconds_between(ts_prev, ts);
        double rx_speed = (double)(rx_bytes - rx_bytes_prev) / diff_seconds;
        double tx_speed = (double)(tx_bytes - tx_bytes_prev) / diff_seconds;

        rx_bytes_prev = rx_bytes;
        tx_bytes_prev = tx_bytes;
        ts_prev = ts;

        static char text[32], rx_speed_text[8], tx_speed_text[8];

        print_with_suffix(rx_speed_text, size(rx_speed_text), rx_speed);
        print_with_suffix(tx_speed_text, size(tx_speed_text), tx_speed);

        if (snprintf(text, size(text), "NET %s↓ %s↑ B/s",
                        rx_speed_text, tx_speed_text) < 0) {
                error("network: snprintf() failed.");
                return "";
        }
        return text;
}
