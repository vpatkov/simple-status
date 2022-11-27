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

static char *find_default_iface(void) {
        static const char *path = "/proc/net/route";
        FILE *f = fopen(path, "r");
        if (f == NULL) {
                error("network: can't open %s: %s.", path, strerror(errno));
                return "lo";
        }

        static char iface[16];
        unsigned long dest = -1;
        char line[128];
        while (fgets(line, size(line), f) != NULL)
                if (sscanf(line, "%15s %lu", iface, &dest) == 2 && dest == 0)
                        break;

        fclose(f);
        return (dest == 0) ? iface : "lo";
}

struct block *network_update(void) {
        static char full_text[32];
        static struct block block = {
                .full_text = full_text,
                .urgent = false,
        };

        char *iface = find_default_iface();
        if (strcmp(iface, "lo") == 0) {
                block.full_text = "NET  DISCONNECTED  ";
                block.urgent = true;
                return &block;
        }

        char rx_path[128], tx_path[128];
        if (snprintf(rx_path, size(rx_path),
                        "/sys/class/net/%s/statistics/rx_bytes", iface) < 0)
                *rx_path = 0;
        if (snprintf(tx_path, size(tx_path),
                        "/sys/class/net/%s/statistics/tx_bytes", iface) < 0)
                *tx_path = 0;

        unsigned long long rx_bytes, tx_bytes;
        if (pscanf(rx_path, "%llu", &rx_bytes) != 1 ||
            pscanf(tx_path, "%llu", &tx_bytes) != 1) {
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
