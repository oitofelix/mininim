/*
  wall.c -- wall module;

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
#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "level.h"
#include "kid/kid.h"
#include "room.h"
#include "wall.h"

/* dungeon vga */
ALLEGRO_BITMAP *dv_wall_left, *dv_wall_center, *dv_wall_right,
  *dv_wall_single, *dv_wall_face, *dv_wall_face_top,
  *dv_wall_left_base, *dv_wall_center_base, *dv_wall_right_base,
  *dv_wall_single_base, *dv_wall_random_block, *dv_wall_wide_divider,
  *dv_wall_narrow_divider, *dv_wall_mark_top_left, *dv_wall_mark_top_right,
  *dv_wall_mark_bottom_left, *dv_wall_mark_bottom_right;

/* palace vga */
ALLEGRO_BITMAP *pv_wall_face, *pv_wall_face_top,
  *pv_wall_mark_00, *pv_wall_mark_01, *pv_wall_mark_02,
  *pv_wall_mark_03, *pv_wall_mark_04, *pv_wall_mark_05, *pv_wall_mark_06,
  *pv_wall_mark_07, *pv_wall_mark_08, *pv_wall_mark_09, *pv_wall_mark_10,
  *pv_wall_mark_11, *pv_wall_mark_12, *pv_wall_mark_13, *pv_wall_mark_14;

void
load_wall (void)
{
  /* dungeon vga */
  dv_wall_left = load_bitmap (DV_WALL_LEFT);
  dv_wall_center = load_bitmap (DV_WALL_CENTER);
  dv_wall_right = load_bitmap (DV_WALL_RIGHT);
  dv_wall_single = load_bitmap (DV_WALL_SINGLE);
  dv_wall_face = load_bitmap (DV_WALL_FACE);
  dv_wall_face_top = load_bitmap (DV_WALL_FACE_TOP);
  dv_wall_left_base = load_bitmap (DV_WALL_LEFT_BASE);
  dv_wall_center_base = load_bitmap (DV_WALL_CENTER_BASE);
  dv_wall_right_base = load_bitmap (DV_WALL_RIGHT_BASE);
  dv_wall_single_base = load_bitmap (DV_WALL_SINGLE_BASE);
  dv_wall_random_block = load_bitmap (DV_WALL_RANDOM_BLOCK);
  dv_wall_wide_divider = load_bitmap (DV_WALL_WIDE_DIVIDER);
  dv_wall_narrow_divider = load_bitmap (DV_WALL_NARROW_DIVIDER);
  dv_wall_mark_top_left = load_bitmap (DV_WALL_MARK_TOP_LEFT);
  dv_wall_mark_top_right = load_bitmap (DV_WALL_MARK_TOP_RIGHT);
  dv_wall_mark_bottom_left = load_bitmap (DV_WALL_MARK_BOTTOM_LEFT);
  dv_wall_mark_bottom_right = load_bitmap (DV_WALL_MARK_BOTTOM_RIGHT);

  /* palace vga */
  pv_wall_face = load_bitmap (PV_WALL_FACE);
  pv_wall_face_top = load_bitmap (PV_WALL_FACE_TOP);
  pv_wall_mark_00 = load_bitmap (PV_WALL_MARK_00);
  pv_wall_mark_01 = load_bitmap (PV_WALL_MARK_01);
  pv_wall_mark_02 = load_bitmap (PV_WALL_MARK_02);
  pv_wall_mark_03 = load_bitmap (PV_WALL_MARK_03);
  pv_wall_mark_04 = load_bitmap (PV_WALL_MARK_04);
  pv_wall_mark_05 = load_bitmap (PV_WALL_MARK_05);
  pv_wall_mark_06 = load_bitmap (PV_WALL_MARK_06);
  pv_wall_mark_07 = load_bitmap (PV_WALL_MARK_07);
  pv_wall_mark_08 = load_bitmap (PV_WALL_MARK_08);
  pv_wall_mark_09 = load_bitmap (PV_WALL_MARK_09);
  pv_wall_mark_10 = load_bitmap (PV_WALL_MARK_10);
  pv_wall_mark_11 = load_bitmap (PV_WALL_MARK_11);
  pv_wall_mark_12 = load_bitmap (PV_WALL_MARK_12);
  pv_wall_mark_13 = load_bitmap (PV_WALL_MARK_13);
  pv_wall_mark_14 = load_bitmap (PV_WALL_MARK_14);
}

void
unload_wall (void)
{
  /* dungeon vga */
  al_destroy_bitmap (dv_wall_left);
  al_destroy_bitmap (dv_wall_center);
  al_destroy_bitmap (dv_wall_right);
  al_destroy_bitmap (dv_wall_single);
  al_destroy_bitmap (dv_wall_face);
  al_destroy_bitmap (dv_wall_face_top);
  al_destroy_bitmap (dv_wall_left_base);
  al_destroy_bitmap (dv_wall_center_base);
  al_destroy_bitmap (dv_wall_right_base);
  al_destroy_bitmap (dv_wall_single_base);
  al_destroy_bitmap (dv_wall_random_block);
  al_destroy_bitmap (dv_wall_wide_divider);
  al_destroy_bitmap (dv_wall_narrow_divider);
  al_destroy_bitmap (dv_wall_mark_top_left);
  al_destroy_bitmap (dv_wall_mark_top_right);
  al_destroy_bitmap (dv_wall_mark_bottom_left);
  al_destroy_bitmap (dv_wall_mark_bottom_right);

  /* palace vga */
  al_destroy_bitmap (pv_wall_face);
  al_destroy_bitmap (pv_wall_face_top);
  al_destroy_bitmap (pv_wall_mark_00);
  al_destroy_bitmap (pv_wall_mark_01);
  al_destroy_bitmap (pv_wall_mark_02);
  al_destroy_bitmap (pv_wall_mark_03);
  al_destroy_bitmap (pv_wall_mark_04);
  al_destroy_bitmap (pv_wall_mark_05);
  al_destroy_bitmap (pv_wall_mark_06);
  al_destroy_bitmap (pv_wall_mark_07);
  al_destroy_bitmap (pv_wall_mark_08);
  al_destroy_bitmap (pv_wall_mark_09);
  al_destroy_bitmap (pv_wall_mark_10);
  al_destroy_bitmap (pv_wall_mark_11);
  al_destroy_bitmap (pv_wall_mark_12);
  al_destroy_bitmap (pv_wall_mark_13);
  al_destroy_bitmap (pv_wall_mark_14);
}

void
draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p,
           enum em em, enum vm vm)
{
  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_dv_wall (bitmap, p);
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_pv_wall (bitmap, p);
    }
    break;
  }
}

void
draw_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
           enum em em, enum vm vm)
{
  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_dv_wall_base (bitmap, p);
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_pv_wall_base (bitmap, p);
    }
    break;
  }
}

void
draw_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
           enum em em, enum vm vm)
{
  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_dv_wall_left (bitmap, p);
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: draw_pv_wall_left (bitmap, p);
    }
    break;
  }
}

void
draw_wall_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm)
{
  switch (wall_correlation (p)) {
  case SWS: draw_wall_face (bitmap, p, em, vm); break;
  case SWW: break;
  case WWS: draw_wall_face (bitmap, p, em, vm); break;
  case WWW: break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);
  }
}

void
draw_wall_face (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f wall_face_top_coord = NULL;

  ALLEGRO_BITMAP *wall_face = NULL,
    *wall_face_top = NULL;

  switch (em) {
  case DUNGEON:
    wall_face_top_coord = d_wall_face_top_coord;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      wall_face = dv_wall_face;
      wall_face_top = dv_wall_face_top;
      break;
    }
    break;
  case PALACE:
    wall_face_top_coord = p_wall_face_top_coord;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      wall_face = pv_wall_face;
      wall_face_top = pv_wall_face_top;
      break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (wall_face, bitmap, wall_face_coord (p, &c), 0);
  draw_bitmapc (wall_face_top, bitmap, wall_face_top_coord (p, &c), 0);
}

void
draw_pv_wall (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_base (bitmap, p);
  draw_pv_wall_left (bitmap, p);
  draw_wall_right (bitmap, p, PALACE, VGA);
}

void
draw_pv_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_brick (bitmap, p, 3, 0);
  return;
}

void
draw_pv_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_brick (bitmap, p, 0, 0);
  draw_pv_wall_brick (bitmap, p, 1, 0);
  draw_pv_wall_brick (bitmap, p, 1, 1);
  draw_pv_wall_brick (bitmap, p, 2, 0);
  draw_pv_wall_brick (bitmap, p, 2, 1);
  return;
}

void
draw_pv_wall_brick (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    int row, int col)
{
  struct rect r;
  struct frame f0, f1;
  f0.flip = f1.flip = 0;

  ALLEGRO_COLOR c = compute_palace_wall_color (p, row, col);
  palace_wall_brick_rect (p, row, col, &r);
  draw_filled_rect (bitmap, &r, c);

  palace_wall_mark_frame (p, row, &f0);
  palace_wall_mark_frame (p, row + 1, &f1);
  draw_frame (bitmap, &f0);
  draw_frame (bitmap, &f1);
}

ALLEGRO_COLOR
palace_wall_color (int i)
{
  switch (i) {
  case 0: return PALACE_WALL_COLOR_00;
  case 1: return PALACE_WALL_COLOR_01;
  case 2: return PALACE_WALL_COLOR_02;
  case 3: return PALACE_WALL_COLOR_03;
  case 4: return PALACE_WALL_COLOR_04;
  case 5: return PALACE_WALL_COLOR_05;
  case 6: return PALACE_WALL_COLOR_06;
  case 7: return PALACE_WALL_COLOR_07;
  }
  return PALACE_WALL_COLOR_00;
}

ALLEGRO_COLOR
compute_palace_wall_color (struct pos *p, int row, int col)
{
  int r;
  struct pos np; npos (p, &np);
  r = prandom_seq (np.room, np.floor * 4 * 11
                   + row * 11
                   + np.place + col, 11, 3);
  return palace_wall_color ((row % 2 ? 0 : 4) + r);
}

struct rect *
palace_wall_brick_rect (struct pos *p, int row, int col,
                        struct rect *r)
{
  r->c.room = p->room;
  r->c.x = PLACE_WIDTH * p->place;
  r->c.y = PLACE_HEIGHT * p->floor + 3;

  switch (row) {
  case 0:
    r->w = PLACE_WIDTH;
    r->h = 20;
    break;
  case 1:
    r->c.y += 20;
    r->c.x += 16 * (col % 2);
    r->w = 16;
    r->h = 21;
    break;
  case 2:
    r->c.y += 41;
    r->c.x += 8 * (col % 2);
    r->w = (col % 2) ? 24 : 8;
    r->h = 19;
    break;
  case 3:
    r->c.y += 60;
    r->w = PLACE_WIDTH;
    r->h = 3;
    break;
  }

  return r;
}

ALLEGRO_BITMAP *
palace_wall_mark (int i)
{
  switch (i) {
  case 0: return pv_wall_mark_00;
  case 1: return pv_wall_mark_01;
  case 2: return pv_wall_mark_02;
  case 3: return pv_wall_mark_03;
  case 4: return pv_wall_mark_04;
  case 5: return pv_wall_mark_05;
  case 6: return pv_wall_mark_06;
  case 7: return pv_wall_mark_07;
  case 8: return pv_wall_mark_08;
  case 9: return pv_wall_mark_09;
  case 10: return pv_wall_mark_10;
  case 11: return pv_wall_mark_11;
  case 12: return pv_wall_mark_12;
  case 13: return pv_wall_mark_13;
  case 14: return pv_wall_mark_14;
  }
  return NULL;
}

struct frame *
palace_wall_mark_frame (struct pos *p, int i, struct frame *f)
{
  int r = prandom_seq_pos (p, i, 1, 2);
  f->c.room = p->room;

  switch (i) {
  case 0:
    f->b = palace_wall_mark (r);
    f->c.x = PLACE_WIDTH * (p->place + 1) - 8;
    f->c.y = PLACE_HEIGHT * p->floor + 3;
    break;
  case 1:
    f->b = palace_wall_mark (r + 3);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 17;
    break;
  case 2:
    f->b = palace_wall_mark (r + 6);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 38;
    break;
  case 3:
    f->b = palace_wall_mark (r + 9);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 58;
    break;
  case 4:
    f->b = palace_wall_mark (r + 12);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 63;
    break;
  }

  return f;
}

void
draw_dv_wall (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_dv_wall_base (bitmap, p);
  draw_dv_wall_left (bitmap, p);
  draw_wall_right (bitmap, p, DUNGEON, VGA);
}

void
draw_dv_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_dv_wall_sws_base (bitmap, p); break;
  case SWW: draw_dv_wall_sww_base (bitmap, p); break;
  case WWS: draw_dv_wall_wws_base (bitmap, p); break;
  case WWW: draw_dv_wall_www_base (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);
  }
}

void
draw_dv_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  switch (wall_correlation (p)) {
  case SWS: draw_dv_wall_sws_left (bitmap, p); break;
  case SWW: draw_dv_wall_sww_left (bitmap, p); break;
  case WWS: draw_dv_wall_wws_left (bitmap, p); break;
  case WWW: draw_dv_wall_www_left (bitmap, p); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);
  }
  draw_dv_wall_randomization (bitmap, p);
}

void
draw_dv_wall_sws_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_single, bitmap, wall_coord (p, &c), 0);
}

void
draw_dv_wall_sws_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_single_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_dv_wall_sww_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_left, bitmap, wall_coord (p, &c), 0);
}

void
draw_dv_wall_sww_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_left_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_dv_wall_wws_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_right, bitmap, wall_coord (p, &c), 0);
}

void
draw_dv_wall_wws_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_right_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_dv_wall_www_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_center, bitmap, wall_coord (p, &c), 0);
}

void
draw_dv_wall_www_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_center_base, bitmap, wall_base_coord (p, &c), 0);
}

struct coord *
wall_base_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * (p->floor + 1);
  c->room = p->room;
  return c;
}

struct coord *
wall_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
wall_face_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
d_wall_face_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 9;
  c->room = p->room;
  return c;
}

struct coord *
p_wall_face_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 10;
  c->room = p->room;
  return c;
}

enum wall_correlation
wall_correlation (struct pos *p)
{
  if (con (p)->fg != WALL)
    error (-1, 0, "%s: requested wall correlation on non-wall (%i, %i, %i)",
           __func__, p->room, p->floor, p->place);

  if (crel (p, 0, -1)->fg != WALL
      && crel (p, 0, +1)->fg != WALL) return SWS;
  else if (crel (p, 0, -1)->fg != WALL
           && crel (p, 0, +1)->fg == WALL) return SWW;
  else if (crel (p, 0, -1)->fg == WALL
           && crel (p, 0, +1)->fg != WALL) return WWS;
  else if (crel (p, 0, -1)->fg == WALL
           && crel (p, 0, +1)->fg == WALL) return WWW;
  else
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);

  return -1;
}

/* wall randomization auxiliary numbers */
static int r0, r1, r2, r3;

/* wall randomization dividers */
static ALLEGRO_BITMAP *wall_divider_00, *wall_divider_01;

void
draw_dv_wall_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (level->em != DUNGEON && level->vm == VGA) return;

  seedp (p);
  prandom (1);
  r0 = prandom(1);
  r1 = prandom(4);
  r2 = prandom(1);
  r3 = prandom(4);

  wall_divider_00 = r2 ? dv_wall_narrow_divider : dv_wall_wide_divider;
  wall_divider_01 = r0 ? dv_wall_narrow_divider : dv_wall_wide_divider;

  enum wall_correlation wc = wall_correlation (p);

  switch (wc) {
  case WWW:
    if (prandom (4) == 0) draw_dv_wall_random_block (bitmap, p);
    draw_dv_wall_divider_01 (bitmap, p);
    draw_dv_wall_divider_00 (bitmap, p);
    if (level->em == DUNGEON) {
      if (prandom(4) == 0)
        draw_dv_wall_right_mark (bitmap, p, prandom (3));
      if (prandom(4) == 0)
        draw_dv_wall_left_mark (bitmap, p, prandom (4));
    }
    break;
  case SWS:
    if (level->em == DUNGEON && prandom (6) == 0)
      draw_dv_wall_left_mark (bitmap, p, prandom (1));
    break;
  case SWW:
    if (prandom (4) == 0)
      draw_dv_wall_random_block (bitmap, p);
    draw_dv_wall_divider_01 (bitmap, p);
    if (level->em == DUNGEON) {
      if (prandom(4) == 0)
        draw_dv_wall_right_mark (bitmap, p, prandom (3));
      if (prandom(4) == 0)
        draw_dv_wall_left_mark (bitmap, p, prandom (3));
    }
    break;
  case WWS:
    draw_dv_wall_divider_01 (bitmap, p);
    draw_dv_wall_divider_00 (bitmap, p);
    if (level->em == DUNGEON) {
      if (prandom(4) == 0)
        draw_dv_wall_right_mark (bitmap, p, prandom (1) + 2);
      if (prandom(4) == 0)
        draw_dv_wall_left_mark (bitmap, p, prandom (4));
    }
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i)", __func__, wc);
  }

  unseedp ();
}

void
draw_dv_wall_left_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r)
{
  ALLEGRO_BITMAP *wall_mark = dv_wall_mark_top_left;
  const int floor_offset[5] = {58, 41, 37, 20, 16};
  int place_offset = 0;

  if (r % 2) wall_mark = dv_wall_mark_bottom_left;
  if (r > 3) place_offset = r3 - r2 + 6;
  else if (r > 1) place_offset = r1 - r0 + 6;

  struct coord c;
  c.room = p->room;
  c.x = 32 * p->place + place_offset +
    8 * (((r == 2) || (r == 3)) ? 1 : 0);
  c.y = 63 * p->floor + 61 - floor_offset[r];

  draw_bitmapc (wall_mark, bitmap, &c, 0);
}

void
draw_dv_wall_right_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r)
{
  ALLEGRO_BITMAP *wall_mark;
  const int floor_offset[4] = {52, 42, 31, 21};

  if (r % 2) wall_mark = dv_wall_mark_bottom_right;
  else wall_mark = dv_wall_mark_top_right;

  struct coord c;
  c.room = p->room;
  c.x = 32 * p->place + 8 * ((r > 1) ? 1 : 0)
    + ((r < 2) ? 24 : r1 - 3);
  c.y = 63 * p->floor + 56 - floor_offset[r];
  draw_bitmapc (wall_mark, bitmap, &c, 0);
}

void
draw_dv_wall_random_block (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (dv_wall_random_block, bitmap, wall_random_block_coord (p, &c), 0);
}

struct coord *
wall_random_block_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

void
draw_dv_wall_divider_00 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (wall_divider_00, bitmap, wall_divider_00_coord (p, &c), 0);
}

struct coord *
wall_divider_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + r3;
  c->y = PLACE_HEIGHT * p->floor + 45;
  c->room = p->room;
  return c;
}

void
draw_dv_wall_divider_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (wall_divider_01, bitmap, wall_divider_01_coord (p, &c), 0);
}

struct coord *
wall_divider_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8 + r1;
  c->y = PLACE_HEIGHT * p->floor + 24;
  c->room = p->room;
  return c;
}
