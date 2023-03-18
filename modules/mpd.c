#include <mpd/client.h>
#include "simple-status.h"
#include "mpd.h"

static struct mpd_connection *conn;

struct block *mpd_update(void) {
	static char full_text[64];
	static struct block block = {
		.urgent = false,
	};

	struct mpd_status *status;
	if (!mpd_send_status(conn) || !(status = mpd_recv_status(conn))) {
		error("mpd: can't get status.");
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
		error("mpd: can't get current song.");
		block.full_text = "";
		return &block;
	}
	const char *artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
	const char *title = mpd_song_get_tag(song, MPD_TAG_TITLE, 0);
	snprintf(full_text, size(full_text), "%s: %s",
		artist ? artist : "???", title ? title : "???");
	mpd_song_free(song);
	mpd_response_finish(conn);

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
		mpd_connection_free(conn);
		exit(EXIT_FAILURE);
	}
}

void mpd_deinit(void) {
	if (conn != NULL)
		mpd_connection_free(conn);
}
