/*
  room.c -- room module;

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

#include "mininim.h"

bool no_recursive_links_continuity;

void
load_room (void)
{
  load_floor ();
  load_broken_floor ();
  load_skeleton_floor ();
  load_loose_floor ();
  load_opener_floor ();
  load_closer_floor ();
  load_spikes_floor ();
  load_wall ();
  load_pillar ();
  load_big_pillar ();
  load_door ();
  load_level_door ();
  load_chopper ();
  load_bricks ();
  load_torch ();
  load_window ();
  load_balcony ();
  load_arch ();
  load_carpet ();
  load_stars ();
  load_mirror ();
}

void
unload_room (void)
{
  /* modules */
  unload_floor ();
  unload_broken_floor ();
  unload_skeleton_floor ();
  unload_loose_floor ();
  unload_opener_floor ();
  unload_closer_floor ();
  unload_spikes_floor ();
  unload_wall ();
  unload_pillar ();
  unload_big_pillar ();
  unload_door ();
  unload_level_door ();
  unload_chopper ();
  unload_bricks ();
  unload_torch ();
  unload_window ();
  unload_balcony ();
  unload_arch ();
  unload_carpet ();
  unload_stars ();
  unload_mirror ();
}

struct rect *
new_rect (struct rect *r, int room, int x, int y, int w, int h)
{
  r->c.room = room;
  r->c.x = x;
  r->c.y = y;
  r->w = w;
  r->h = h;
  return r;
}

void
draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                  ALLEGRO_COLOR color)
{
  struct coord nc = r->c;

  if (! cutscene && nc.room != room_view) {
    rect2room (r, room_view, &nc);
    if (nc.room != room_view) return;
  }

  draw_filled_rectangle (to, nc.x, nc.y, nc.x + r->w - 1,
                         nc.y + r->h - 1, color);
}

void
clear_rect_to_color (ALLEGRO_BITMAP *to, struct rect *r,
                     ALLEGRO_COLOR color)
{
  int op, src, dst;
  al_get_blender (&op, &src, &dst);
  al_set_blender (ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_ZERO);
  draw_filled_rect (to, r, color);
  al_set_blender (op, src, dst);
}

void
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              struct coord *c, int flags)
{
  int w = al_get_bitmap_width (from);
  int h = al_get_bitmap_height (from);
  draw_bitmap_regionc (from, to, 0, 0, w, h, c, flags);
}

void
draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                     float sx, float sy, float sw, float sh,
                     struct coord *c, int flags)
{
  if (! from) return;

  struct coord nc = *c;

  if (! cutscene && nc.room != room_view) {
    struct frame f;
    f.b = from;
    f.c = *c;
    frame2room (&f, room_view, &nc);
    if (nc.room != room_view) return;
  }

  draw_bitmap_region (from, to, sx, sy, sw, sh, nc.x, nc.y, flags);

  if (cutscene || con_caching
      || no_recursive_links_continuity) return;

  int x = nc.x;
  int y = nc.y;
  int room = nc.room;
  int w = sw;
  int h = sh;

  struct coord mc;

  int cx, cy,cw, ch;
  al_get_clipping_rectangle (&cx, &cy, &cw, &ch);
  al_reset_clipping_rectangle ();

  if (room == roomd (room, LEFT)
      && x < 0) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, ABOVE)
      && y < 3) {
    mc = nc;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, LEFT) && x < 0
      && room == roomd (room, ABOVE) && y < 3) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (room, ABOVE) && y < 3) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y += PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, LEFT)
      && x < 0
      && room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x += PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    mc = nc;
    mc.x -= PLACE_WIDTH * PLACES;
    mc.y -= PLACE_HEIGHT * FLOORS;
    draw_bitmap_region (from, to, sx, sy, sw, sh, mc.x, mc.y, flags);
  }

  al_set_clipping_rectangle (cx, cy, cw, ch);
}

void
draw_room (ALLEGRO_BITMAP *bitmap, int room,
           enum em em, enum vm vm)
{
  struct pos p;
  p.room = room;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_conbg (bitmap, &p, em, vm, false);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place <= PLACES; p.place++)
      draw_confg (bitmap, &p, em, vm, false);
}

void
draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm, bool redraw)
{
  switch (con (p)->bg) {
  case NO_BG:
    if (em == PALACE) draw_bricks_02 (bitmap, p, em, vm);
    break;
  case BRICKS_00: draw_bricks_00 (bitmap, p, em, vm); break;
  case BRICKS_01: draw_bricks_01 (bitmap, p, em, vm); break;
  case BRICKS_02: draw_bricks_02 (bitmap, p, em, vm); break;
  case BRICKS_03: draw_bricks_03 (bitmap, p, em, vm); break;
  case NO_BRICKS: break;
  case TORCH: draw_torch (bitmap, p, em, vm); break;
  case WINDOW: draw_window (bitmap, p, em, vm); break;
  case BALCONY: draw_balcony (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown background (%i)",
           __func__, con (p)->bg);
  }

  if (con (p)->fg == LEVEL_DOOR)
    draw_level_door (bitmap, p, em, vm);

  if (! redraw) return;

  struct pos pr; prel (p, &pr, +0, +1);

  if (con (&pr)->fg == LEVEL_DOOR || con (&pr)->bg == BALCONY)
    draw_conbg (bitmap, &pr, em, vm, true);

  draw_confg (bitmap, p, em, vm, true);
  draw_confg_left (bitmap, &pr, em, vm, true);

  if (con (p)->fg == LEVEL_DOOR || con (p)->bg == BALCONY) {
    struct pos pl; prel (p, &pl, +0, -1);
    struct pos pal; prel (p, &pal, -1, -1);
    struct pos pa; prel (p, &pa, -1, +0);
    struct pos par; prel (p, &par, -1, +1);

    draw_confg (bitmap, &pl, em, vm, true);
    draw_conbg (bitmap, &pal, em, vm, true);
    draw_conbg (bitmap, &pa, em, vm, true);

    draw_confg_base (bitmap, &par, em, vm);
    draw_confg_left (bitmap, &par, em, vm, true);
  }
}

void
draw_confg_no_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm, bool redraw)
{
  draw_confg_base (bitmap, p, em, vm);
  draw_confg_left (bitmap, p, em, vm, redraw);
  draw_confg_right (bitmap, p, em, vm, redraw);
}

void
draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm, bool redraw)
{
  draw_confg_no_top (bitmap, p, em, vm, redraw);
  draw_confg_top (bitmap, p, em, vm, redraw);
}

void
draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  switch (con (p)->fg) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_base (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_base (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_base (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_base (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP: return;
  case WALL:
    if (con_caching) draw_wall_base (bitmap, p, em, vm);
    else draw_wall_base_cache (bitmap, p);
    break;
  case DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_base (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_floor_base (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_floor_base (bitmap, p, em, vm); break;
  case TCARPET: draw_door_pole_base (bitmap, p, em, vm); break;
  case MIRROR: draw_floor_base (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }
}

void
draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm, bool redraw)
{
  struct pos pl; prel (p, &pl, +0, -1);

  switch (con (p)->fg) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_left (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_left (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_left (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_left (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_left (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL:
    if (con_caching) draw_wall_left (bitmap, p, em, vm);
    else draw_wall_left_cache (bitmap, p);
    break;
  case DOOR: draw_door_left (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_level_door_left (bitmap, p, em, vm); break;
  case CHOPPER: draw_chopper_left (bitmap, p, em, vm); break;
  case ARCH_BOTTOM:
    draw_floor_left (bitmap, p, em, vm);
    draw_arch_bottom (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, p, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, p, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, p, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, p, em, vm); break;
  case CARPET:
    draw_floor_left (bitmap, p, em, vm);
    draw_door_pole (bitmap, p, em, vm);
    break;
  case TCARPET:
    if (con (p)->ext.design == ARCH_CARPET_RIGHT_00
        || con (p)->ext.design == ARCH_CARPET_RIGHT_01)
      draw_arch_top_right_end (bitmap, p, em, vm);
    else draw_door_pole (bitmap, p, em, vm);
    break;
  case MIRROR:
    draw_floor_left (bitmap, p, em, vm);
    draw_mirror (bitmap, p, em, vm);
    break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;

  if (con (p)->fg == ARCH_BOTTOM) {
    struct pos pa; prel (p, &pa, -1, +0);
    draw_confg_base (bitmap, &pa, em, vm);
  }
}

void
draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm, bool redraw)
{
  switch (con (p)->fg) {
  case NO_FLOOR: return;
  case FLOOR: draw_floor_right (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_right (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_right (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_right (bitmap, p, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_right (bitmap, p, em, vm); break;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_right (bitmap, p, em, vm); break;
  case WALL: draw_wall_right (bitmap, p, em, vm); break;
  case DOOR: draw_door_right (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_level_door_right (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_right (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_floor_right (bitmap, p, em, vm); break;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_carpet_right (bitmap, p, em, vm); break;
  case TCARPET: draw_carpet_right (bitmap, p, em, vm); break;
  case MIRROR: draw_floor_right (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;

  struct pos pr, pa;
  prel (p, &pr, +0, +1);
  prel (p, &pa, -1, +0);

  if (con (p)->fg == DOOR) {
    draw_confg_base (bitmap, &pa, em, vm);
    draw_confg_right (bitmap, &pa, em, vm, true);
  }

  draw_confg_base (bitmap, &pr, em, vm);
  draw_confg_left (bitmap, &pr, em, vm, true);
}

void
draw_confg_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm, bool redraw)
{
  switch (con (p)->fg) {
  case NO_FLOOR: return;
  case FLOOR: return;
  case BROKEN_FLOOR: return;
  case SKELETON_FLOOR: return;
  case LOOSE_FLOOR: return;
  case SPIKES_FLOOR: return;
  case OPENER_FLOOR: return;
  case CLOSER_FLOOR: return;
  case STUCK_FLOOR: return;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_top (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM: return;
  case BIG_PILLAR_TOP: draw_big_pillar_top_top (bitmap, p, em, vm); break;
  case WALL: draw_wall_top (bitmap, p, em, vm); break;
  case DOOR: draw_door_top (bitmap, p, em, vm); break;
  case LEVEL_DOOR: return;
  case CHOPPER: return;
  case ARCH_BOTTOM: return;
  case ARCH_TOP_MID: return;
  case ARCH_TOP_SMALL: return;
  case ARCH_TOP_LEFT: return;
  case ARCH_TOP_RIGHT: return;
  case CARPET: draw_carpet_top (bitmap, p, em, vm); break;
  case TCARPET: draw_carpet_top (bitmap, p, em, vm); break;
  case MIRROR: return;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;

  struct pos pa; prel (p, &pa, -1, +0);
  struct pos par; prel (p, &par, -1, +1);

  draw_confg_base (bitmap, &pa, em, vm);
  draw_confg_right (bitmap, &pa, em, vm, true);

  draw_confg_base (bitmap, &par, em, vm);
  draw_confg_left (bitmap, &par, em, vm, true);
}

void
draw_room_frame_fg (ALLEGRO_BITMAP *bitmap, enum em em, enum vm vm,
                    struct frame *f)
{
  struct coord nc;
  struct pos pbl, pbr, pm, ptl, ptr, np;

  survey (_bl, posf, f, &nc, &pbl, &np);
  survey (_br, posf, f, &nc, &pbr, &np);
  survey (_m, posf, f, &nc, &pm, &np);
  survey (_tl, posf, f, &nc, &ptl, &np);
  survey (_tr, posf, f, &nc, &ptr, &np);

  bool pbr_p = ! peq (&pbr, &pbl);
  bool pm_p = ! peq (&pm, &pbr) && ! peq (&pm, &pbl);
  bool ptl_p = ! peq (&ptl, &pm) && ! peq (&ptl, &pbr) && ! peq (&ptl, &pbl);
  bool ptr_p = ! peq (&ptr, &ptl) && ! peq (&ptr, &pm) && ! peq (&ptr, &pbr) && ! peq (&ptr, &pbl);

  struct coord c; frame2room (f, room_view, &c);

  int w = al_get_bitmap_width (f->b);
  int h = al_get_bitmap_height (f->b);

  set_target_bitmap (bitmap);
  al_set_clipping_rectangle (c.x, c.y, w, h);

  /* SPIKES FLOOR */
  if (con (&pbl)->fg == SPIKES_FLOOR) draw_spikes_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == SPIKES_FLOOR && pbr_p) draw_spikes_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == SPIKES_FLOOR && pm_p) draw_spikes_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == SPIKES_FLOOR && ptl_p) draw_spikes_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == SPIKES_FLOOR && ptr_p) draw_spikes_fg (bitmap, &ptr, em, vm);

  /* BROKEN FLOOR */
  if (con (&pbl)->fg == BROKEN_FLOOR) draw_broken_floor_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == BROKEN_FLOOR && pbr_p) draw_broken_floor_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == BROKEN_FLOOR && pm_p) draw_broken_floor_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == BROKEN_FLOOR && ptl_p) draw_broken_floor_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == BROKEN_FLOOR && ptr_p) draw_broken_floor_fg (bitmap, &ptr, em, vm);

  /* OPENER FLOOR */
  if (con (&pbl)->fg == OPENER_FLOOR) draw_opener_floor_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == OPENER_FLOOR && pbr_p) draw_opener_floor_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == OPENER_FLOOR && pm_p) draw_opener_floor_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == OPENER_FLOOR && ptl_p) draw_opener_floor_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == OPENER_FLOOR && ptr_p) draw_opener_floor_fg (bitmap, &ptr, em, vm);

  /* CLOSER FLOOR */
  if (con (&pbl)->fg == CLOSER_FLOOR) draw_closer_floor_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == CLOSER_FLOOR && pbr_p) draw_closer_floor_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == CLOSER_FLOOR && pm_p) draw_closer_floor_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == CLOSER_FLOOR && ptl_p) draw_closer_floor_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == CLOSER_FLOOR && ptr_p) draw_closer_floor_fg (bitmap, &ptr, em, vm);

  /* PILLAR */
  if (con (&pbl)->fg == PILLAR) draw_pillar_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == PILLAR && pbr_p) draw_pillar_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == PILLAR && pm_p) draw_pillar_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == PILLAR && ptl_p) draw_pillar_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == PILLAR && ptr_p) draw_pillar_fg (bitmap, &ptr, em, vm);

  /* BIG PILLAR BOTTOM */
  if (con (&pbl)->fg == BIG_PILLAR_BOTTOM) draw_big_pillar_bottom_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == BIG_PILLAR_BOTTOM && pbr_p) draw_big_pillar_bottom_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == BIG_PILLAR_BOTTOM && pm_p) draw_big_pillar_bottom_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == BIG_PILLAR_BOTTOM && ptl_p) draw_big_pillar_bottom_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == BIG_PILLAR_BOTTOM && ptr_p) draw_big_pillar_bottom_fg (bitmap, &ptr, em, vm);

  /* BIG PILLAR TOP */
  if (con (&pbl)->fg == BIG_PILLAR_TOP) draw_big_pillar_top_left (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == BIG_PILLAR_TOP && pbr_p) draw_big_pillar_top_left (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == BIG_PILLAR_TOP && pm_p) draw_big_pillar_top_left (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == BIG_PILLAR_TOP && ptl_p) draw_big_pillar_top_left (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == BIG_PILLAR_TOP && ptr_p) draw_big_pillar_top_left (bitmap, &ptr, em, vm);

  /* ARCH BOTTOM */
  if (con (&pbl)->fg == ARCH_BOTTOM) draw_arch_bottom_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == ARCH_BOTTOM && pbr_p) draw_arch_bottom_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == ARCH_BOTTOM && pm_p) draw_arch_bottom_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == ARCH_BOTTOM && ptl_p) draw_arch_bottom_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == ARCH_BOTTOM && ptr_p) draw_arch_bottom_fg (bitmap, &ptr, em, vm);

  /* WALL */
  if (con (&pbl)->fg == WALL) draw_wall_left_cache (bitmap, &pbl);
  if (con (&pbr)->fg == WALL && pbr_p) draw_wall_left_cache (bitmap, &pbr);
  if (con (&pm)->fg == WALL && pm_p) draw_wall_left_cache (bitmap, &pm);
  if (con (&ptl)->fg == WALL && ptl_p) draw_wall_left_cache (bitmap, &ptl);
  if (con (&ptr)->fg == WALL && ptr_p) draw_wall_left_cache (bitmap, &ptr);

  /* DOOR */
  if (con (&pbl)->fg == DOOR) draw_door_fg (bitmap, &pbl, f, em, vm);
  if (con (&pbr)->fg == DOOR && pbr_p) draw_door_fg (bitmap, &pbr, f, em, vm);
  if (con (&pm)->fg == DOOR && pm_p) draw_door_fg (bitmap, &pm, f, em, vm);
  if (con (&ptl)->fg == DOOR && ptl_p) draw_door_fg (bitmap, &ptl, f, em, vm);
  if (con (&ptr)->fg == DOOR && ptr_p) draw_door_fg (bitmap, &ptr, f, em, vm);

  /* CHOPPER */
  if (con (&pbl)->fg == CHOPPER) draw_chopper_fg (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == CHOPPER && pbr_p) draw_chopper_fg (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == CHOPPER && pm_p) draw_chopper_fg (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == CHOPPER && ptl_p) draw_chopper_fg (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == CHOPPER && ptr_p) draw_chopper_fg (bitmap, &ptr, em, vm);

  /* MIRROR */
  if (con (&pbl)->fg == MIRROR) draw_mirror_fg (bitmap, &pbl, f, em, vm);
  if (con (&pbr)->fg == MIRROR && pbr_p) draw_mirror_fg (bitmap, &pbr, f, em, vm);
  if (con (&pm)->fg == MIRROR && pm_p) draw_mirror_fg (bitmap, &pm, f, em, vm);
  if (con (&ptl)->fg == MIRROR && ptl_p) draw_mirror_fg (bitmap, &ptl, f, em, vm);
  if (con (&ptr)->fg == MIRROR && ptr_p) draw_mirror_fg (bitmap, &ptr, f, em, vm);

  /* ARCH TOP MID */
  if (con (&pbl)->fg == ARCH_TOP_MID) draw_arch_top_mid (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == ARCH_TOP_MID && pbr_p) draw_arch_top_mid (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == ARCH_TOP_MID && pm_p) draw_arch_top_mid (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == ARCH_TOP_MID && ptl_p) draw_arch_top_mid (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == ARCH_TOP_MID && ptr_p) draw_arch_top_mid (bitmap, &ptr, em, vm);

  /* ARCH TOP SMALL */
  if (con (&pbl)->fg == ARCH_TOP_SMALL) draw_arch_top_small (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == ARCH_TOP_SMALL && pbr_p) draw_arch_top_small (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == ARCH_TOP_SMALL && pm_p) draw_arch_top_small (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == ARCH_TOP_SMALL && ptl_p) draw_arch_top_small (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == ARCH_TOP_SMALL && ptr_p) draw_arch_top_small (bitmap, &ptr, em, vm);

  /* ARCH TOP LEFT */
  if (con (&pbl)->fg == ARCH_TOP_LEFT) draw_arch_top_left (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == ARCH_TOP_LEFT && pbr_p) draw_arch_top_left (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == ARCH_TOP_LEFT && pm_p) draw_arch_top_left (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == ARCH_TOP_LEFT && ptl_p) draw_arch_top_left (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == ARCH_TOP_LEFT && ptr_p) draw_arch_top_left (bitmap, &ptr, em, vm);

  /* ARCH TOP RIGHT */
  if (con (&pbl)->fg == ARCH_TOP_RIGHT) draw_arch_top_right (bitmap, &pbl, em, vm);
  if (con (&pbr)->fg == ARCH_TOP_RIGHT && pbr_p) draw_arch_top_right (bitmap, &pbr, em, vm);
  if (con (&pm)->fg == ARCH_TOP_RIGHT && pm_p) draw_arch_top_right (bitmap, &pm, em, vm);
  if (con (&ptl)->fg == ARCH_TOP_RIGHT && ptl_p) draw_arch_top_right (bitmap, &ptl, em, vm);
  if (con (&ptr)->fg == ARCH_TOP_RIGHT && ptr_p) draw_arch_top_right (bitmap, &ptr, em, vm);

  /* CARPET */
  if (con (&pbl)->fg == CARPET) draw_carpet_fg (bitmap, &pbl, f, em, vm);
  if (con (&pbr)->fg == CARPET && pbr_p) draw_carpet_fg (bitmap, &pbr, f, em, vm);
  if (con (&pm)->fg == CARPET && pm_p) draw_carpet_fg (bitmap, &pm, f, em, vm);
  if (con (&ptl)->fg == CARPET && ptl_p) draw_carpet_fg (bitmap, &ptl, f, em, vm);
  if (con (&ptr)->fg == CARPET && ptr_p) draw_carpet_fg (bitmap, &ptr, f, em, vm);

  /* TCARPET */
  if (con (&pbl)->fg == TCARPET) draw_carpet_fg (bitmap, &pbl, f, em, vm);
  if (con (&pbr)->fg == TCARPET && pbr_p) draw_carpet_fg (bitmap, &pbr, f, em, vm);
  if (con (&pm)->fg == TCARPET && pm_p) draw_carpet_fg (bitmap, &pm, f, em, vm);
  if (con (&ptl)->fg == TCARPET && ptl_p) draw_carpet_fg (bitmap, &ptl, f, em, vm);
  if (con (&ptr)->fg == TCARPET && ptr_p) draw_carpet_fg (bitmap, &ptr, f, em, vm);

  al_reset_clipping_rectangle ();
}

void
draw_room_anim_fg_sub (ALLEGRO_BITMAP *bitmap,
                       enum em em, enum vm vm, struct anim *a)
{
  struct coord nc;
  struct pos p, np, pbl, pbr, pm, ptl, ptr,
    ptl2, pbr2, ptr2;

  struct frame *f = &a->f;
  struct coord c; frame2room (f, room_view, &c);

  int w = al_get_bitmap_width (f->b);
  int h = al_get_bitmap_height (f->b);

  survey (_bl, posf, f, &nc, &pbl, &np);
  survey (_br, posf, f, &nc, &pbr, &np);
  survey (_m, posf, f, &nc, &pm, &np);
  survey (_tl, posf, f, &nc, &ptl, &np);
  survey (_tr, posf, f, &nc, &ptr, &np);

  survey (_br, pos, f, &nc, &pbr2, &np);
  survey (_tl, pos, f, &nc, &ptl2, &np);
  survey (_tr, pos, f, &nc, &ptr2, &np);

  set_target_bitmap (bitmap);
  al_set_clipping_rectangle (c.x, c.y, w, h);

  /* FALLING */
  if (is_anim_fall (f)) {
    draw_confg_base (bitmap, &pbr, em, vm);
    draw_confg_left (bitmap, &pbr, em, vm, true);

    if (! peq (&pbr, &ptr)) {
      draw_confg_base (bitmap, &ptr, em, vm);
      draw_confg_left (bitmap, &ptr, em, vm, true);
    }

    if (! peq (&pbr2, &ptr2)) {
      draw_confg_base (bitmap, &ptr2, em, vm);
      draw_confg_left (bitmap, &ptr2, em, vm, true);
      draw_confg_right (bitmap, &ptr2, em, vm, true);
    }
  }

  /* CLIMBING STAIRS */
  if (is_kid_stairs (f)) draw_level_door_fg (bitmap, &a->p, f, em, vm);

  /* VERTICAL JUMP */
  if (is_kid_vjump_touching_above (f)) {
    if (! is_strictly_traversable (&ptl))
      draw_confg_no_top (bitmap, &ptl, em, vm, true);

    if (! peq (&ptl2, &ptl) && ! is_strictly_traversable (&ptl2)
        && ! is_strictly_traversable (&ptl)) {
      draw_confg_base (bitmap, &ptl2, em, vm);
      draw_confg_right (bitmap, &ptl2, em, vm, true);
    }

    if (! peq (&ptr, &ptl) && ! peq (&ptr, &ptl2)
        && ! is_strictly_traversable (&ptr)) {
      draw_confg_base (bitmap, &ptr, em, vm);
      draw_confg_left (bitmap, &ptr, em, vm, true);
    }
  }

  /* HANGING */
  if (is_kid_hang (f) && f->dir == RIGHT) {
    get_hanged_pos (&a->hang_pos, f->dir, &p);
    draw_confg_base (bitmap, &p, em, vm);
    draw_confg_left (bitmap, &p, em, vm, true);
  }

  /* CLIMBING FLOOR */
  if (is_kid_climb (f) && f->dir == RIGHT) {
    get_hanged_pos (&a->hang_pos, f->dir, &p);
    draw_confg_base (bitmap, &p, em, vm);

    if (con (&p)->fg != BROKEN_FLOOR
        && (con (&p)->fg != OPENER_FLOOR
            || ! opener_floor_at_pos (&p)->broken)
        && (con (&p)->fg != CLOSER_FLOOR
            || ! closer_floor_at_pos (&p)->broken)) {
      if (f->b == kid_climb_00
          || f->b == kid_climb_01
          || f->b == kid_climb_02
          || f->b == kid_climb_08
          || f->b == kid_climb_09)
        draw_floor_corner_02 (bitmap, &p, em, vm);
      else if (f->b == kid_climb_05
               || f->b == kid_climb_06)
        draw_floor_corner_00 (bitmap, &p, em, vm);
      else if (f->b == kid_climb_03
               || f->b == kid_climb_07
               || f->b == kid_climb_04)
        draw_floor_corner_01 (bitmap, &p, em, vm);
    }
  }

  al_reset_clipping_rectangle ();

  draw_room_frame_fg (bitmap, em, vm, f);

  /* xframe */
  if (a->xf.b) {
    struct frame xf; xframe_frame (&a->f, &a->xf, &xf);
    draw_room_frame_fg (bitmap, em, vm, &xf);
  }

  /* splash */
  if (a->splash) {
    struct frame sf; splash_frame (&a->f, &sf);
    draw_room_frame_fg (bitmap, em, vm, &sf);
  }
}

void
draw_room_anim_fg (ALLEGRO_BITMAP *bitmap,
                   enum em em, enum vm vm, struct anim *a0)
{
  draw_room_anim_fg_sub (bitmap, em, vm, a0);

  int x = a0->f.c.x;
  int y = a0->f.c.y;
  int room = a0->f.c.room;
  int w = al_get_bitmap_width (a0->f.b);
  int h = al_get_bitmap_height (a0->f.b);

  struct anim a = *a0;

  if (room == roomd (room, LEFT)
      && x < 0) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, ABOVE)
      && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, LEFT) && x < 0
      && room == roomd (room, ABOVE) && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (room, ABOVE) && y < 3) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y += PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, LEFT)
      && x < 0
      && room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x += PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }

  if (room == roomd (room, RIGHT)
      && x + w - 1 >= PLACE_WIDTH * PLACES
      && room == roomd (room, BELOW)
      && y + h - 1 >= PLACE_HEIGHT * FLOORS) {
    a.f.c = a0->f.c;
    a.f.c.x -= PLACE_WIDTH * PLACES;
    a.f.c.y -= PLACE_HEIGHT * FLOORS;
    draw_room_anim_fg_sub (bitmap, em, vm, &a);
  }
}

void
draw_confg_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm, struct frame *f)
{
  switch (con (p)->fg) {
  case NO_FLOOR: return;
  case FLOOR: return;
  case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: return;
  case LOOSE_FLOOR: return;
  case SPIKES_FLOOR: draw_spikes_fg (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_fg (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_fg (bitmap, p, em, vm); break;
  case STUCK_FLOOR: return;
  case HIDDEN_FLOOR: return;
  case PILLAR: draw_pillar_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL: draw_wall_left_cache (bitmap, p); break;
  case DOOR: draw_door_fg (bitmap, p, f, em, vm); break;
  case LEVEL_DOOR: draw_level_door_fg (bitmap, p, f, em, vm); break;
  case CHOPPER: draw_chopper_fg (bitmap, p, em, vm); break;
  case ARCH_BOTTOM: draw_arch_bottom_fg (bitmap, p, em, vm);
    break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, p, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, p, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, p, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, p, em, vm); break;
  case CARPET: draw_carpet_fg (bitmap, p, f, em, vm); break;
  case TCARPET:
    draw_door_pole_base (bitmap, p, em, vm);
    draw_carpet_fg (bitmap, p, f, em, vm); break;
  case MIRROR: draw_mirror_fg (bitmap, p, f, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }
}

/* void */
/* draw_room_anim_fg (ALLEGRO_BITMAP *bitmap, */
/*                    enum em em, enum vm vm, struct anim *a) */
/* { */
/*   struct coord bl, br, m, tl, tr; */
/*   struct pos pbl, npbl, pbr, npbr, */
/*     pm, npm, ptl, nptl, ptr, nptr; */

/*   struct frame *f = &a->f; */

/*   survey (_bl, posf, f, &bl, &pbl, &npbl); */
/*   survey (_br, posf, f, &br, &pbr, &npbr); */
/*   survey (_m, posf, f, &m, &pm, &npm); */
/*   survey (_tl, posf, f, &tl, &ptl, &nptl); */
/*   survey (_tr, posf, f, &tr, &ptr, &nptr); */

/*   draw_room_fg (bitmap, &pbl, em, vm, f); */
/*   draw_room_fg (bitmap, &pbr, em, vm, f); */
/*   draw_room_fg (bitmap, &pm, em, vm, f); */
/*   draw_room_fg (bitmap, &ptl, em, vm, f); */
/*   draw_room_fg (bitmap, &ptr, em, vm, f); */

/*   /\* survey (_bl, pos, f, &bl, &pbl, &npbl); *\/ */
/*   /\* survey (_br, pos, f, &br, &pbr, &npbr); *\/ */
/*   /\* survey (_m, pos, f, &m, &pm, &npm); *\/ */
/*   /\* survey (_tl, pos, f, &tl, &ptl, &nptl); *\/ */
/*   /\* survey (_tr, pos, f, &tr, &ptr, &nptr); *\/ */

/*   /\* draw_room_fg (bitmap, &pbl, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &pbr, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &pm, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &ptl, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &ptr, em, vm, f); *\/ */

/*   if (! a->xf.b) return; */

/*   struct frame xf; xframe_frame (&a->f, &a->xf, &xf); */
/*   f = &xf; */

/*   survey (_bl, posf, f, &bl, &pbl, &npbl); */
/*   survey (_br, posf, f, &br, &pbr, &npbr); */
/*   survey (_m, posf, f, &m, &pm, &npm); */
/*   survey (_tl, posf, f, &tl, &ptl, &nptl); */
/*   survey (_tr, posf, f, &tr, &ptr, &nptr); */

/*   draw_room_fg (bitmap, &pbl, em, vm, f); */
/*   draw_room_fg (bitmap, &pbr, em, vm, f); */
/*   draw_room_fg (bitmap, &pm, em, vm, f); */
/*   draw_room_fg (bitmap, &ptl, em, vm, f); */
/*   draw_room_fg (bitmap, &ptr, em, vm, f); */

/*   /\* survey (_bl, pos, f, &bl, &pbl, &npbl); *\/ */
/*   /\* survey (_br, pos, f, &br, &pbr, &npbr); *\/ */
/*   /\* survey (_m, pos, f, &m, &pm, &npm); *\/ */
/*   /\* survey (_tl, pos, f, &tl, &ptl, &nptl); *\/ */
/*   /\* survey (_tr, pos, f, &tr, &ptr, &nptr); *\/ */

/*   /\* draw_room_fg (bitmap, &pbl, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &pbr, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &pm, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &ptl, em, vm, f); *\/ */
/*   /\* draw_room_fg (bitmap, &ptr, em, vm, f); *\/ */
/* } */

/* void */
/* draw_room_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, */
/*               enum em em, enum vm vm, struct frame *f) */
/* { */
/*   struct coord tl, bl, br, nc; */
/*   struct pos ptl, pbl, pm, ptf, ptr, pmt, pmbo, */
/*     fptl, fptr, fpmt, fpmbo, fptf, np; */

/*   survey (_tl, pos, f, &tl, &ptl, &np); */
/*   survey (_bl, pos, f, &bl, &pbl, &np); */
/*   survey (_br, pos, f, &br, &np, &np); */
/*   survey (_m, pos, f, &nc, &pm, &np); */
/*   survey (_tf, pos, f, &nc, &ptf, &np); */
/*   survey (_tr, pos, f, &nc, &ptr, &np); */
/*   survey (_mt, pos, f, &nc, &pmt, &np); */
/*   survey (_mbo, pos, f, &nc, &pmbo, &np); */

/*   survey (_tl, posf, f, &nc, &fptl, &np); */
/*   survey (_tr, posf, f, &nc, &fptr, &np); */
/*   survey (_mt, posf, f, &nc, &fpmt, &np); */
/*   survey (_mbo, posf, f, &nc, &fpmbo, &np); */
/*   survey (_tf, posf, f, &nc, &fptf, &np); */

/*   struct pos pr; prel (p, &pr, +0, +1); */
/*   struct pos pb; prel (p, &pb, +1, +0); */
/*   struct pos pa; prel (p, &pa, -1, +0); */

/*   /\* if NO_FLOOR, there is nothing to draw *\/ */
/*   if (con (p)->fg == NO_FLOOR) return; */

/*   /\* when falling at construct's left *\/ */
/*   if (br.y >= (p->floor + 1) * PLACE_HEIGHT - 6 */
/*       && br.x >= p->place * PLACE_WIDTH */
/*       && is_anim_fall (f)) { */
/*     draw_confg_base (bitmap, p, em, vm); */
/*     draw_confg_left (bitmap, p, em, vm, true); */

/*     if (con (p)->fg == TCARPET) */
/*       draw_confg_right (bitmap, p, em, vm, true); */
/*   } */
/*   /\* when climbing the construct *\/ */
/*   else if (peq (&ptf, p) */
/*            && is_kid_climb (f) */
/*            && f->dir == RIGHT) { */
/*     draw_confg_base (bitmap, p, em, vm); */

/*     if (is_carpet (p)) { */
/*       draw_door_pole (bitmap, p, em, vm); */
/*       draw_confg_right (bitmap, p, em, vm, true); */
/*     } */

/*     if (con (p)->fg == BROKEN_FLOOR) */
/*       draw_broken_floor_fg (bitmap, p, em, vm); */
/*     else if (con (p)->fg == OPENER_FLOOR */
/*              && opener_floor_at_pos (p)->broken) */
/*       draw_opener_floor_fg (bitmap, p, em, vm); */
/*     else if (con (p)->fg == CLOSER_FLOOR */
/*              && closer_floor_at_pos (p)->broken) */
/*       draw_closer_floor_fg (bitmap, p, em, vm); */
/*     else { */
/*       if (con (p)->fg == DOOR) */
/*         draw_door_fg (bitmap, p, f, em, vm); */

/*       if (f->b == kid_climb_02 */
/*           || f->b == kid_climb_08 */
/*           || f->b == kid_climb_09) */
/*         draw_floor_corner_02 (bitmap, p, em, vm); */
/*       else if (f->b == kid_climb_05 */
/*                || f->b == kid_climb_06) */
/*         draw_floor_corner_00 (bitmap, p, em, vm); */
/*       else if (f->b == kid_climb_03 */
/*                || f->b == kid_climb_07 */
/*                || f->b == kid_climb_04) */
/*         draw_floor_corner_01 (bitmap, p, em, vm); */

/*       if (con (p)->fg == PILLAR) */
/*         draw_pillar_fg (bitmap, p, em, vm); */
/*       else if (con (p)->fg == BIG_PILLAR_BOTTOM) */
/*         draw_big_pillar_bottom_fg (bitmap, p, em, vm); */
/*       else if (con (p)->fg == ARCH_BOTTOM) */
/*         draw_arch_bottom_fg (bitmap, p, em, vm); */
/*     } */
/*     /\* when below the construction *\/ */
/*   } else if (! (is_kid_hang_or_climb (f) && f->dir == LEFT) */
/*              && ! is_anim_fall (f) */
/*              && ! is_strictly_traversable (p) */
/*              && tl.y <= (p->floor + 1) * PLACE_HEIGHT + 3 */
/*              && bl.y >= (p->floor + 1) * PLACE_HEIGHT + 3 */
/*              && ! (is_kid_vjump (f) && ((struct anim *) f->id)->hang */
/*                    && f->dir == LEFT)) */
/*     draw_confg (bitmap, p, em, vm, true); */
/*   /\* when vjumping at the left of a non-hangable construct *\/ */
/*   else if (peq (&fptr, p) */
/*            && is_kid_vjump (f) */
/*            && ! is_hangable_con (p, RIGHT)) { */
/*     draw_confg_base (bitmap, p, em, vm); */
/*     draw_confg_left (bitmap, p, em, vm, true); */
/*   } */
/*   /\* other cases *\/ */
/*   else draw_confg_fg (bitmap, p, em, vm, f); */
/* } */

ALLEGRO_COLOR
green_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -12);
  g = add_char (g, +16);
  b = add_char (b, -29);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
gray_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, +13);
  g = add_char (g, -7);
  b = add_char (b, -52);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
yellow_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, +78);
  g = add_char (g, +25);
  b = add_char (b, -64);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
blue_hue_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -96);
  g = add_char (g, -80);
  b = add_char (b, +64);
  return al_map_rgb (r, g, b);
}

ALLEGRO_COLOR
selection_palette (ALLEGRO_COLOR c)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;

  signed char d;
  if (vm == CGA && ! hgc) d = -128;
  else if (vm == CGA && hgc && em == DUNGEON) d = -96;
  else if (vm == VGA && em == PALACE) d = +32;
  else d = +64;

  r = add_char (r, d);
  g = add_char (g, d);
  b = add_char (b, d);
  return al_map_rgb (r, g, b);
}

ALLEGRO_BITMAP *
apply_hue_palette (ALLEGRO_BITMAP *bitmap)
{
  switch (hue) {
  case HUE_NONE: default: return bitmap;
  case HUE_GREEN:
    return apply_palette (bitmap, green_hue_palette);
  case HUE_GRAY:
    return apply_palette (bitmap, gray_hue_palette);
  case HUE_YELLOW:
    return apply_palette (bitmap, yellow_hue_palette);
  case HUE_BLUE:
    return apply_palette (bitmap, blue_hue_palette);
  }
}

ALLEGRO_COLOR
apply_hue_color (ALLEGRO_COLOR c)
{
  switch (hue) {
  case HUE_NONE: default: return c;
  case HUE_GREEN: return green_hue_palette (c);
  case HUE_GRAY: return gray_hue_palette (c);
  case HUE_YELLOW: return yellow_hue_palette (c);
  case HUE_BLUE: return blue_hue_palette (c);
  }
}

void
draw_no_floor_selection (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (peq (p, &mouse_pos)) {
    struct rect r;
    new_rect (&r, p->room, p->place * PLACE_WIDTH + 25,
              p->floor * PLACE_HEIGHT - 13,
              PLACE_WIDTH, PLACE_HEIGHT);
    draw_filled_rect (bitmap, &r, NO_FLOOR_SELECTION_COLOR);
  }
}
