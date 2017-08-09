/*
  sword.c -- sword module;

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

void
draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  push_drawn_rectangle (bitmap);
  draw_object (bitmap, "SWORD_ITEM", p);
  struct drawn_rectangle *dr = pop_drawn_rectangle ();
  push_clipping_rectangle (dr->bitmap, dr->x, dr->y, dr->w, dr->h);
  draw_tile_fg_front (bitmap, p, NULL);
  struct pos pr; prel (p, &pr, +0, +1);
  draw_tile_fg_front (bitmap, &pr, NULL);
  pop_clipping_rectangle ();
}

bool
is_sword_frame (struct frame *f)
{
  size_t i;
  for (i = 0; i < actor_nmemb; i++)
    if (f->b == actor[i].xf.b) return true;
  return false;
}
