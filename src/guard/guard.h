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

/* bitmaps */
#define GUARD_LIFE "data/life/guard.png"
#define GUARD_SPLASH "data/splash/guard.png"

/* functions */
void load_guard (void);
void unload_guard (void);
struct anim *create_guard (struct anim *g0, struct anim *g1,
                           struct pos *p, enum dir dir);
ALLEGRO_BITMAP *apply_guard_palette (ALLEGRO_BITMAP *bitmap,
                                     enum anim_type type, int style,
                                     enum vm vm);
void draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct anim *g,
                       enum vm);
void draw_start_guards (ALLEGRO_BITMAP *bitmap, enum vm vm);
bool is_guard (struct anim *a);
bool is_guard_by_type (enum anim_type t);
void draw_guard_lives (ALLEGRO_BITMAP *bitmap, struct anim *g, enum vm vm);
palette get_guard_palette (int style, enum vm vm);
void apply_guard_mode (struct anim *g, enum gm gm);

/* variables */
extern struct anim *guarda;
extern size_t guarda_nmemb;

#endif	/* MININIM_GUARD_H */
