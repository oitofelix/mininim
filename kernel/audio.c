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

#include <math.h>
#include <error.h>
#include <stdio.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "audio.h"

static ALLEGRO_SAMPLE_INSTANCE *sample_instance;
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
  ALLEGRO_SAMPLE *sample = al_load_sample (filename);
  if (! sample) error (-1, 0, "%s (\"%s\"): cannot load sample", __func__, filename);

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

void
play_sample (ALLEGRO_SAMPLE *sample)
{
  sample_instance = al_create_sample_instance (sample);
  if (! sample_instance)
    error (-1, 0, "%s (%p): cannot create sample instance", __func__, sample);

  ALLEGRO_MIXER *mixer = get_default_mixer ();

  if (! al_attach_sample_instance_to_mixer(sample_instance, mixer))
    error (-1, 0, "%s (%p): cannot attach sample instance to mixer (%p)",
           __func__, sample, sample_instance);

  if (! al_play_sample_instance (sample_instance))
    error (-1, 0, "%s (%p): cannot play sample instance (%p)",
           __func__, sample, sample_instance);
}

bool
is_playing_sample (void)
{
  return al_get_sample_instance_playing (sample_instance);
}

double
get_sample_position (void)
{
  if (! is_playing_sample ()) return INFINITY;
  return (double) al_get_sample_instance_position (sample_instance) /
    (double) al_get_sample_instance_frequency (sample_instance);
}
