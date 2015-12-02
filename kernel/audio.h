/*
  audio.h -- audio module;

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

#ifndef FREEPOP_AUDIO_H
#define FREEPOP_AUDIO_H

#include <allegro5/allegro_audio.h>

/* functions */
void init_audio (void);
void finalize_audio (void);
ALLEGRO_SAMPLE *load_sample (char *filename);
void play_sample (ALLEGRO_SAMPLE *sample);
bool is_playing_sample (void);
double get_sample_position (void);

/* variables */
extern bool audio_enabled;

#endif	/* FREEPOP_AUDIO_H */
