/*
  carpet.c -- carpet module;

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
draw_carpet (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  enum tile_fg f = fake (p);
  int d = fake_ext (p) + 1;

  draw_object_index_part (bitmap, "CARPET", d, f == CARPET
                          ? "MAIN" : "MAIN_TRAVERSABLE", p);

  draw_object_index_part (bitmap, "CARPET", d, "FACE", p);
}

void
draw_carpet_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  int d = fake_ext (p) + 1;
  draw_object_index_part (bitmap, "CARPET", d, "FRONT", p);

  struct pos par; prel (p, &par, -1, +1);

  switch (should_draw_face (p, f, RIGHT)) {
  case SHOULD_DRAW_FULL:
    push_drawn_rectangle (bitmap);
    draw_object_index_part (bitmap, "CARPET", d, "FACE", p);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p);
    break;
  case SHOULD_DRAW_PART:
    push_drawn_rectangle (bitmap);
    draw_object_index_part_width (bitmap, "CARPET", d, "FACE", p,
                                  CARPET_FG_WIDTH);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p);
    break;
  case SHOULD_DRAW_NONE: default: break;
  }
}
