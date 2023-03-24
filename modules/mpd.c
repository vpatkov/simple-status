#include <mpd/client.h>
#include "simple-status.h"
#include "mpd.h"

static struct mpd_connection *conn;

/* Remove invalid trailing UTF-8 code point, if any */
static void fix_trailing_codepoint(char *s) {
	if (*s == 0)
		return;

	size_t len = strlen(s);
	char *p = s + len - 1;
	while (p > s && (*p & 0xc0) == 0x80)
		p--;

	int n;
	if ((*p & 0x80) == 0)
		n = 1;
	else if ((*p & 0xe0) == 0xc0)
		n = 2;
	else if ((*p & 0xf0) == 0xe0)
		n = 3;
	else if ((*p & 0xf8) == 0xf0)
		n = 4;
	else
		n = 0;

	if (p + n <= s + len)
		p[n] = 0;
	else
		p[0] = 0;
}

/* Simple JSON string sanitizer */
static void sanitize(char *s) {
	for (char c; (c = *s) != 0; s++)
		if (c == '"' || c == '\\' || (c > 0 && c <= 0x1f) || c == 0x7f)
			*s = ' ';
}

struct block *mpd_update(void) {
	static char full_text[64];
	static struct block block = {
		.urgent = false,
	};

	if (conn == NULL) {
		mpd_init();
		if (conn == NULL) {
			block.full_text = "";
			return &block;
		}
	}

	struct mpd_status *status;
	if (!mpd_send_status(conn) || !(status = mpd_recv_status(conn))) {
		error("mpd: %s.", mpd_connection_get_error_message(conn));
		if (mpd_connection_get_error(conn) == MPD_ERROR_CLOSED) {
			mpd_deinit();
			mpd_init();
		}
		block.full_text = "";
		return &block;
	}

	enum mpd_state state = mpd_status_get_state(status);
	mpd_status_free(status);
	mpd_response_finish(conn);
	if (state != MPD_STATE_PLAY) {
		block.full_text = "";
		return &block;
	}

	struct mpd_song *song;
	if (!mpd_send_current_song(conn) || !(song = mpd_recv_song(conn))) {
		error("mpd: %s.", mpd_connection_get_error_message(conn));
		block.full_text = "";
		return &block;
	}

	const char *artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
	const char *title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
	int n = snprintf(full_text, size(full_text) - 1, "%s: %s",
		artist ? artist : "???", title ? title : "???");
	if (n >= (int)size(full_text) - 1) {
		fix_trailing_codepoint(full_text);
		strcat(full_text, ">");
	}

	mpd_song_free(song);
	mpd_response_finish(conn);
	sanitize(full_text);
	block.full_text = full_text;
	return &block;
}

void mpd_init(void) {
	conn = mpd_connection_new(NULL, 0, 0);
	if (conn == NULL) {
		error("mpd: out of memory.");
		exit(EXIT_FAILURE);
	}
	if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
		error("mpd: %s.", mpd_connection_get_error_message(conn));
		mpd_deinit();
	}
}

void mpd_deinit(void) {
	if (conn != NULL) {
		mpd_connection_free(conn);
		conn = NULL;
	}
}
