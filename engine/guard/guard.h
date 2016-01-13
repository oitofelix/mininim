/*
  guard.h -- guard module;

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

#ifndef MININIM_GUARD_H
#define MININIM_GUARD_H

#include "kernel/audio.h"

/* kid actions */
#include "guard-normal.h"
#include "guard-vigilant.h"
#include "guard-hit.h"
#include "guard-die.h"
#include "guard-fall.h"
#include "guard-walkf.h"
#include "guard-walkb.h"
#include "guard-attack.h"
#include "guard-defense.h"

/* bitmaps */
#define GUARD_LIFE "data/life/guard.png"
#define GUARD_SPLASH "data/splash/guard.png"

/* sounds */
#define GUARD_HIT_SAMPLE "dat/digisnd1/taking a life to the enemy.ogg"

/* functions */
void load_guard (void);
void unload_guard (void);
struct anim *create_guard (struct anim *g0, struct anim *g1,
                           struct pos *p, enum dir dir);
void draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct anim *g,
                       enum vm);

/* variables */
extern struct anim *guarda;
extern size_t guarda_nmemb;

extern ALLEGRO_SAMPLE *guard_hit_sample;

#endif	/* MININIM_GUARD_H */
