/*
  wall-depedv.c -- wall dungeon ega, palace ega, dungeon vga module;

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

/* wall randomization auxiliary numbers */
static int r0, r1, r2, r3;

/* wall randomization dividers */
static ALLEGRO_BITMAP *wall_divider_00, *wall_divider_01;

/* dungeon ega */
ALLEGRO_BITMAP *de_wall_left, *de_wall_center, *de_wall_right,
  *de_wall_single, *de_wall_left_base, *de_wall_center_base,
  *de_wall_right_base, *de_wall_single_base, *de_wall_random_block,
  *de_wall_wide_divider, *de_wall_narrow_divider, *de_wall_mark_top_left,
  *de_wall_mark_top_right, *de_wall_mark_bottom_left, *de_wall_mark_bottom_right;

/* palace ega */
ALLEGRO_BITMAP *pe_wall_left, *pe_wall_center, *pe_wall_right,
  *pe_wall_single, *pe_wall_left_base, *pe_wall_center_base,
  *pe_wall_right_base, *pe_wall_single_base, *pe_wall_random_block,
  *pe_wall_wide_divider, *pe_wall_narrow_divider;

/* dungeon vga */
ALLEGRO_BITMAP *dv_wall_left, *dv_wall_center, *dv_wall_right,
  *dv_wall_single, *dv_wall_left_base, *dv_wall_center_base,
  *dv_wall_right_base, *dv_wall_single_base, *dv_wall_random_block,
  *dv_wall_wide_divider, *dv_wall_narrow_divider, *dv_wall_mark_top_left,
  *dv_wall_mark_top_right, *dv_wall_mark_bottom_left, *dv_wall_mark_bottom_right;

static void draw_sws_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_sws_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_sww_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_sww_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_wws_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_wws_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_www_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_www_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
static void draw_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                enum em em, enum vm vm);
static void draw_base_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                     enum em em, enum vm vm);
static void draw_random_block (ALLEGRO_BITMAP *bitmap, struct pos *p,
                               enum em em, enum vm vm);
static void draw_divider_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
static void draw_divider_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm);
static void draw_left_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r,
                            enum vm vm);
static void draw_right_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r,
                             enum vm vm);

void
load_wall_depedv (void)
{
  /* dungeon ega */
  de_wall_left = load_bitmap (DE_WALL_LEFT);
  de_wall_center = load_bitmap (DE_WALL_CENTER);
  de_wall_right = load_bitmap (DE_WALL_RIGHT);
  de_wall_single = load_bitmap (DE_WALL_SINGLE);
  de_wall_left_base = load_bitmap (DE_WALL_LEFT_BASE);
  de_wall_center_base = load_bitmap (DE_WALL_CENTER_BASE);
  de_wall_right_base = load_bitmap (DE_WALL_RIGHT_BASE);
  de_wall_single_base = load_bitmap (DE_WALL_SINGLE_BASE);
  de_wall_random_block = load_bitmap (DE_WALL_RANDOM_BLOCK);
  de_wall_wide_divider = load_bitmap (DE_WALL_WIDE_DIVIDER);
  de_wall_narrow_divider = load_bitmap (DE_WALL_NARROW_DIVIDER);
  de_wall_mark_top_left = load_bitmap (DE_WALL_MARK_TOP_LEFT);
  de_wall_mark_top_right = load_bitmap (DE_WALL_MARK_TOP_RIGHT);
  de_wall_mark_bottom_left = load_bitmap (DE_WALL_MARK_BOTTOM_LEFT);
  de_wall_mark_bottom_right = load_bitmap (DE_WALL_MARK_BOTTOM_RIGHT);

  /* palace ega */
  pe_wall_left = load_bitmap (PE_WALL_LEFT);
  pe_wall_center = load_bitmap (PE_WALL_CENTER);
  pe_wall_right = load_bitmap (PE_WALL_RIGHT);
  pe_wall_single = load_bitmap (PE_WALL_SINGLE);
  pe_wall_left_base = load_bitmap (PE_WALL_LEFT_BASE);
  pe_wall_center_base = load_bitmap (PE_WALL_CENTER_BASE);
  pe_wall_right_base = load_bitmap (PE_WALL_RIGHT_BASE);
  pe_wall_single_base = load_bitmap (PE_WALL_SINGLE_BASE);
  pe_wall_random_block = load_bitmap (PE_WALL_RANDOM_BLOCK);
  pe_wall_wide_divider = load_bitmap (PE_WALL_WIDE_DIVIDER);
  pe_wall_narrow_divider = load_bitmap (PE_WALL_NARROW_DIVIDER);

  /* dungeon vga */
  dv_wall_left = load_bitmap (DV_WALL_LEFT);
  dv_wall_center = load_bitmap (DV_WALL_CENTER);
  dv_wall_right = load_bitmap (DV_WALL_RIGHT);
  dv_wall_single = load_bitmap (DV_WALL_SINGLE);
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
}

void
unload_wall_depedv (void)
{
  /* dungeon ega */
  destroy_bitmap (de_wall_left);
  destroy_bitmap (de_wall_center);
  destroy_bitmap (de_wall_right);
  destroy_bitmap (de_wall_single);
  destroy_bitmap (de_wall_left_base);
  destroy_bitmap (de_wall_center_base);
  destroy_bitmap (de_wall_right_base);
  destroy_bitmap (de_wall_single_base);
  destroy_bitmap (de_wall_random_block);
  destroy_bitmap (de_wall_wide_divider);
  destroy_bitmap (de_wall_narrow_divider);
  destroy_bitmap (de_wall_mark_top_left);
  destroy_bitmap (de_wall_mark_top_right);
  destroy_bitmap (de_wall_mark_bottom_left);
  destroy_bitmap (de_wall_mark_bottom_right);

  /* palace ega */
  destroy_bitmap (pe_wall_left);
  destroy_bitmap (pe_wall_center);
  destroy_bitmap (pe_wall_right);
  destroy_bitmap (pe_wall_single);
  destroy_bitmap (pe_wall_left_base);
  destroy_bitmap (pe_wall_center_base);
  destroy_bitmap (pe_wall_right_base);
  destroy_bitmap (pe_wall_single_base);
  destroy_bitmap (pe_wall_random_block);
  destroy_bitmap (pe_wall_wide_divider);
  destroy_bitmap (pe_wall_narrow_divider);

  /* dungeon vga */
  destroy_bitmap (dv_wall_left);
  destroy_bitmap (dv_wall_center);
  destroy_bitmap (dv_wall_right);
  destroy_bitmap (dv_wall_single);
  destroy_bitmap (dv_wall_left_base);
  destroy_bitmap (dv_wall_center_base);
  destroy_bitmap (dv_wall_right_base);
  destroy_bitmap (dv_wall_single_base);
  destroy_bitmap (dv_wall_random_block);
  destroy_bitmap (dv_wall_wide_divider);
  destroy_bitmap (dv_wall_narrow_divider);
  destroy_bitmap (dv_wall_mark_top_left);
  destroy_bitmap (dv_wall_mark_top_right);
  destroy_bitmap (dv_wall_mark_bottom_left);
  destroy_bitmap (dv_wall_mark_bottom_right);
}

void
draw_wall_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  draw_wall_base_depedv (bitmap, p, em, vm);
  draw_wall_left_depedv (bitmap, p, em, vm);
  draw_wall_right (bitmap, p, em, vm);
  draw_wall_top (bitmap, p, em, vm);
}

void
draw_wall_base_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm)
{
  switch (wall_correlation (p)) {
  case SWS: draw_sws_base (bitmap, p, em, vm); break;
  case SWW: draw_sww_base (bitmap, p, em, vm); break;
  case WWS: draw_wws_base (bitmap, p, em, vm); break;
  case WWW: draw_www_base (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);
  }
  draw_base_randomization (bitmap, p, em, vm);
}

void
draw_wall_left_depedv (ALLEGRO_BITMAP *bitmap, struct pos *p,
                       enum em em, enum vm vm)
{
  switch (wall_correlation (p)) {
  case SWS: draw_sws_left (bitmap, p, em, vm); break;
  case SWW: draw_sww_left (bitmap, p, em, vm); break;
  case WWS: draw_wws_left (bitmap, p, em, vm); break;
  case WWW: draw_www_left (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i, %i. %i)",
           __func__, p->room, p->floor, p->place);
  }
  draw_randomization (bitmap, p, em, vm);
}

void
draw_sws_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_single = NULL;

  if (vm == VGA) wall_single = apply_hue_palette (dv_wall_single);
  else if (em == DUNGEON) wall_single = de_wall_single;
  else wall_single = pe_wall_single;

  if (peq (p, &mouse_pos))
    wall_single = apply_palette (wall_single, selection_palette);

  struct coord c;
  draw_bitmapc (wall_single, bitmap, wall_coord (p, &c), 0);
}

void
draw_sws_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_single_base = NULL;

  if (vm == VGA) wall_single_base = apply_hue_palette (dv_wall_single_base);
  else if (em == DUNGEON) wall_single_base = de_wall_single_base;
  else wall_single_base = pe_wall_single_base;

  if (peq (p, &mouse_pos))
    wall_single_base = apply_palette (wall_single_base, selection_palette);

  struct coord c;
  draw_bitmapc (wall_single_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_sww_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_left = NULL;

  if (vm == VGA) wall_left = apply_hue_palette (dv_wall_left);
  else if (em == DUNGEON) wall_left = de_wall_left;
  else wall_left = pe_wall_left;

  if (peq (p, &mouse_pos))
    wall_left = apply_palette (wall_left, selection_palette);

  struct coord c;
  draw_bitmapc (wall_left, bitmap, wall_coord (p, &c), 0);
}

void
draw_sww_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_left_base = NULL;

  if (vm == VGA) wall_left_base = apply_hue_palette (dv_wall_left_base);
  else if (em == DUNGEON) wall_left_base = de_wall_left_base;
  else wall_left_base = pe_wall_left_base;

  if (peq (p, &mouse_pos))
    wall_left_base = apply_palette (wall_left_base, selection_palette);

  struct coord c;
  draw_bitmapc (wall_left_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_wws_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_right = NULL;

  if (vm == VGA) wall_right = apply_hue_palette (dv_wall_right);
  else if (em == DUNGEON) wall_right = de_wall_right;
  else wall_right = pe_wall_right;

  if (peq (p, &mouse_pos))
    wall_right = apply_palette (wall_right, selection_palette);

  struct coord c;
  draw_bitmapc (wall_right, bitmap, wall_coord (p, &c), 0);
}

void
draw_wws_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_right_base = NULL;

  if (vm == VGA) wall_right_base = apply_hue_palette (dv_wall_right_base);
  else if (em == DUNGEON) wall_right_base = de_wall_right_base;
  else wall_right_base = pe_wall_right_base;

  if (peq (p, &mouse_pos))
    wall_right_base = apply_palette (wall_right_base, selection_palette);

  struct coord c;
  draw_bitmapc (wall_right_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_www_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_center = NULL;

  if (vm == VGA) wall_center = apply_hue_palette (dv_wall_center);
  else if (em == DUNGEON) wall_center = de_wall_center;
  else  wall_center = pe_wall_center;

  if (peq (p, &mouse_pos))
    wall_center = apply_palette (wall_center, selection_palette);

  struct coord c;
  draw_bitmapc (wall_center, bitmap, wall_coord (p, &c), 0);
}

void
draw_www_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
               enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_center_base = NULL;

  if (vm == VGA) wall_center_base = apply_hue_palette (dv_wall_center_base);
  else if (em == DUNGEON) wall_center_base = de_wall_center_base;
  else wall_center_base = pe_wall_center_base;

  if (peq (p, &mouse_pos))
    wall_center_base = apply_palette (wall_center_base, selection_palette);

  struct coord c;
  draw_bitmapc (wall_center_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_narrow_divider = NULL,
    *wall_wide_divider = NULL;

  if (vm == VGA) {
    wall_narrow_divider = apply_hue_palette (dv_wall_narrow_divider);
    wall_wide_divider = apply_hue_palette (dv_wall_wide_divider);
  } else if (em == DUNGEON) {
    wall_narrow_divider = de_wall_narrow_divider;
    wall_wide_divider = de_wall_wide_divider;
  } else {
    wall_narrow_divider = pe_wall_narrow_divider;
    wall_wide_divider = pe_wall_wide_divider;
  }

  if (peq (p, &mouse_pos)) {
    wall_narrow_divider = apply_palette (wall_narrow_divider, selection_palette);
    wall_wide_divider = apply_palette (wall_wide_divider, selection_palette);
  }

  seedp (p);
  prandom (1);
  r0 = prandom(1);
  r1 = prandom(4);
  r2 = prandom(1);
  r3 = prandom(4);

  wall_divider_00 = r2 ? wall_narrow_divider : wall_wide_divider;
  wall_divider_01 = r0 ? wall_narrow_divider : wall_wide_divider;

  enum wall_correlation wc = wall_correlation (p);

  switch (wc) {
  case WWW:
    if (prandom (4) == 0) draw_random_block (bitmap, p, em, vm);
    draw_divider_01 (bitmap, p, em, vm);
    draw_divider_00 (bitmap, p, em, vm);
    if (em == DUNGEON) {
      if (prandom(4) == 0)
        draw_right_mark (bitmap, p, prandom (3), vm);
      if (prandom(4) == 0)
        draw_left_mark (bitmap, p, prandom (4), vm);
    }
    break;
  case SWS:
    if (em == DUNGEON && prandom (6) == 0)
      draw_left_mark (bitmap, p, prandom (1), vm);
    break;
  case SWW:
    if (prandom (4) == 0)
      draw_random_block (bitmap, p, em, vm);
    draw_divider_01 (bitmap, p, em, vm);
    if (em == DUNGEON) {
      if (prandom(4) == 0)
        draw_right_mark (bitmap, p, prandom (3), vm);
      if (prandom(4) == 0)
        draw_left_mark (bitmap, p, prandom (3), vm);
    }
    break;
  case WWS:
    draw_divider_01 (bitmap, p, em, vm);
    draw_divider_00 (bitmap, p, em, vm);
    if (em == DUNGEON) {
      if (prandom(4) == 0)
        draw_right_mark (bitmap, p, prandom (1) + 2, vm);
      if (prandom(4) == 0)
        draw_left_mark (bitmap, p, prandom (4), vm);
    }
    break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i)", __func__, wc);
  }

  unseedp ();
}

void
draw_base_randomization (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_narrow_divider = NULL,
    *wall_wide_divider = NULL;

  if (vm == VGA) {
    wall_narrow_divider = apply_hue_palette (dv_wall_narrow_divider);
    wall_wide_divider = apply_hue_palette (dv_wall_wide_divider);
  } else if (em == DUNGEON) {
    wall_narrow_divider = de_wall_narrow_divider;
    wall_wide_divider = de_wall_wide_divider;
  } else {
    wall_narrow_divider = pe_wall_narrow_divider;
    wall_wide_divider = pe_wall_wide_divider;
  }

  if (peq (p, &mouse_pos)) {
    wall_narrow_divider = apply_palette (wall_narrow_divider, selection_palette);
    wall_wide_divider = apply_palette (wall_wide_divider, selection_palette);
  }

  seedp (p);
  prandom (1);
  r0 = prandom(1);
  r1 = prandom(4);
  r2 = prandom(1);
  r3 = prandom(4);

  wall_divider_00 = r2 ? wall_narrow_divider : wall_wide_divider;
  wall_divider_01 = r0 ? wall_narrow_divider : wall_wide_divider;

  enum wall_correlation wc = wall_correlation (p);

  switch (wc) {
  case WWW: draw_divider_00 (bitmap, p, em, vm); break;
  case SWS: break;
  case SWW: break;
  case WWS: draw_divider_00 (bitmap, p, em, vm); break;
  default:
    error (-1, 0, "%s: unknown wall correlation (%i)", __func__, wc);
  }

  unseedp ();
}

void
draw_random_block (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *wall_random_block = NULL;

  if (vm == VGA) wall_random_block = apply_hue_palette (dv_wall_random_block);
  else if (em == DUNGEON) wall_random_block = de_wall_random_block;
  else wall_random_block = pe_wall_random_block;

  if (peq (p, &mouse_pos))
    wall_random_block = apply_palette (wall_random_block, selection_palette);

  struct coord c;
  draw_bitmapc (wall_random_block, bitmap, wall_random_block_coord (p, &c), 0);
}

void
draw_divider_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;
  draw_bitmapc (wall_divider_00, bitmap, wall_divider_00_coord (p, &c), 0);
}

void
draw_divider_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;
  draw_bitmapc (wall_divider_01, bitmap, wall_divider_01_coord (p, &c), 0);
}

void
draw_left_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r,
                enum vm vm)
{
  ALLEGRO_BITMAP *wall_mark_top_left = NULL,
    *wall_mark_bottom_left = NULL;

  if (vm == VGA) {
    wall_mark_top_left = apply_hue_palette (dv_wall_mark_top_left);
    wall_mark_bottom_left = apply_hue_palette (dv_wall_mark_bottom_left);
  } else {
    wall_mark_top_left = de_wall_mark_top_left;
    wall_mark_bottom_left = de_wall_mark_bottom_left;
  }

  if (peq (p, &mouse_pos)) {
    wall_mark_top_left = apply_palette (wall_mark_top_left, selection_palette);
    wall_mark_bottom_left = apply_palette (wall_mark_bottom_left, selection_palette);
  }

  ALLEGRO_BITMAP *wall_mark = wall_mark_top_left;
  const int floor_offset[5] = {58, 41, 37, 20, 16};
  int place_offset = 0;

  if (r % 2) wall_mark = wall_mark_bottom_left;
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
draw_right_mark (ALLEGRO_BITMAP *bitmap, struct pos *p, int r,
                 enum vm vm)
{
  ALLEGRO_BITMAP *wall_mark_top_right = NULL,
    *wall_mark_bottom_right = NULL;

  if (vm == VGA) {
    wall_mark_top_right = apply_hue_palette (dv_wall_mark_top_right);
    wall_mark_bottom_right = apply_hue_palette (dv_wall_mark_bottom_right);
  } else {
    wall_mark_top_right = de_wall_mark_top_right;
    wall_mark_bottom_right = de_wall_mark_bottom_right;
  }

  if (peq (p, &mouse_pos)) {
    wall_mark_top_right = apply_palette (wall_mark_top_right, selection_palette);
    wall_mark_bottom_right = apply_palette (wall_mark_bottom_right, selection_palette);
  }

  ALLEGRO_BITMAP *wall_mark;
  const int floor_offset[4] = {52, 42, 31, 21};

  if (r % 2) wall_mark = wall_mark_bottom_right;
  else wall_mark = wall_mark_top_right;

  struct coord c;
  c.room = p->room;
  c.x = 32 * p->place + 8 * ((r > 1) ? 1 : 0)
    + ((r < 2) ? 24 : r1 - 3);
  c.y = 63 * p->floor + 56 - floor_offset[r];
  draw_bitmapc (wall_mark, bitmap, &c, 0);
}

struct coord *
wall_divider_00_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + r3;
  c->y = PLACE_HEIGHT * p->floor + 45;
  c->room = p->room;
  return c;
}

struct coord *
wall_divider_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8 + r1;
  c->y = PLACE_HEIGHT * p->floor + 24;
  c->room = p->room;
  return c;
}

struct coord *
wall_random_block_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}
