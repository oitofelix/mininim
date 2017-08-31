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
  if (f && ! should_draw_face (p, f, LEFT)) {
    struct actor *a = get_actor_by_id (f->parent_id);

    invert_frame_dir (&a->f, &a->f);

    ALLEGRO_TRANSFORM trans;
    lua_Number dx = - a->f.c.x + (2 * PLACE_WIDTH * p->place + 30)
      - (a->f.c.x + IW (get_bitmap_width (a->f.b)));
    al_build_transform (&trans, OW (dx), 0, 1, 1, 0);
    al_set_target_bitmap (bitmap);
    al_use_transform (&trans);

    push_clipping_rectangle (bitmap, OW (PLACE_WIDTH * p->place + 2),
                             OH (PLACE_HEIGHT * p->floor + 3),
                             OW (16), OH (PLACE_HEIGHT - 9));
    draw_actor_frame (bitmap, a);
    pop_clipping_rectangle ();

    al_identity_transform (&trans);
    al_set_target_bitmap (bitmap);
    al_use_transform (&trans);

    invert_frame_dir (&a->f, &a->f);

    /* draw mirror properly */
    push_reset_clipping_rectangle (bitmap);
    draw_object_part_width (bitmap, "MIRROR", "FACE", p, 19);
    pop_clipping_rectangle ();
  } else draw_object_part_width (bitmap, "MIRROR", "MAIN", p, 19);

  ignore_clipping_rectangle_intersection = false;
}
