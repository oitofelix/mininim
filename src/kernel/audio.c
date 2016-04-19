/*
  audio.c -- audio module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

/* TODO: make volume distance recalculation algorithms multi-room
   aware.  Until then the volume is absolute wherever its source may
   be. */

static struct audio_sample *audio_sample;
static size_t audio_sample_nmemb;

bool audio_enabled = true;
float volume = 1.0;

void
init_audio (void)
{
  if (! al_install_audio ())
    error (-1, 0, "%s (void): cannot initialize audio", __func__);
  if (! al_init_acodec_addon ())
    error (-1, 0, "%s (void): cannot initialize audio codecs", __func__);
  if (! al_reserve_samples (16))
    error (-1, 0, "%s (void): cannot reserve audio samples", __func__);
}

void
finalize_audio (void)
{
  al_uninstall_audio ();
}

ALLEGRO_SAMPLE *
load_sample (char *filename)
{
  ALLEGRO_SAMPLE *sample =
    load_resource (filename, (load_resource_f) al_load_sample);

  if (! sample)
    error (-1, 0, "%s (\"%s\"): cannot load sample",
           __func__, filename);

  return sample;
}

void
enable_audio (bool b)
{
  ALLEGRO_MIXER *mixer = get_default_mixer ();

  if (b) {
    audio_enabled = true;
    set_mixer_gain (mixer, volume);
  }
  else {
    audio_enabled = false;
    set_mixer_gain (mixer, 0);
  }
}

void
set_mixer_gain (ALLEGRO_MIXER *mixer, float new_gain)
{
  if (! al_set_mixer_gain (mixer, new_gain))
    error (-1, 0, "%s (%p, %f): cannot set mixer gain", __func__, mixer, new_gain);
}

ALLEGRO_MIXER *
get_default_mixer (void)
{
  ALLEGRO_MIXER *mixer = al_get_default_mixer ();
  if (! mixer) error (-1, 0, "%s (void): default mixer not set", __func__);
  return mixer;
}

ALLEGRO_SAMPLE_INSTANCE *
play_sample (ALLEGRO_SAMPLE *sample, int room)
{
  size_t i;

  /* do nothing if the same sample has been played with less than a
     drawing cycle of difference in the same room */
  for (i = 0; i < audio_sample_nmemb; i++) {
    struct audio_sample *as = &audio_sample[i];
    if (as->sample == sample
        && as->anim_cycle == anim_cycle
        && as->room == room)
      return as->instance;
  }

  struct audio_sample as;
  as.played = false;
  as.sample = sample;
  as.instance = al_create_sample_instance (sample);
  as.anim_cycle = anim_cycle;
  as.room = room;
  as.volume = -1;

  if (! as.instance)
    error (-1, 0, "%s (%p): cannot create sample instance", __func__, sample);

  audio_sample =
    add_to_array (&as, 1, audio_sample, &audio_sample_nmemb,
                  audio_sample_nmemb, sizeof (as));

  qsort (audio_sample, audio_sample_nmemb, sizeof (as), compare_samples);
  return as.instance;
}

void
play_samples (void)
{
  clear_played_samples ();
  /* adjust_samples_volume (); */

  size_t i;
  for (i = 0; i < audio_sample_nmemb; i++) {
    struct audio_sample *as = &audio_sample[i];
    if (! as->played) {
      ALLEGRO_MIXER *mixer = get_default_mixer ();

      if (! al_attach_sample_instance_to_mixer(as->instance, mixer))
        error (-1, 0, "%s: cannot attach sample instance to mixer (%p, %p)",
                __func__, as->sample, as->instance);

      /* as->volume = get_adjusted_sample_volume (as); */
      al_set_sample_instance_gain (as->instance, as->volume);

      if (! al_play_sample_instance (as->instance))
        error (-1, 0, "%s: cannot play sample instance (%p, %p)",
                __func__, as->sample, as->instance);

      as->played = true;
    } else al_set_sample_instance_gain (as->instance, as->volume);
  }
}

int
compare_samples (const void *s0, const void *s1)
{
  struct audio_sample *_s0 = (struct audio_sample *) s0;
  struct audio_sample *_s1 = (struct audio_sample *) s1;

  if (_s0->instance < _s1->instance) return -1;
  else if (_s0->instance > _s1->instance) return 1;
  else return 0;
}

struct audio_sample *
get_audio_sample (ALLEGRO_SAMPLE_INSTANCE *si)
{
  struct audio_sample as;
  as.instance = si;
  return bsearch (&as, audio_sample, audio_sample_nmemb, sizeof (as),
                  compare_samples);
}

double
get_sample_position (ALLEGRO_SAMPLE_INSTANCE *si)
{
  struct audio_sample *as = get_audio_sample (si);
  if (! as) return INFINITY;
  else if (! as->played) return 0;
  else if (! al_get_sample_instance_playing (si)) return INFINITY;
  else return (double) al_get_sample_instance_position (si) /
         (double) al_get_sample_instance_frequency (si);
}

bool
is_playing_sample_instance (struct ALLEGRO_SAMPLE_INSTANCE *si)
{
  if (isfinite (get_sample_position (si))) return true;
  else return false;
}

bool
is_playing_sample (struct ALLEGRO_SAMPLE *s)
{
  size_t i;
  for (i = 0; i < audio_sample_nmemb; i++) {
    struct audio_sample *as = &audio_sample[i];
    if (s == as->sample
        && is_playing_sample_instance (as->instance)) return true;
  }
  return false;
}

void
remove_sample (struct audio_sample *s)
{
  al_destroy_sample_instance (s->instance);
  size_t i =  s - audio_sample;
  audio_sample =
    remove_from_array (audio_sample, &audio_sample_nmemb, i, 1, sizeof (*s));
  if (audio_sample_nmemb == 0) audio_sample = NULL;
}

void
clear_played_samples (void)
{
  size_t i;
  for (i = 0; i < audio_sample_nmemb; i++) {
    struct audio_sample *as = &audio_sample[i];
    if (as->played
        && ! al_get_sample_instance_playing (as->instance)) {
      remove_sample (as);
      if (i > 0) i--;
    }
  }
}

void
adjust_samples_volume (void)
{
  size_t i;
  for (i = 0; i < audio_sample_nmemb; i++) {
    struct audio_sample *as = &audio_sample[i];
    as->volume = get_adjusted_sample_volume (as);
  }
}

float
get_adjusted_sample_volume (struct audio_sample *as)
{
  if (as->room < 0) return 1.0;
  /* int d = room_dist (room_view, as->room, 10); */
  /* if (d == 0) d = 1; */
  /* if (d <= 10) return 1.0 / d; */
  if (is_room_visible (as->room)) return 1.0;
  else return 0.5;
}

void
stop_all_samples (void)
{
  while (audio_sample_nmemb)
    remove_sample (&audio_sample[0]);
}

void
stop_sample (ALLEGRO_SAMPLE_INSTANCE *si,
             ALLEGRO_SAMPLE *s)
{
  struct audio_sample *as = get_audio_sample (si);
  if (! as) return;

  if (s && as->sample != s) return;

  remove_sample (as);
}

bool
is_instance_of_sample (ALLEGRO_SAMPLE_INSTANCE *si,
             ALLEGRO_SAMPLE *s)
{
  struct audio_sample *as = get_audio_sample (si);
  return si && s && as && as->sample == s;
}
