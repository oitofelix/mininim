/*
  audio.c -- audio module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

static struct audio_instance *audio_instance;
static size_t audio_instance_nmemb;

static ALLEGRO_AUDIO_STREAM *load_audio_stream (const char *filename);

float audio_volume = 1.0;

void
init_audio (void)
{
  if (! al_install_audio ())
    error (0, 0, "%s (void): cannot initialize audio", __func__);
  if (! al_init_acodec_addon ())
    error (0, 0, "%s (void): cannot initialize audio codecs", __func__);
  al_reserve_samples (RESERVED_AUDIO_SAMPLES);
  set_audio_volume (audio_volume);
}

void
finalize_audio (void)
{
  al_uninstall_audio ();
}

void
set_audio_volume (float volume)
{
  ALLEGRO_MIXER *mixer = al_get_default_mixer ();
  al_set_mixer_gain (mixer, volume);
  audio_volume = volume;
}

static ALLEGRO_AUDIO_STREAM *
load_audio_stream (const char *filename)
{
  ALLEGRO_MIXER *mixer = al_get_default_mixer ();
  unsigned int freq = al_get_mixer_frequency(mixer);
  return al_load_audio_stream (filename, 2, (float) freq
                               / (float) DEFAULT_HZ);
}

bool
audio_source_eq (struct audio_source *as0, struct audio_source *as1)
{
  if (as0->type != as1->type) return false;

  switch (as0->type) {
  case AUDIO_SAMPLE:
    return as0->data.sample == as1->data.sample;
    break;
  case AUDIO_STREAM:
#if WINDOWS_PORT
    return ! strcasecmp (as0->data.stream, as1->data.stream);
#else
    return ! strcmp (as0->data.stream, as1->data.stream);
#endif
    break;
  default: assert (false); break;
  }

  return false;
}

struct audio_source *
load_audio (struct audio_source *as, enum audio_type audio_type,
            const char *filename)
{
  switch (audio_type) {
  case AUDIO_SAMPLE:
    as->data.sample = (ALLEGRO_SAMPLE *)
      load_resource (filename, (load_resource_f) al_load_sample, true);
    if (! as->data.sample) {
      error (0, 0, "%s (\"%s\"): cannot load sample",
             __func__, filename);
      return NULL;
    }
    break;
  case AUDIO_STREAM:
    as->data.stream = xasprintf ("%s", filename);
    break;
  default: assert (false); break;
  }

  as->type = audio_type;

  if (load_callback) load_callback ();

  return as;
}

bool
is_audio_rendering ()
{
  return rendering & AUDIO_RENDERING;
}

union audio_instance_data
play_audio (struct audio_source *as, struct pos *p, int actor_id)
{
  if (! is_audio_rendering ())
    return (union audio_instance_data) {NULL};

  /* do nothing if the same sample has been played in a near cycle */
  struct audio_instance *sai =
    search_audio_instance (as, anim_cycle, NULL, -1);
  if (sai) return sai->data;

  struct audio_instance ai;

  ai.source = as;
  ai.played = false;
  ai.anim_cycle = anim_cycle;
  ai.actor_id = actor_id;
  ai.volume = -1;

  if (p) npos (p, &ai.p);
  else p = NULL;

  switch (as->type) {
  case AUDIO_SAMPLE:
    ai.position.sample = 0;
    ai.data.sample = al_create_sample_instance (as->data.sample);
    if (! ai.data.sample)
      return (union audio_instance_data) {.sample = NULL};
    break;
  case AUDIO_STREAM:
    ai.position.stream = 0;
    ai.data.stream = (ALLEGRO_AUDIO_STREAM *)
      load_resource
      (as->data.stream, (load_resource_f) load_audio_stream, true);
    if (! ai.data.stream) {
      error (0, 0, "%s (\"%s\"): cannot load audio stream", __func__,
             as->data.stream);
      return (union audio_instance_data) {.stream = NULL};
    }
    break;
  default: assert (false); break;
  }

  audio_instance =
    add_to_array (&ai, 1, audio_instance, &audio_instance_nmemb,
                  audio_instance_nmemb, sizeof (ai));

  qsort (audio_instance, audio_instance_nmemb, sizeof (ai), compare_audio_instances);

  return ai.data;
}

int
compare_audio_instances (const void *_ai0, const void *_ai1)
{
  struct audio_instance *ai0 = (struct audio_instance *) _ai0;
  struct audio_instance *ai1 = (struct audio_instance *) _ai1;

  void *d0 = ai0->data.sample;
  void *d1 = ai1->data.sample;

  if (d0 < d1) return -1;
  else if (d0 > d1) return 1;
  else return 0;
}

struct audio_instance *
get_audio_instance (union audio_instance_data data)
{
  struct audio_instance ai;
  ai.data = data;
  return bsearch (&ai, audio_instance, audio_instance_nmemb, sizeof (ai),
                  compare_audio_instances);
}

struct audio_instance *
search_audio_instance (struct audio_source *as, uint64_t anim_cycle,
                       struct pos *p, int actor_id)
{
  size_t i;
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];
    if ((! as
         || (as->type == AUDIO_SAMPLE
             && as->data.sample == ai->source->data.sample)
         || (as->type == AUDIO_STREAM
             && as->data.stream == ai->source->data.stream))
        && (anim_cycle == 0 || anim_cycle - ai->anim_cycle < 2)
        && (! p || peq (p, &ai->p))
        && (actor_id < 0 || actor_id == ai->actor_id))
      return ai;
  }
  return NULL;
}

void
play_audio_instances (void)
{
  clear_played_audio_instances ();
  /* adjust_samples_volume (); */

  size_t i;
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];

    switch (ai->source->type) {
    case AUDIO_SAMPLE:
      al_set_sample_instance_gain (ai->data.sample, ai->volume);
      break;
    case AUDIO_STREAM:
      al_set_audio_stream_gain (ai->data.stream, ai->volume);
      break;
    }

    if (! ai->played) {
      ALLEGRO_MIXER *mixer = al_get_default_mixer ();

      ai->played = true;
      /* ai->volume = get_adjusted_sample_volume (ai); */

      switch (ai->source->type) {
      case AUDIO_SAMPLE:
        al_set_sample_instance_gain (ai->data.sample, ai->volume);
        al_attach_sample_instance_to_mixer (ai->data.sample, mixer);
        al_play_sample_instance (ai->data.sample);
        break;
      case AUDIO_STREAM:
        al_set_audio_stream_gain (ai->data.stream, ai->volume);
        al_attach_audio_stream_to_mixer (ai->data.stream, mixer);
        break;
      }
    }
  }
}

bool
pause_audio_instance (union audio_instance_data data, bool val)
{
  struct audio_instance *ai = get_audio_instance (data);

  if (! is_audio_instance_playing (ai->data))
    return false;

  if (val) {
    switch (ai->source->type) {
    case AUDIO_SAMPLE:
      if (! al_get_sample_instance_playing (ai->data.sample))
        return true;
      ai->position.sample =
        al_get_sample_instance_position (ai->data.sample);
      return
        al_set_sample_instance_playing (ai->data.sample, false);
      break;
    case AUDIO_STREAM:
      if (! al_get_audio_stream_playing (ai->data.stream))
        return true;
      ai->position.stream =
        al_get_audio_stream_position_secs (ai->data.stream);
      return al_set_audio_stream_playing (ai->data.stream, false);
      break;
    default: assert (false);
    }
  } else {
    switch (ai->source->type) {
    case AUDIO_SAMPLE:
      if (al_get_sample_instance_playing (ai->data.sample))
        return true;
      al_set_sample_instance_position (ai->data.sample,
                                       ai->position.sample);
      ai->position.sample = 0;
      return
        al_set_sample_instance_playing (ai->data.sample, true);
      break;
    case AUDIO_STREAM:
      if (al_get_audio_stream_playing (ai->data.stream))
        return true;
      ai->position.stream = 0;
      /* al_seek_audio_stream_secs (ai->data.stream, */
      /*                            ai->position.stream); */
      return
        al_set_audio_stream_playing (ai->data.stream, true);
      break;
    default: assert (false);
    }
  }

  return false;
}

void
pause_audio_instances (bool val)
{
  size_t i;
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];
    pause_audio_instance (ai->data, val);
  }
}

void
clear_played_audio_instances (void)
{
  size_t i;
 again:
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];
    if (! is_audio_instance_playing (ai->data)) {
      remove_audio_instance (ai);
      goto again;
    }
  }
}

double
get_audio_instance_position (union audio_instance_data data)
{
  struct audio_instance *ai = get_audio_instance (data);
  if (! ai) return INFINITY;
  else if (! ai->played) return 0;
  else if ((ai->source->type == AUDIO_SAMPLE
            && al_get_sample_instance_position (ai->data.sample) == 0
            && ai->position.sample == 0
            && ! al_get_sample_instance_playing (ai->data.sample))
           || (ai->source->type == AUDIO_STREAM
               && al_get_audio_stream_position_secs (ai->data.stream)
               >= al_get_audio_stream_length_secs (ai->data.stream)))
    return INFINITY;
  else switch (ai->source->type) {
    case AUDIO_SAMPLE:
      return (double) al_get_sample_instance_position (ai->data.sample) /
        (double) al_get_sample_instance_frequency (ai->data.sample);
    case AUDIO_STREAM:
      return al_get_audio_stream_position_secs (ai->data.stream);
    default: assert (false); break;
  }

  return INFINITY;
}

bool
is_audio_instance_playing (union audio_instance_data data)
{
  return isfinite (get_audio_instance_position (data));
}

bool
is_audio_instance_paused (union audio_instance_data data)
{
  struct audio_instance *ai = get_audio_instance (data);
  return
    is_audio_instance_playing (data)
    && ((ai->source->type == AUDIO_SAMPLE
         && ai->position.sample > 0
         && ! al_get_sample_instance_playing (ai->data.sample))
        || (ai->source->type == AUDIO_STREAM
            && ! al_get_audio_stream_playing (ai->data.stream)));
}

struct audio_instance *
is_audio_source_playing (struct audio_source *as)
{
  size_t i;
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];
    if (((as->type == AUDIO_SAMPLE
          && as->data.sample == ai->source->data.sample)
         || (as->type == AUDIO_STREAM
             && as->data.stream == ai->source->data.stream))
        && is_audio_instance_playing (ai->data))
      return ai;
  }
  return NULL;
}

void
destroy_audio (struct audio_source *as)
{
  switch (as->type) {
  case AUDIO_SAMPLE:
    if (as->data.sample) al_destroy_sample (as->data.sample);
    break;
  case AUDIO_STREAM:
    al_free (as->data.stream);
    break;
  default: assert (false); break;
  }
}

void
remove_audio_instance (struct audio_instance *ai)
{
  switch (ai->source->type) {
  case AUDIO_SAMPLE:
    al_destroy_sample_instance (ai->data.sample);
    break;
  case AUDIO_STREAM:
    al_destroy_audio_stream (ai->data.stream);
    break;
  default: assert (false); break;
  }

  size_t i =  ai - audio_instance;
  audio_instance =
    remove_from_array (audio_instance, &audio_instance_nmemb, i, 1, sizeof (*ai));

  if (audio_instance_nmemb == 0) audio_instance = NULL;
}

void
adjust_audio_instances_volume (void)
{
  size_t i;
  for (i = 0; i < audio_instance_nmemb; i++) {
    struct audio_instance *ai = &audio_instance[i];
    ai->volume = get_adjusted_audio_instance_volume (ai);
  }
}

float
get_adjusted_audio_instance_volume (struct audio_instance *ai)
{
  return 1.0;
  /* if (! ai->p) return 1.0; */
  /* int d = room_dist (mr.room, ai->p.room, 10); */
  /* if (d == 0) d = 1; */
  /* if (d <= 10) return 1.0 / d; */
  /* if (is_room_visible (ai->p.room)) return 1.0; */
  /* else return 0.5; */
}

void
stop_audio_instances (void)
{
  while (audio_instance_nmemb)
    remove_audio_instance (&audio_instance[0]);
}

bool
stop_audio_instance (struct audio_source *as, struct pos *p, int actor_id)
{
  struct audio_instance *ai = search_audio_instance (as, 0, p, actor_id);
  if (ai) remove_audio_instance (ai);
  return ai;
}

bool
is_instance_of_audio_source (union audio_instance_data data,
                             struct audio_source *as)
{
  struct audio_instance *ai = get_audio_instance (data);
  return as && ai && as == ai->source;
}
