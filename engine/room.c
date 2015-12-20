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

#include <stdio.h>
#include <error.h>
#include <allegro5/allegro.h>

#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "physics.h"
#include "level.h"
#include "kid/kid.h"
#include "floor.h"
#include "broken-floor.h"
#include "skeleton-floor.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"
#include "pillar.h"
#include "big-pillar.h"
#include "wall.h"
#include "door.h"
#include "level-door.h"
#include "chopper.h"
#include "bricks.h"
#include "torch.h"
#include "window.h"
#include "room.h"

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

  load_loose_floor_samples ();
  load_opener_floor_samples ();
  load_closer_floor_samples ();
  load_spikes_floor_samples ();
  load_door_samples ();
  load_level_door_samples ();
  load_chopper_samples ();
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

  /* sounds */
  unload_loose_floor_samples ();
  unload_opener_floor_samples ();
  unload_closer_floor_samples ();
  unload_spikes_floor_samples ();
  unload_door_samples ();
  unload_level_door_samples ();
  unload_chopper_samples ();
}

void
draw_filled_rect (ALLEGRO_BITMAP *to, struct rect *r,
                  ALLEGRO_COLOR color)
{
  struct coord nc = r->c;

  if (! cutscene) {
    rect2room (r, room_view, &nc);
    if (to == screen && nc.room != room_view) return;
  }

  draw_filled_rectangle (to, nc.x, nc.y, nc.x + r->w - 1,
                         nc.y + r->h - 1, color);
}

void
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              struct coord *c, int flags)
{
  struct coord nc = *c;

  if (! cutscene) {
    struct frame f;
    f.b = from;
    f.c = *c;
    frame2room (&f, room_view, &nc);
    if (to == screen && nc.room != room_view) return;
  }

  draw_bitmap (from, to, nc.x, nc.y, flags);
}

void
draw_bitmap_regionc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
                     float sx, float sy, float sw, float sh,
                     struct coord *c, int flags)
{
  struct coord nc = *c;

  if (! cutscene) {
    struct frame f;
    f.b = from;
    f.c = *c;
    frame2room (&f, room_view, &nc);
    if (to == screen && nc.room != room_view) return;
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
      draw_con (bitmap, &p, em, vm, false);
}

void
draw_con (ALLEGRO_BITMAP *bitmap, struct pos *p,
          enum em em, enum vm vm, bool redraw)
{
  draw_conbg (bitmap, p, em, vm);
  draw_confg (bitmap, p, em, vm, redraw);
}

void
draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  switch (con (p)->bg) {
  case NO_BG: break;
  case BRICKS_01: draw_bricks_01 (bitmap, p, em, vm); break;
  case BRICKS_02: draw_bricks_02 (bitmap, p, em, vm); break;
  case BRICKS_03: draw_bricks_03 (bitmap, p, em, vm); break;
  case BRICKS_04: draw_bricks_04 (bitmap, p, em, vm); break;
  case BRICKS_05: draw_bricks_05 (bitmap, p, em, vm); break;
  case TORCH: draw_torch (bitmap, p, em, vm); break;
  case WINDOW: draw_window (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown background (%i)",
           __func__, con (p)->bg);
  }
}

void
draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm, bool redraw)
{
  draw_confg_base (bitmap, p, em, vm);
  draw_confg_left (bitmap, p, em, vm, redraw);
  draw_confg_right (bitmap, p, em, vm, redraw);
}

void
draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_base (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_floor_base (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_base (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_base (bitmap, p, em, vm); break;
  case PILLAR: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM: draw_floor_base (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP: break;
  case WALL: draw_wall_base (bitmap, p, em, vm); break;
  case DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_floor_base (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_base (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }
}

void
draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm, bool redraw)
{
  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_left (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_left (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_left (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_left (bitmap, p, em, vm); break;
  case PILLAR: draw_pillar_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_left (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL: draw_wall_left (bitmap, p, em, vm); break;
  case DOOR: draw_door_left (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_floor_left (bitmap, p, em, vm); break;
  case CHOPPER: draw_chopper_left (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;
}

void
draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm, bool redraw)
{
  struct pos pr, pa, par;

  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_right (bitmap, p, em, vm); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: draw_skeleton_floor_right (bitmap, p, em, vm); break;
  case LOOSE_FLOOR: draw_loose_floor_right (bitmap, p, em, vm); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, p, em, vm); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, p, em, vm); break;
  case CLOSER_FLOOR: draw_closer_floor_right (bitmap, p, em, vm); break;
  case PILLAR: draw_pillar_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_right (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_right (bitmap, p, em, vm); break;
  case WALL: draw_wall_right (bitmap, p, em, vm); break;
  case DOOR: draw_door_right (bitmap, p, em, vm); break;
  case LEVEL_DOOR: draw_level_door_right (bitmap, p, em, vm); break;
  case CHOPPER: draw_floor_right (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;

  prel (p, &pr, +0, +1);
  prel (p, &pa, -1, +0);
  prel (p, &par, -1, +1);

  /* right */
  draw_confg_base (bitmap, &pr, em, vm);
  draw_confg_left (bitmap, &pr, em, vm, true);

  /* above */
  switch (con (p)->fg) {
  case PILLAR: case DOOR:
    draw_confg_right (bitmap, &pa, em, vm, true);
    break;
  default: break;
  }

  /* above right */
  switch (con (p)->fg) {
  case WALL: case DOOR:
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
  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: break;
  case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, p, em, vm); break;
  case SKELETON_FLOOR: break;
  case LOOSE_FLOOR: break;
  case SPIKES_FLOOR: draw_spikes_fg (bitmap, p, em, vm); break;
  case OPENER_FLOOR: break;
  case CLOSER_FLOOR: break;
  case PILLAR: draw_pillar_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_BOTTOM:
    draw_big_pillar_bottom_fg (bitmap, p, em, vm); break;
  case BIG_PILLAR_TOP:
    draw_big_pillar_top_left (bitmap, p, em, vm); break;
  case WALL: draw_wall_left (bitmap, p, em, vm); break;
  case DOOR: draw_door_fg (bitmap, p, f, em, vm); break;
  case LEVEL_DOOR: draw_level_door_fg (bitmap, p, f, em, vm); break;
  case CHOPPER: draw_chopper_fg (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
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
  struct coord tl, c;
  struct pos ptl, pm, ptf, ptr, pmt, pmbo,
    fptl, fptr, fpmt, fpmbo, np;

  survey (_tl, pos, f, &tl, &ptl, &np);
  survey (_m, pos, f, &c, &pm, &np);
  survey (_tf, pos, f, &c, &ptf, &np);
  survey (_tr, pos, f, &c, &ptr, &np);
  survey (_mt, pos, f, &c, &pmt, &np);
  survey (_mbo, pos, f, &c, &pmbo, &np);

  survey (_tl, posf, f, &c, &fptl, &np);
  survey (_tr, posf, f, &c, &fptr, &np);
  survey (_mt, posf, f, &c, &fpmt, &np);
  survey (_mbo, posf, f, &c, &fpmbo, &np);

  /* when falling at construct's left */
  if ((peq (p, prel (&ptl, &np, 0, +1))
       || peq (p, prel (&pm, &np, 0, +1)))
      && is_kid_fall (f)) {
    draw_confg_base (screen, p, em, vm);
    draw_confg_left (screen, p, em, vm, true);
  }
  /* when climbing the construct */
  else if (peq (&ptf, p)
           && is_kid_climb (f)
           && f->dir == RIGHT) {
    draw_confg_base (screen, p, em, vm);

    if (con (p)->fg == BROKEN_FLOOR)
      draw_broken_floor_fg (screen, p, em, vm);
    else {
      if (con (p)->fg == DOOR)
        draw_door_fg (screen, p, f, em, vm);

      if (f->b == kid_climb_03
          || f->b == kid_climb_09
          || f->b == kid_climb_10)
        draw_floor_corner_03 (screen, p, em, vm);
      else if (f->b == kid_climb_06
               || f->b == kid_climb_07)
        draw_floor_corner_01 (screen, p, em, vm);
      else if (f->b == kid_climb_04
               || f->b == kid_climb_08
               || f->b == kid_climb_05)
        draw_floor_corner_02 (screen, p, em, vm);

      if (con (p)->fg == PILLAR)
        draw_pillar_fg (screen, p, em, vm);
      else if (con (p)->fg == BIG_PILLAR_BOTTOM)
        draw_big_pillar_bottom_fg (screen, p, em, vm);
    }
    /* when below the construction */
  } else if ((peq (p, &fptl)
              || peq (p, &fptr)
              || peq (p, &fpmt)
              || peq (p, &ptl)
              || peq (p, &ptr)
              || peq (p, &pmt))
             && (peq (p, prel (&fpmbo, &np, -1, 0))
                 || peq (p, prel (&pmbo, &np, -1, 0)))
             && ! (is_kid_hang_or_climb (f) && f->dir == LEFT)
             && ! is_kid_fall (f)
             && floor_left_coord (p, &c)->y <= tl.y
             && ! is_strictly_traversable (p))
    draw_confg (screen, p, em, vm, true);
  /* other cases */
  draw_confg_fg (screen, p, em, vm, f);
}
