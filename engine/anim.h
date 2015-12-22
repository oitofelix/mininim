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

#ifndef MININIM_ANIM_H
#define MININIM_ANIM_H

#include "prince.h"

/* types */
enum quit_anim {
  NO_QUIT, RESTART_LEVEL, QUIT_GAME
};

/* functions */
void play_anim (void (*draw_callback) (void),
                void (*sample_callback) (void),
                void (*compute_callback) (void),
                int freq);
void draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f);
void draw_xframe (ALLEGRO_BITMAP *bitmap, struct frame *f,
                  struct frame_offset *xf);
struct coord *xframe_coord (struct frame *f, struct frame_offset *xf,
                            struct coord *c);
struct frame *xframe_frame (struct frame *f, struct frame_offset *xf,
                            struct frame *nf);
struct frame *next_frame (struct frame *f, struct frame *nf,
                          struct frame_offset *fo);
void select_frame (struct anim *a, struct frameset *fs, int i);
void select_xframe (struct frame_offset *xf, struct frameset *fs, int j);
bool wait_anim (int cycles);

/* variables */
extern enum quit_anim quit_anim; /* set to true to quit animation */
extern bool next_frame_inv; /* invert draw_anim offset interpretation  */
extern bool cutscene; /* don't apply physics if set */

/* macros */
#define SCRIPT_HZ 12
#define CYCLE_TO_EFFECT_DURATION(x) ((x) * (EFFECT_HZ / SCRIPT_HZ))
#define SECS_TO_SCYCLES(x) ((x) * SCRIPT_HZ)

#endif	/* MININIM_ANIM_H */
