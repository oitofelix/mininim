/*
  mirror.c -- mirror module;

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

struct mirror *mirror = NULL;
size_t mirror_nmemb = 0;

void
draw_mirror_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  ignore_clipping_rectangle_intersection = true;
  /* draw actor */
  if (f) {
    push_clipping_rectangle (bitmap, OW (PLACE_WIDTH * p->place + 2),
                             OH (PLACE_HEIGHT * p->floor + 3),
                             OW (16), OH (PLACE_HEIGHT - 9));
    struct actor *a = get_actor_by_id (f->parent_id);
    struct actor a0 = *a;
    invert_frame_dir (&a0.f, &a0.f);
    a0.f.c.x = (2 * PLACE_WIDTH * p->place + 36)
      - (a->f.c.x + IW (get_bitmap_width (a->f.b)));
    draw_actor_frame (bitmap, &a0);
    pop_clipping_rectangle ();
  }

  /* draw mirror properly */
  push_reset_clipping_rectangle (bitmap);
  draw_object_part_width (bitmap, "MIRROR", "FACE", p, 19);
  pop_clipping_rectangle ();

  ignore_clipping_rectangle_intersection = false;
}
