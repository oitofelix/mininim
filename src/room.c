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

struct rect
new_rect (int room, int x, int y, int w, int h)
{
  struct rect r;
  r.c.room = room;
  r.c.x = x;
  r.c.y = y;
  r.w = w;
  r.h = h;
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
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
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

  draw_bitmap (from, to, nc.x, nc.y, flags);
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
}

void
draw_room (ALLEGRO_BITMAP *bitmap, int room,
           enum em em, enum vm vm)
{
  struct pos p;
  p.room = room;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_conbg (bitmap, &p, em, vm);

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place <= PLACES; p.place++) {
      draw_confg (bitmap, &p, em, vm, false);
    }
}

void
draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  switch (con (p)->bg) {
  case NO_BG:
    if (em == PALACE) draw_bricks_02 (bitmap, p, em, vm); break;
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
}

void
draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm, bool redraw)
{
  if (con (p)->fg != WALL) {
    draw_confg_base (bitmap, p, em, vm);
    draw_confg_left (bitmap, p, em, vm, redraw);
  }
  draw_confg_right (bitmap, p, em, vm, redraw);
}

void
draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct pos pv; pos2room (p, room_view, &pv);
  if (pv.floor < -1 || pv.floor > FLOORS
      || pv.place < -1 || pv.place > PLACES) return;

  /* if (con (p)->fg != WALL) pv = *p; */
  pv = *p;

  switch (con (&pv)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case BROKEN_FLOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case SKELETON_FLOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_base (bitmap, &pv, em, vm); break;
  case SPIKES_FLOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_base (bitmap, &pv, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_base (bitmap, &pv, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_base (bitmap, &pv, em, vm); break;
  case HIDDEN_FLOOR:
    if (peq (p, &mouse_pos)) draw_floor_base (bitmap, &pv, em, vm); break;
  case PILLAR: draw_floor_base (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_BOTTOM: draw_floor_base (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_TOP: break;
  case WALL: draw_wall_base_cache (bitmap, &pv); break;
  case DOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case LEVEL_DOOR: draw_floor_base (bitmap, &pv, em, vm); break;
  case CHOPPER: draw_floor_base (bitmap, &pv, em, vm); break;
  case ARCH_BOTTOM: draw_floor_base (bitmap, &pv, em, vm); break;
  case ARCH_TOP_MID: break;
  case ARCH_TOP_SMALL: break;
  case ARCH_TOP_LEFT: break;
  case ARCH_TOP_RIGHT: break;
  case CARPET: draw_floor_base (bitmap, &pv, em, vm); break;
  case TCARPET: draw_door_pole_base (bitmap, &pv, em, vm); break;
  case MIRROR: draw_floor_base (bitmap, &pv, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (&pv)->fg);
  }
}

void
draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm, bool redraw)
{
  struct pos pv; pos2room (p, room_view, &pv);
  if (pv.floor < -1 || pv.floor > FLOORS
      || pv.place < -1 || pv.place > PLACES) return;

  /* if (con (p)->fg != WALL) pv = *p; */
  pv = *p;

  struct pos pl; prel (&pv, &pl, +0, -1);

  switch (con (&pv)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_left (bitmap, &pv, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, &pv, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_left (bitmap, &pv, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_left (bitmap, &pv, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, &pv, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, &pv, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_left (bitmap, &pv, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_left (bitmap, &pv, em, vm); break;
  case HIDDEN_FLOOR:
    if (peq (p, &mouse_pos)) draw_floor_left (bitmap, &pv, em, vm); break;
  case PILLAR: draw_pillar_left (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_left (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, &pv, em, vm); break;
  case WALL: draw_wall_left_cache (bitmap, &pv); break;
  case DOOR: draw_door_left (bitmap, &pv, em, vm); break;
  case LEVEL_DOOR: draw_level_door_left (bitmap, &pv, em, vm); break;
  case CHOPPER: draw_chopper_left (bitmap, &pv, em, vm); break;
  case ARCH_BOTTOM:
    draw_floor_left (bitmap, &pv, em, vm);
    draw_arch_bottom (bitmap, &pv, em, vm); break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, &pv, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, &pv, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, &pv, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, &pv, em, vm); break;
  case CARPET:
    draw_floor_left (bitmap, &pv, em, vm);
    draw_door_pole (bitmap, &pv, em, vm);
    break;
  case TCARPET:
    if (con (p)->ext.design == ARCH_CARPET_RIGHT_00
        || con (p)->ext.design == ARCH_CARPET_RIGHT_01)
      draw_arch_top_right_end (bitmap, &pv, em, vm);
    else {
      draw_door_pole (bitmap, &pv, em, vm);
      draw_door_pole_base (bitmap, &pv, em, vm);
    }
    break;
  case MIRROR: draw_floor_left (bitmap, &pv, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (&pv)->fg);
  }

  if (! redraw) return;

  if (con (&pv)->fg == ARCH_BOTTOM) {
    struct pos pa; prel (p, &pa, -1, +0);
    draw_confg_base (bitmap, &pa, em, vm);
  }

  if (con (&pv)->fg == MIRROR)
    draw_mirror (bitmap, &pv, em, vm);
}

void
draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm, bool redraw)
{
  struct pos pv; pos2room (p, room_view, &pv);
  if (pv.floor < -1 || pv.floor > FLOORS
      || pv.place < -1 || pv.place >= PLACES) return;

  pv = *p;

  struct pos pr, pa, par;

  switch (con (&pv)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_right (bitmap, &pv, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, &pv, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_right (bitmap, &pv, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_right (bitmap, &pv, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, &pv, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, &pv, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_right (bitmap, &pv, em, vm); break;
  case STUCK_FLOOR: draw_pressed_closer_floor_right (bitmap, &pv, em, vm); break;
  case HIDDEN_FLOOR:
    if (peq (p, &mouse_pos)) draw_floor_right (bitmap, &pv, em, vm); break;
  case PILLAR: draw_pillar_right (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_right (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_right (bitmap, &pv, em, vm); break;
  case WALL: draw_wall_right (bitmap, &pv, em, vm); break;
  case DOOR: draw_door_right (bitmap, &pv, em, vm); break;
  case LEVEL_DOOR: draw_level_door_right (bitmap, &pv, em, vm); break;
  case CHOPPER: draw_floor_right (bitmap, &pv, em, vm); break;
  case ARCH_BOTTOM: draw_floor_right (bitmap, &pv, em, vm); break;
  case ARCH_TOP_MID: break;
  case ARCH_TOP_SMALL: break;
  case ARCH_TOP_LEFT: break;
  case ARCH_TOP_RIGHT: break;
  case CARPET: draw_carpet_right (bitmap, &pv, em, vm); break;
  case TCARPET: draw_carpet_right (bitmap, &pv, em, vm); break;
  case MIRROR: draw_floor_right (bitmap, &pv, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (&pv)->fg);
  }

  if (! redraw) return;

  prel (&pv, &pr, +0, +1);
  prel (&pv, &pa, -1, +0);
  prel (&pv, &par, -1, +1);

  /* right */
  draw_confg_base (bitmap, &pr, em, vm);
  draw_confg_left (bitmap, &pr, em, vm, true);

  /* above */
  switch (con (&pv)->fg) {
  case PILLAR: case DOOR:
    draw_confg_base (bitmap, &pa, em, vm);
    draw_confg_right (bitmap, &pa, em, vm, true);
    break;
  default: break;
  }

  /* above right */
  switch (con (&pv)->fg) {
  case WALL: case DOOR: case PILLAR:
  case CARPET: case TCARPET:
    draw_confg_base (bitmap, &par, em, vm);
    draw_confg_left (bitmap, &par, em, vm, true);
    draw_falling_loose_floor (bitmap, &par, em, vm);
    break;
  default: break;
  }
}

void
draw_confg_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm, struct frame *f)
{
  struct pos pv; pos2room (p, room_view, &pv);
  if (pv.floor < -1 || pv.floor > FLOORS
      || pv.place < -1 || pv.place > PLACES) return;

  /* if (con (p)->fg != WALL) pv = *p; */
  pv = *p;

  switch (con (&pv)->fg) {
  case NO_FLOOR: break;
  case FLOOR: break;
  case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, &pv, em, vm); break;
  case SKELETON_FLOOR: break;
  case LOOSE_FLOOR: break;
  case SPIKES_FLOOR: draw_spikes_fg (bitmap, &pv, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_fg (bitmap, &pv, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_fg (bitmap, &pv, em, vm); break;
  case STUCK_FLOOR: break;
  case HIDDEN_FLOOR: break;
  case PILLAR: draw_pillar_fg (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_fg (bitmap, &pv, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, &pv, em, vm); break;
  case WALL:
    draw_wall_left_cache (bitmap, &pv); break;
  case DOOR: draw_door_fg (bitmap, &pv, f, em, vm); break;
  case LEVEL_DOOR: draw_level_door_fg (bitmap, &pv, f, em, vm); break;
  case CHOPPER: draw_chopper_fg (bitmap, &pv, em, vm); break;
  case ARCH_BOTTOM: draw_arch_bottom_fg (bitmap, &pv, em, vm);
    break;
  case ARCH_TOP_MID: draw_arch_top_mid (bitmap, &pv, em, vm); break;
  case ARCH_TOP_SMALL: draw_arch_top_small (bitmap, &pv, em, vm); break;
  case ARCH_TOP_LEFT: draw_arch_top_left (bitmap, &pv, em, vm); break;
  case ARCH_TOP_RIGHT: draw_arch_top_right (bitmap, &pv, em, vm); break;
  case CARPET: draw_carpet_fg (bitmap, &pv, f, em, vm); break;
  case TCARPET:
    draw_door_pole_base (bitmap, p, em, vm);
    draw_carpet_fg (bitmap, &pv, f, em, vm); break;
  case MIRROR: draw_mirror_fg (bitmap, &pv, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (&pv)->fg);
  }
}

void
draw_room_anim_fg (ALLEGRO_BITMAP *bitmap,
                   enum em em, enum vm vm, struct anim *a)
{
  struct coord bl, br, m, tl, tr;
  struct pos pbl, npbl, pbr, npbr,
    pm, npm, ptl, nptl, ptr, nptr;

  struct frame *f = &a->f;

  survey (_bl, posf, f, &bl, &pbl, &npbl);
  survey (_br, posf, f, &br, &pbr, &npbr);
  survey (_m, posf, f, &m, &pm, &npm);
  survey (_tl, posf, f, &tl, &ptl, &nptl);
  survey (_tr, posf, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, em, vm, f);
  draw_room_fg (bitmap, &pbr, em, vm, f);
  draw_room_fg (bitmap, &pm, em, vm, f);
  draw_room_fg (bitmap, &ptl, em, vm, f);
  draw_room_fg (bitmap, &ptr, em, vm, f);

  survey (_bl, pos, f, &bl, &pbl, &npbl);
  survey (_br, pos, f, &br, &pbr, &npbr);
  survey (_m, pos, f, &m, &pm, &npm);
  survey (_tl, pos, f, &tl, &ptl, &nptl);
  survey (_tr, pos, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, em, vm, f);
  draw_room_fg (bitmap, &pbr, em, vm, f);
  draw_room_fg (bitmap, &pm, em, vm, f);
  draw_room_fg (bitmap, &ptl, em, vm, f);
  draw_room_fg (bitmap, &ptr, em, vm, f);

  if (! a->xf.b) return;

  struct frame xf; xframe_frame (&a->f, &a->xf, &xf);
  f = &xf;

  survey (_bl, posf, f, &bl, &pbl, &npbl);
  survey (_br, posf, f, &br, &pbr, &npbr);
  survey (_m, posf, f, &m, &pm, &npm);
  survey (_tl, posf, f, &tl, &ptl, &nptl);
  survey (_tr, posf, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, em, vm, f);
  draw_room_fg (bitmap, &pbr, em, vm, f);
  draw_room_fg (bitmap, &pm, em, vm, f);
  draw_room_fg (bitmap, &ptl, em, vm, f);
  draw_room_fg (bitmap, &ptr, em, vm, f);

  survey (_bl, pos, f, &bl, &pbl, &npbl);
  survey (_br, pos, f, &br, &pbr, &npbr);
  survey (_m, pos, f, &m, &pm, &npm);
  survey (_tl, pos, f, &tl, &ptl, &nptl);
  survey (_tr, pos, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, em, vm, f);
  draw_room_fg (bitmap, &pbr, em, vm, f);
  draw_room_fg (bitmap, &pm, em, vm, f);
  draw_room_fg (bitmap, &ptl, em, vm, f);
  draw_room_fg (bitmap, &ptr, em, vm, f);
}

void
draw_room_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
              enum em em, enum vm vm, struct frame *f)
{
  struct coord tl, bl, br, nc;
  struct pos ptl, pbl, pm, ptf, ptr, pmt, pmbo,
    fptl, fptr, fpmt, fpmbo, fptf, np;

  survey (_tl, pos, f, &tl, &ptl, &np);
  survey (_bl, pos, f, &bl, &pbl, &np);
  survey (_br, pos, f, &br, &np, &np);
  survey (_m, pos, f, &nc, &pm, &np);
  survey (_tf, pos, f, &nc, &ptf, &np);
  survey (_tr, pos, f, &nc, &ptr, &np);
  survey (_mt, pos, f, &nc, &pmt, &np);
  survey (_mbo, pos, f, &nc, &pmbo, &np);

  survey (_tl, posf, f, &nc, &fptl, &np);
  survey (_tr, posf, f, &nc, &fptr, &np);
  survey (_mt, posf, f, &nc, &fpmt, &np);
  survey (_mbo, posf, f, &nc, &fpmbo, &np);
  survey (_tf, posf, f, &nc, &fptf, &np);

  struct pos pr; prel (p, &pr, +0, +1);
  struct pos pb; prel (p, &pb, +1, +0);
  struct pos pa; prel (p, &pa, -1, +0);

  struct pos pv; pos2room (p, room_view, &pv);
  pv = *p;

  /* if NO_FLOOR, there is nothing to draw */
  if (con (&pv)->fg == NO_FLOOR) return;

  /* when falling at construct's left */
  if (br.y >= (pv.floor + 1) * PLACE_HEIGHT - 6
      && br.x >= pv.place * PLACE_WIDTH
      && is_anim_fall (f)) {
    draw_confg_base (bitmap, &pv, em, vm);
    draw_confg_left (bitmap, &pv, em, vm, true);

    if (con (&pv)->fg == TCARPET)
      draw_confg_right (bitmap, &pv, em, vm, true);
  }
  /* when climbing the construct */
  else if (peq (&ptf, &pv)
           && is_kid_climb (f)
           && f->dir == RIGHT) {
    draw_confg_base (bitmap, &pv, em, vm);

    if (is_carpet (&pv)) {
      draw_door_pole (bitmap, &pv, em, vm);
      draw_confg_right (bitmap, &pv, em, vm, true);
    }

    if (con (&pv)->fg == BROKEN_FLOOR)
      draw_broken_floor_fg (bitmap, &pv, em, vm);
    else if (con (&pv)->fg == OPENER_FLOOR
             && opener_floor_at_pos (&pv)->broken)
      draw_opener_floor_fg (bitmap, &pv, em, vm);
    else if (con (&pv)->fg == CLOSER_FLOOR
             && closer_floor_at_pos (&pv)->broken)
      draw_closer_floor_fg (bitmap, &pv, em, vm);
    else {
      if (con (&pv)->fg == DOOR)
        draw_door_fg (bitmap, &pv, f, em, vm);

      if (f->b == kid_climb_02
          || f->b == kid_climb_08
          || f->b == kid_climb_09)
        draw_floor_corner_02 (bitmap, &pv, em, vm);
      else if (f->b == kid_climb_05
               || f->b == kid_climb_06)
        draw_floor_corner_00 (bitmap, &pv, em, vm);
      else if (f->b == kid_climb_03
               || f->b == kid_climb_07
               || f->b == kid_climb_04)
        draw_floor_corner_01 (bitmap, &pv, em, vm);

      if (con (&pv)->fg == PILLAR)
        draw_pillar_fg (bitmap, &pv, em, vm);
      else if (con (&pv)->fg == BIG_PILLAR_BOTTOM)
        draw_big_pillar_bottom_fg (bitmap, &pv, em, vm);
      else if (con (&pv)->fg == ARCH_BOTTOM)
        draw_arch_bottom_fg (bitmap, &pv, em, vm);
    }
    /* when below the construction */
  } else if (! (is_kid_hang_or_climb (f) && f->dir == LEFT)
             && ! is_anim_fall (f)
             && ! is_strictly_traversable (&pv)
             && tl.y <= (pv.floor + 1) * PLACE_HEIGHT + 3
             && bl.y >= (pv.floor + 1) * PLACE_HEIGHT + 3
             && ! (is_kid_vjump (f) && ((struct anim *) f->id)->hang
                   && f->dir == LEFT))
    draw_confg (bitmap, &pv, em, vm, true);
  /* when vjumping at the left of a non-hangable construct */
  else if (peq (&fptr, &pv)
           && is_kid_vjump (f)
           && ! is_hangable_con (&pv, RIGHT)) {
    draw_confg_base (bitmap, &pv, em, vm);
    draw_confg_left (bitmap, &pv, em, vm, true);
  }
  /* other cases */
  else draw_confg_fg (bitmap, &pv, em, vm, f);
}

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
  enum conbg bg = con (p)->bg;
  if (peq (p, &mouse_pos)
      && con (p)->fg == NO_FLOOR
      && (bg == NO_BRICKS
          || (em == DUNGEON && bg == NO_BG)
          || (em == PALACE && bg == BRICKS_00))) {
    struct rect r = new_rect (p->room, p->place * PLACE_WIDTH + 25,
                              p->floor * PLACE_HEIGHT + 3,
                              PLACE_WIDTH, PLACE_HEIGHT - 16);
    draw_filled_rect (bitmap, &r, NO_FLOOR_SELECTION_COLOR);
  }
}
