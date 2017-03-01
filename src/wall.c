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

void
draw_wall_part (ALLEGRO_BITMAP *bitmap, struct pos *p, char *part)
{
  lua_State *L = main_L;
  if (! L_push_video_routine (L)) return;

  lua_pushstring (L, "DRAW");
  lua_pushstring (L, "WALL");
  lua_pushstring (L, part);
  L_pushposition (L, p);

  L_target_bitmap = bitmap;
  L_call (L, 4, 0);
  L_target_bitmap = NULL;
}

void
draw_wall_part_width (ALLEGRO_BITMAP *bitmap, struct pos *p, char *part,
                      int width)
{
  lua_State *L = main_L;
  if (! L_push_video_routine (L)) return;

  lua_pushstring (L, "DRAW");
  lua_pushstring (L, "WALL");
  lua_pushstring (L, part);
  L_pushposition (L, p);
  if (width >= 0) lua_pushnumber (L, width);
  else lua_pushnil (L);

  L_target_bitmap = bitmap;
  L_call (L, 5, 0);
  L_target_bitmap = NULL;
}

void
draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_wall_part (bitmap, p, "BASE");
  draw_wall_part (bitmap, p, "LEFT");
  draw_wall_part (bitmap, p, "RIGHT");
  draw_wall_part (bitmap, p, "TOP");
}

void
draw_wall_left_cache (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c; wall_left_coord (p, &c);
  struct pos np; npos (p, &np);
  struct coord nc; wall_left_coord (&np, &nc);
  struct coord vc; coord2room (&c, room_view, &vc);
  int x, y;
  if (mr_coord (np.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, nc.x, nc.y,
                         PLACE_WIDTH, PLACE_HEIGHT - 3, &c, 0);
  else if (mr_coord (vc.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, vc.x, vc.y,
                         PLACE_WIDTH, PLACE_HEIGHT - 3, &vc, 0);
  else if (mr_coord (p->room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, c.x, c.y,
                         PLACE_WIDTH, PLACE_HEIGHT - 3, &c, 0);
  else draw_wall_part (bitmap, p, "LEFT");
}

void
draw_wall_base_cache (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c; wall_base_coord (p, &c);
  struct pos np; npos (p, &np);
  struct coord nc; wall_base_coord (&np, &nc);
  struct coord vc; coord2room (&c, room_view, &vc);
  int x, y;
  if (mr_coord (np.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, nc.x, nc.y,
                         PLACE_WIDTH, 3, &c, 0);
  else if (mr_coord (vc.room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, vc.x, vc.y,
                         PLACE_WIDTH, 3, &vc, 0);
  else if (mr_coord (p->room, -1, &x, &y))
    draw_bitmap_regionc (mr.cell[x][y].cache, bitmap, c.x, c.y,
                         PLACE_WIDTH, 3, &c, 0);
  else draw_wall_part (bitmap, p, "BASE");
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
draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
              enum em em, enum vm vm)
{
  draw_wall_base_cache (bitmap, p);
  draw_wall_left_cache (bitmap, p);

  struct pos par; prel (p, &par, -1, +1);
  struct pos pr; prel (p, &pr, +0, +1);

  switch (should_draw_face (p, f)) {
  case SHOULD_DRAW_FULL:
    push_drawn_rectangle (bitmap);
    draw_confg_right (bitmap, p, em, vm);
    draw_confg_top (bitmap, p, em, vm);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p, em, vm);
    break;
  case SHOULD_DRAW_PART:
    push_drawn_rectangle (bitmap);
    draw_wall_part_width (bitmap, p, "RIGHT", WALL_FG_WIDTH);
    if (fake (&par) == NO_FLOOR)
      draw_wall_part_width (bitmap, p, "TOP", WALL_FG_WIDTH);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p, em, vm);
    break;
  case SHOULD_DRAW_NONE: default: break;
  }
}

enum wall_correlation
wall_correlation (struct pos *p)
{
  if (fake (p) != WALL) assert (false);

  enum confg fl = fake_rel (p, 0, -1);
  enum confg fr = fake_rel (p, 0, +1);

  if (fl != WALL && fr != WALL) return SWS;
  else if (fl != WALL && fr == WALL) return SWW;
  else if (fl == WALL && fr != WALL) return WWS;
  else if (fl == WALL && fr == WALL) return WWW;
  else return INVALID;
}

struct coord *
wall_base_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * (p->floor + 1));
}

struct coord *
wall_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 3);
}
