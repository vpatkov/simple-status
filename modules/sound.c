#include "simple-status.h"
#include "sound.h"
#include <alsa/asoundlib.h>

/* in percents, negative if muted */
static int volume(void) {

#define E(x) { \
        if (x) { \
                if (mixer) \
                        snd_mixer_close(mixer); \
                error("sound: %s failed.", #x); \
                return 0; \
        } \
}

        snd_mixer_t *mixer = NULL;
        E(snd_mixer_open(&mixer, 0));
        E(snd_mixer_attach(mixer, "default"));
        E(snd_mixer_selem_register(mixer, NULL, NULL));
        E(snd_mixer_load(mixer));

        snd_mixer_selem_id_t *id;
        snd_mixer_selem_id_alloca(&id);
        snd_mixer_selem_id_set_index(id, 0);
        snd_mixer_selem_id_set_name(id, "Master");

        snd_mixer_elem_t *elem;
        E((elem = snd_mixer_find_selem(mixer, id)) == NULL);
        long v, vmax, vmin;
        E(snd_mixer_selem_get_playback_volume_range(elem, &vmin, &vmax));
        E(snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &v));
        int s;
        E(snd_mixer_selem_get_playback_switch(elem, SND_MIXER_SCHN_FRONT_LEFT, &s));

        snd_mixer_close(mixer);

#undef E

        int v_percents = (vmax-vmin == 0) ? 100 :
                100 * (v-vmin) / (vmax-vmin);

        return s ? v_percents : -v_percents;
}

struct block *sound_update(void) {
        const int volume_threshold = 70;

        static char full_text[16];
        static struct block block = {
                .full_text = full_text,
        };

        int v = volume();
        block.urgent = v < 0 || v >= volume_threshold;
        snprintf(full_text, size(full_text), "VOL %2d%%%s",
                abs(v), v >= 0 ? "" : " muted");
        return &block;
}
