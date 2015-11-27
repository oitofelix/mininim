/*
  anim.h -- animation module;

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

#ifndef FREEPOP_ANIM_H
#define FREEPOP_ANIM_H

#include "prince.h"

/* functions */
void play_anim (void (*callback) (void), int freq);
void draw_anim (ALLEGRO_BITMAP *bitmap, struct anim a);
void draw_xanim (ALLEGRO_BITMAP *bitmap, struct anim a);
struct coord xanim_coord (struct anim a);
struct anim anim_from_xanim (struct anim a);
struct anim next_anim (struct anim a, ALLEGRO_BITMAP* frame, int dx, int dy);
bool wait_anim (int cycles);

/* variables */
extern bool quit_anim; /* set to true to quit animation */
extern bool next_anim_inv; /* invert draw_anim offset interpretation  */
extern bool cutscene; /* don't apply physics if set */

/* macros */
#define SCRIPT_HZ 12
#define CYCLE_TO_EFFECT_DURATION(x) ((x) * (EFFECT_HZ / SCRIPT_HZ))
#define SECS_TO_SCYCLES(x) ((x) * SCRIPT_HZ)

#endif	/* FREEPOP_ANIM_H */
