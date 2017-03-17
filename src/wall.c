/*
  wall.c -- wall module;

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

static void draw_wall_cache (ALLEGRO_BITMAP *bitmap, struct pos *p);

void
draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (tile_caching) {
    draw_object_part (bitmap, "WALL", "FRONT", p);
    draw_object_part (bitmap, "WALL", "FACE", p);
  } else draw_wall_cache (bitmap, p);
}

void
draw_wall_cache (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c; coord_object_part (&c, "WALL", "FRONT", p);
  struct pos np; npos (p, &np);
  struct coord nc; coord_object_part (&nc, "WALL", "FRONT", &np);
  struct coord vc; coord2room (&c, room_view, &vc);
  int x, y;
  if (mr_coord (np.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, nc.x, nc.y,
                         PLACE_WIDTH, PLACE_HEIGHT, &c, 0);
  else if (mr_coord (vc.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, vc.x, vc.y,
                         PLACE_WIDTH, PLACE_HEIGHT, &vc, 0);
  else if (mr_coord (p->room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, c.x, c.y,
                         PLACE_WIDTH, PLACE_HEIGHT, &c, 0);
  else {
    draw_object_part (bitmap, "WALL", "FRONT", p);
    draw_object_part (bitmap, "WALL", "FACE", p);
  }
}

enum should_draw
should_draw_face (struct pos *p, struct frame *f)
{
  if (! f) return false;

  struct pos ptr, ptl;
  surveyo (_tr, -4, +10, pos, f, NULL, &ptr, NULL);
  surveyo (_tl, -14, +10, pos, f, NULL, &ptl, NULL);

  if (is_fake (p)) return SHOULD_DRAW_PART;
  else if (is_sword_frame (f))
    return SHOULD_DRAW_NONE;
  else if (peq (&ptr, p) || peq (&ptl, p))
    return SHOULD_DRAW_FULL;
  else return SHOULD_DRAW_NONE;
}

void
draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  draw_object_part (bitmap, "WALL", "FRONT", p);

  struct pos par; prel (p, &par, -1, +1);
  struct pos pr; prel (p, &pr, +0, +1);

  switch (should_draw_face (p, f)) {
  case SHOULD_DRAW_FULL:
    push_drawn_rectangle (bitmap);
    draw_object_part (bitmap, "WALL", "FACE", p);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p);
    break;
  case SHOULD_DRAW_PART:
    push_drawn_rectangle (bitmap);
    draw_object_part_width (bitmap, "WALL", "FACE", p, WALL_FG_WIDTH);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p);
    break;
  case SHOULD_DRAW_NONE: default: break;
  }
}

enum wall_correlation
wall_correlation (struct pos *p)
{
  if (fake (p) != WALL) assert (false);

  enum tile_fg fl = fake_rel (p, 0, -1);
  enum tile_fg fr = fake_rel (p, 0, +1);

  if (fl != WALL && fr != WALL) return SWS;
  else if (fl != WALL && fr == WALL) return SWW;
  else if (fl == WALL && fr != WALL) return WWS;
  else if (fl == WALL && fr == WALL) return WWW;
  else return INVALID;
}
