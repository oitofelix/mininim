/*
  bricks.c -- bricks module;

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

/* dungeon vga */
ALLEGRO_BITMAP *dv_bricks_00, *dv_bricks_01,
  *dv_bricks_02, *dv_bricks_03;

/* palace vga */
ALLEGRO_BITMAP *pv_bricks_00, *pv_bricks_01;

static struct coord *d_bricks_coord_01 (struct pos *p, struct coord *c);
static struct coord *p_bricks_coord_01 (struct pos *p, struct coord *c);
static struct coord *d_bricks_coord_02 (struct pos *p, struct coord *c);
static struct coord *p_bricks_coord_02 (struct pos *p, struct coord *c);
static struct coord *d_bricks_coord_03 (struct pos *p, struct coord *c);
static struct coord *d_bricks_coord_04 (struct pos *p, struct coord *c);

static ALLEGRO_COLOR dv_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR de_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR dc_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR pv_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR pe_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR pc_palette (ALLEGRO_COLOR c);

void
load_bricks (void)
{
  /* dungeon vga */
  dv_bricks_00 = load_bitmap (DV_BRICKS_00);
  dv_bricks_01 = load_bitmap (DV_BRICKS_01);
  dv_bricks_02 = load_bitmap (DV_BRICKS_02);
  dv_bricks_03 = load_bitmap (DV_BRICKS_03);

  /* palace vga */
  pv_bricks_00 = load_bitmap (PV_BRICKS_00);
  pv_bricks_01 = load_bitmap (PV_BRICKS_01);
}

void
unload_bricks (void)
{
  /* dungeon vga */
  destroy_bitmap (dv_bricks_00);
  destroy_bitmap (dv_bricks_01);
  destroy_bitmap (dv_bricks_02);
  destroy_bitmap (dv_bricks_03);

  /* palace vga */
  destroy_bitmap (pv_bricks_00);
  destroy_bitmap (pv_bricks_01);
}

palette
get_palette (enum em em, enum vm vm)
{
  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: return dc_palette;
    case EGA: return de_palette;
    case VGA: return dv_palette;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: return pc_palette;
    case EGA: return pe_palette;
    case VGA: return pv_palette;
    }
    break;
  }
  return NULL;
}

void
draw_bricks_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord = NULL;
  ALLEGRO_BITMAP *bricks = NULL;

  if (em == DUNGEON) {
    bricks_coord = d_bricks_coord_01;
    bricks = dv_bricks_00;
  } else return;

  palette pal = get_palette (em, vm);
  bricks = apply_palette (bricks, pal);

  if (vm == VGA) bricks = apply_hue_palette (bricks);
  if (hgc) bricks = apply_palette (bricks, hgc_palette);
  if (peq (p, &mouse_pos))
    bricks = apply_palette (bricks, selection_palette);

  struct coord c;
  draw_bitmapc (bricks, bitmap, bricks_coord (p, &c) , 0);
}

void
draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord = NULL;
  ALLEGRO_BITMAP *bricks = NULL;

  if (em == DUNGEON) {
    bricks_coord = d_bricks_coord_02;
    bricks = dv_bricks_01;
  } else {
    bricks_coord = p_bricks_coord_02;
    bricks = pv_bricks_01;
  }

  palette pal = get_palette (em, vm);
  bricks = apply_palette (bricks, pal);

  if (vm == VGA) bricks = apply_hue_palette (bricks);
  if (hgc) bricks = apply_palette (bricks, hgc_palette);
  if (peq (p, &mouse_pos))
    bricks = apply_palette (bricks, selection_palette);

  struct coord c;
  draw_bitmapc (bricks, bitmap, bricks_coord (p, &c) , 0);
}

void
draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord = NULL;
  ALLEGRO_BITMAP *bricks = NULL;

  if (em == DUNGEON) {
    bricks_coord = d_bricks_coord_03;
    bricks = dv_bricks_02;
  } else {
    bricks_coord = p_bricks_coord_01;
    bricks = pv_bricks_00;
  }

  palette pal = get_palette (em, vm);
  bricks = apply_palette (bricks, pal);

  if (vm == VGA) bricks = apply_hue_palette (bricks);
  if (hgc) bricks = apply_palette (bricks, hgc_palette);
  if (peq (p, &mouse_pos))
    bricks = apply_palette (bricks, selection_palette);

  struct coord c;
  draw_bitmapc (bricks, bitmap, bricks_coord (p, &c) , 0);
}

void
draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord = NULL;
  ALLEGRO_BITMAP *bricks = NULL;

  if (em == DUNGEON) {
    bricks_coord = d_bricks_coord_04;
    bricks = dv_bricks_03;
  } else {
    bricks_coord = p_bricks_coord_02;
    bricks = pv_bricks_01;
  }

  palette pal = get_palette (em, vm);
  bricks = apply_palette (bricks, pal);

  if (vm == VGA) bricks = apply_hue_palette (bricks);
  if (hgc) bricks = apply_palette (bricks, hgc_palette);
  if (peq (p, &mouse_pos))
    bricks = apply_palette (bricks, selection_palette);

  struct coord c;
  draw_bitmapc (bricks, bitmap, bricks_coord (p, &c) , 0);
}

struct coord *
d_bricks_coord_01 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 1,
               PLACE_HEIGHT * p->floor + 16);
}

struct coord *
p_bricks_coord_01 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 29);
}

struct coord *
d_bricks_coord_02 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 2,
               PLACE_HEIGHT * p->floor + 21);
}

struct coord *
p_bricks_coord_02 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 26);
}

struct coord *
d_bricks_coord_03 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 4,
               PLACE_HEIGHT * p->floor + 11);
}

struct coord *
d_bricks_coord_04 (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1) + 4,
               PLACE_HEIGHT * p->floor + 10);
}

static ALLEGRO_COLOR
dv_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, DV_BRICKS_COLOR)) return DV_BRICKS_COLOR;
  else return c;
}

static ALLEGRO_COLOR
de_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, DV_BRICKS_COLOR)) return DE_BRICKS_COLOR;
  else return c;
}

static ALLEGRO_COLOR
dc_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, DV_BRICKS_COLOR)) return DC_BRICKS_COLOR;
  else return c;
}

static ALLEGRO_COLOR
pv_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PV_BRICKS_COLOR)) return PV_BRICKS_COLOR;
  else return c;
}

static ALLEGRO_COLOR
pe_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PV_BRICKS_COLOR)) return PE_BRICKS_COLOR;
  else return c;
}

static ALLEGRO_COLOR
pc_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, PV_BRICKS_COLOR)) return PC_BRICKS_COLOR;
  else return c;
}
