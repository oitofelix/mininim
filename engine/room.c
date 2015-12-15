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
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"
#include "pillar.h"
#include "wall.h"
#include "door.h"
#include "room.h"

ALLEGRO_BITMAP *bricks_01, *bricks_02, *bricks_03, *bricks_04,
  *window, *torch;

void
load_room (void)
{
  switch (level->type) {
  case DUNGEON:
    switch (video_mode) {
    case VGA:
      load_vdungeon_floor ();
      load_vdungeon_loose_floor ();
      load_vdungeon_opener_floor ();
      load_vdungeon_closer_floor ();
      load_vdungeon_spikes_floor ();
      load_vdungeon_wall ();
      load_vdungeon_pillar ();
      load_vdungeon_door ();
      bricks_01 = load_bitmap (VDUNGEON_BRICKS_01);
      bricks_02 = load_bitmap (VDUNGEON_BRICKS_02);
      bricks_03 = load_bitmap (VDUNGEON_BRICKS_03);
      bricks_04 = load_bitmap (VDUNGEON_BRICKS_04);
      torch = load_bitmap (VDUNGEON_TORCH);
      window = load_bitmap (VDUNGEON_WINDOW);
      break;
    default:
      error (-1, 0, "%s: unknown video mode (%i)", __func__, video_mode);
    }
    break;
  case PALACE:
    break;
  default:
    error (-1, 0, "%s: unknown level type (%i)", __func__, level->type);
  }

  load_loose_floor_samples ();
  load_opener_floor_samples ();
  load_closer_floor_samples ();
  load_spikes_floor_samples ();
  load_door_samples ();
}

void
unload_room (void)
{
  /* modules */
  unload_floor ();
  unload_loose_floor ();
  unload_opener_floor ();
  unload_closer_floor ();
  unload_spikes_floor ();
  unload_wall ();
  unload_pillar ();
  unload_door ();

  /* sounds */
  unload_loose_floor_samples ();
  unload_opener_floor_samples ();
  unload_closer_floor_samples ();
  unload_spikes_floor_samples ();
  unload_door_samples ();

  /* bitmaps */
  al_destroy_bitmap (bricks_01);
  al_destroy_bitmap (bricks_02);
  al_destroy_bitmap (bricks_03);
  al_destroy_bitmap (bricks_04);
  al_destroy_bitmap (torch);
  al_destroy_bitmap (window);
}

void
draw_bitmapc (ALLEGRO_BITMAP *from, ALLEGRO_BITMAP *to,
              struct coord *c, int flags)
{
  int w = al_get_bitmap_width (from);
  int h = al_get_bitmap_height (from);

  struct coord nc = *c;

  if (! cutscene) {
    nbitmap_coord (c, &nc, w, h);
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
    nbitmap_coord (c, &nc, sw, sh);
    if (to == screen
        && nc.room != room_view) return;
  }

  draw_bitmap_region (from, to, sx, sy, sw, sh, nc.x, nc.y, flags);
}

void
draw_room (ALLEGRO_BITMAP *bitmap, int room)
{
  struct pos p;
  p.room = room;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++)
      draw_con (bitmap, &p, false);
}

void
draw_con (ALLEGRO_BITMAP *bitmap, struct pos *p,
          bool redraw)
{
  draw_conbg (bitmap, p);
  draw_confg (bitmap, p, redraw);
}

void
draw_conbg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  enum confg fg = con (p)->fg;

  /* consistency: wall, pillars and doors can't have background */
  if (fg == WALL || fg == PILLAR || fg == DOOR) return;

  switch (con (p)->bg) {
  case NO_BG: break;
  case BRICKS_01: draw_bricks_01 (bitmap, p); break;
  case BRICKS_02: draw_bricks_02 (bitmap, p); break;
  case BRICKS_03: draw_bricks_03 (bitmap, p); break;
  case BRICKS_04: draw_bricks_04 (bitmap, p); break;
  case TORCH: draw_torch (bitmap, p); break;
  case WINDOW: draw_window (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown background (%i)",
           __func__, con (p)->bg);
  }
}

void
draw_confg (ALLEGRO_BITMAP *bitmap, struct pos *p,
            bool redraw)
{
  draw_confg_base (bitmap, p);
  draw_confg_left (bitmap, p, redraw);
  draw_confg_right (bitmap, p, redraw);
}

void
draw_confg_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_base (bitmap, p); break;
  case BROKEN_FLOOR: draw_floor_base (bitmap, p); break;
  case SKELETON_FLOOR: draw_floor_base (bitmap, p); break;
  case LOOSE_FLOOR: draw_loose_floor_base (bitmap, p); break;
  case SPIKES_FLOOR: draw_floor_base (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor_base (bitmap, p); break;
  case CLOSER_FLOOR: draw_closer_floor_base (bitmap, p); break;
  case PILLAR: draw_floor_base (bitmap, p); break;
  case WALL: draw_wall_base (bitmap, p); break;
  case DOOR: draw_floor_base (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }
}

void
draw_confg_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 bool redraw)
{
  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_left (bitmap, p); break;
  case BROKEN_FLOOR: draw_broken_floor_left (bitmap, p); break;
  case SKELETON_FLOOR: draw_skeleton_floor_left (bitmap, p); break;
  case LOOSE_FLOOR: draw_loose_floor_left (bitmap, p); break;
  case SPIKES_FLOOR: draw_spikes_floor_left (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor_left (bitmap, p); break;
  case CLOSER_FLOOR: draw_closer_floor_left (bitmap, p); break;
  case PILLAR: draw_pillar_left (bitmap, p); break;
  case WALL: draw_wall_left (bitmap, p); break;
  case DOOR: draw_door_left (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;
}

void
draw_confg_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                bool redraw)
{
  struct pos pr, pa, par;

  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: draw_floor_right (bitmap, p); break;
  case BROKEN_FLOOR: draw_broken_floor_right (bitmap, p); break;
  case SKELETON_FLOOR: draw_skeleton_floor_right (bitmap, p); break;
  case LOOSE_FLOOR: draw_loose_floor_right (bitmap, p); break;
  case SPIKES_FLOOR: draw_spikes_floor_right (bitmap, p); break;
  case OPENER_FLOOR: draw_opener_floor_right (bitmap, p); break;
  case CLOSER_FLOOR: draw_closer_floor_right (bitmap, p); break;
  case PILLAR: draw_pillar_right (bitmap, p); break;
  case WALL: draw_wall_right (bitmap, p); break;
  case DOOR: draw_door_right (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  if (! redraw) return;

  prel (p, &pr, +0, +1);
  prel (p, &pa, -1, +0);
  prel (p, &par, -1, +1);

  /* right */
  draw_confg_base (bitmap, &pr);
  draw_confg_left (bitmap, &pr, true);

  /* above */
  switch (con (p)->fg) {
  case PILLAR:
    draw_confg_right (bitmap, &pa, true);
    break;
  default: break;
  }

  /* above right */
  switch (con (p)->fg) {
  case WALL: case DOOR:
    draw_confg_base (bitmap, &par);
    draw_confg_left (bitmap, &par, true);
    draw_falling_loose_floor (bitmap, &par);
    break;
  default: break;
  }
}

void
draw_confg_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
               struct frame *f)
{
  struct pos pr, pa, par;

  switch (con (p)->fg) {
  case NO_FLOOR: break;
  case FLOOR: break;
  case BROKEN_FLOOR: draw_broken_floor_fg (bitmap, p); break;
  case SKELETON_FLOOR: break;
  case LOOSE_FLOOR: break;
  case SPIKES_FLOOR: draw_spikes_fg (bitmap, p); break;
  case OPENER_FLOOR: break;
  case CLOSER_FLOOR: break;
  case PILLAR: draw_pillar_fg (bitmap, p); break;
  case WALL: draw_wall_left (bitmap, p); break;
  case DOOR: draw_door_fg (bitmap, p, f); break;
  default:
    error (-1, 0, "%s: unknown foreground (%i)",
           __func__, con (p)->fg);
  }

  prel (p, &pr, +0, +1);
  prel (p, &pa, -1, +0);
  prel (p, &par, -1, +1);

  /* right */
  switch (con (p)->fg) {
  case DOOR:
    draw_confg_fg (bitmap, &pr, f);
    break;
  default: break;
  }

  /* above right */
  switch (con (p)->fg) {
  case DOOR:
    draw_confg_base (bitmap, &par);
    draw_confg_left (bitmap, &par, true);
    break;
  default: break;
  }
}

void
draw_room_anim_fg (ALLEGRO_BITMAP *bitmap, struct anim *a)
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

  draw_room_fg (bitmap, &pbl, f);
  draw_room_fg (bitmap, &pbr, f);
  draw_room_fg (bitmap, &pm, f);
  draw_room_fg (bitmap, &ptl, f);
  draw_room_fg (bitmap, &ptr, f);

  survey (_bl, pos, f, &bl, &pbl, &npbl);
  survey (_br, pos, f, &br, &pbr, &npbr);
  survey (_m, pos, f, &m, &pm, &npm);
  survey (_tl, pos, f, &tl, &ptl, &nptl);
  survey (_tr, pos, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, f);
  draw_room_fg (bitmap, &pbr, f);
  draw_room_fg (bitmap, &pm, f);
  draw_room_fg (bitmap, &ptl, f);
  draw_room_fg (bitmap, &ptr, f);

  if (! a->xf.b) return;

  struct frame xf; xframe_frame (&a->f, &a->xf, &xf);
  f = &xf;

  survey (_bl, posf, f, &bl, &pbl, &npbl);
  survey (_br, posf, f, &br, &pbr, &npbr);
  survey (_m, posf, f, &m, &pm, &npm);
  survey (_tl, posf, f, &tl, &ptl, &nptl);
  survey (_tr, posf, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, f);
  draw_room_fg (bitmap, &pbr, f);
  draw_room_fg (bitmap, &pm, f);
  draw_room_fg (bitmap, &ptl, f);
  draw_room_fg (bitmap, &ptr, f);

  survey (_bl, pos, f, &bl, &pbl, &npbl);
  survey (_br, pos, f, &br, &pbr, &npbr);
  survey (_m, pos, f, &m, &pm, &npm);
  survey (_tl, pos, f, &tl, &ptl, &nptl);
  survey (_tr, pos, f, &tr, &ptr, &nptr);

  draw_room_fg (bitmap, &pbl, f);
  draw_room_fg (bitmap, &pbr, f);
  draw_room_fg (bitmap, &pm, f);
  draw_room_fg (bitmap, &ptl, f);
  draw_room_fg (bitmap, &ptr, f);
}

void
draw_room_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
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
    draw_confg_base (screen, p);
    draw_confg_left (screen, p, true);
  }
  /* when climbing the construct */
  else if (peq (&ptf, p)
           && is_kid_climb (f)
           && f->dir == RIGHT) {
    draw_confg_base (screen, p);

    if (con (p)->fg == PILLAR)
      draw_pillar_fg (screen, p);
    else if (con (p)->fg == BROKEN_FLOOR)
      draw_broken_floor_fg (screen, p);
    else {
      if (con (p)->fg == DOOR)
        draw_door_fg (screen, p, f);

      if (f->b == kid_climb_03
          || f->b == kid_climb_09
          || f->b == kid_climb_10)
        draw_floor_corner_03 (screen, p);
      else if (f->b == kid_climb_06
               || f->b == kid_climb_07)
        draw_floor_corner_01 (screen, p);
      else if (f->b == kid_climb_04
               || f->b == kid_climb_08
               || f->b == kid_climb_05)
        draw_floor_corner_02 (screen, p);
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
             && floor_left_coord (p, &c)->y <= tl.y)
    draw_confg (screen, p, true);
  /* other cases */
  draw_confg_fg (screen, p, f);
}

void
draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (bricks_01, bitmap, bricks_coord_12 (p, &c) , 0);
}

void
draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (bricks_02, bitmap, bricks_coord_12 (p, &c) , 0);
}

void
draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (bricks_03, bitmap, bricks_coord_34 (p, &c) , 0);
}

void
draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (bricks_04, bitmap, bricks_coord_34 (p, &c) , 0);
}

struct coord *
bricks_coord_12 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 15;
  c->room = p->room;
  return c;
}

struct coord *
bricks_coord_34 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 4;
  c->room = p->room;
  return c;
}

void
draw_torch (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (torch, bitmap, torch_coord (p, &c), 0);
}

struct coord *
torch_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 22;
  c->room = p->room;
  return c;
}

void
draw_window (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (window, bitmap, window_coord (p, &c), 0);
}

struct coord *
window_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 12;
  c->room = p->room;
  return c;
}
