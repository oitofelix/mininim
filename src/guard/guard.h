/*
  guard.h -- guard module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

/* functions */
struct actor *create_guard (struct actor *g0, struct actor *g1,
                           struct pos *p, enum dir dir);
ALLEGRO_BITMAP *apply_guard_palette (ALLEGRO_BITMAP *bitmap,
                                     enum actor_type type, int style);
void draw_guard_frame (ALLEGRO_BITMAP *bitmap, struct actor *g);
void draw_start_guards (ALLEGRO_BITMAP *bitmap);
bool is_guard (struct actor *a);
bool is_guard_by_type (enum actor_type t);
void draw_guard_hp (ALLEGRO_BITMAP *bitmap, struct actor *g);
palette get_guard_palette (int style);
void apply_guard_mode (struct actor *g, enum gm gm);
palette get_shadow_hp_palette (void);

/* variables */
extern struct actor *guarda;
extern size_t guarda_nmemb;

#endif	/* MININIM_GUARD_H */
