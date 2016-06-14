/*
  floor.c -- floor module;

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

/* dungeon cga */
ALLEGRO_BITMAP *dc_floor_base, *dc_floor_left,
  *dc_floor_right, *dc_floor_corner_00,
  *dc_floor_corner_01, *dc_floor_corner_02;

/* palace cga */
ALLEGRO_BITMAP *pc_floor_base, *pc_floor_left,
  *pc_floor_right, *pc_floor_corner_00,
  *pc_floor_corner_01, *pc_floor_corner_02;

/* dungeon ega */
ALLEGRO_BITMAP *de_floor_base, *de_floor_left,
  *de_floor_right, *de_floor_corner_00,
  *de_floor_corner_01, *de_floor_corner_02;

/* palace ega */
ALLEGRO_BITMAP *pe_floor_base, *pe_floor_left,
  *pe_floor_right, *pe_floor_corner_00,
  *pe_floor_corner_01, *pe_floor_corner_02;

/* dungeon vga */
ALLEGRO_BITMAP *dv_floor_base, *dv_floor_left,
  *dv_floor_right, *dv_floor_corner_00,
  *dv_floor_corner_01, *dv_floor_corner_02;

/* palace vga */
ALLEGRO_BITMAP *pv_floor_base, *pv_floor_left,
  *pv_floor_right, *pv_floor_corner_00,
  *pv_floor_corner_01, *pv_floor_corner_02;

void
load_floor (void)
{
  /* dungeon cga */
  dc_floor_base = load_bitmap (DC_FLOOR_BASE);
  dc_floor_left = load_bitmap (DC_FLOOR_LEFT);
  dc_floor_right = load_bitmap (DC_FLOOR_RIGHT);
  dc_floor_corner_00 = load_bitmap (DC_FLOOR_CORNER_00);
  dc_floor_corner_01 = load_bitmap (DC_FLOOR_CORNER_01);
  dc_floor_corner_02 = load_bitmap (DC_FLOOR_CORNER_02);

  /* palace cga */
  pc_floor_base = load_bitmap (PC_FLOOR_BASE);
  pc_floor_left = load_bitmap (PC_FLOOR_LEFT);
  pc_floor_right = load_bitmap (PC_FLOOR_RIGHT);
  pc_floor_corner_00 = load_bitmap (PC_FLOOR_CORNER_00);
  pc_floor_corner_01 = load_bitmap (PC_FLOOR_CORNER_01);
  pc_floor_corner_02 = load_bitmap (PC_FLOOR_CORNER_02);

  /* dungeon ega */
  de_floor_base = load_bitmap (DE_FLOOR_BASE);
  de_floor_left = load_bitmap (DE_FLOOR_LEFT);
  de_floor_right = load_bitmap (DE_FLOOR_RIGHT);
  de_floor_corner_00 = load_bitmap (DE_FLOOR_CORNER_00);
  de_floor_corner_01 = load_bitmap (DE_FLOOR_CORNER_01);
  de_floor_corner_02 = load_bitmap (DE_FLOOR_CORNER_02);

  /* palace ega */
  pe_floor_base = load_bitmap (PE_FLOOR_BASE);
  pe_floor_left = load_bitmap (PE_FLOOR_LEFT);
  pe_floor_right = load_bitmap (PE_FLOOR_RIGHT);
  pe_floor_corner_00 = load_bitmap (PE_FLOOR_CORNER_00);
  pe_floor_corner_01 = load_bitmap (PE_FLOOR_CORNER_01);
  pe_floor_corner_02 = load_bitmap (PE_FLOOR_CORNER_02);

  /* dungeon vga */
  dv_floor_base = load_bitmap (DV_FLOOR_BASE);
  dv_floor_left = load_bitmap (DV_FLOOR_LEFT);
  dv_floor_right = load_bitmap (DV_FLOOR_RIGHT);
  dv_floor_corner_00 = load_bitmap (DV_FLOOR_CORNER_00);
  dv_floor_corner_01 = load_bitmap (DV_FLOOR_CORNER_01);
  dv_floor_corner_02 = load_bitmap (DV_FLOOR_CORNER_02);

  /* palace vga */
  pv_floor_base = load_bitmap (PV_FLOOR_BASE);
  pv_floor_left = load_bitmap (PV_FLOOR_LEFT);
  pv_floor_right = load_bitmap (PV_FLOOR_RIGHT);
  pv_floor_corner_00 = load_bitmap (PV_FLOOR_CORNER_00);
  pv_floor_corner_01 = load_bitmap (PV_FLOOR_CORNER_01);
  pv_floor_corner_02 = load_bitmap (PV_FLOOR_CORNER_02);
}

void
unload_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_floor_left);
  destroy_bitmap (dc_floor_right);
  destroy_bitmap (dc_floor_base);
  destroy_bitmap (dc_floor_corner_00);
  destroy_bitmap (dc_floor_corner_01);
  destroy_bitmap (dc_floor_corner_02);

  /* palace cga */
  destroy_bitmap (pc_floor_left);
  destroy_bitmap (pc_floor_right);
  destroy_bitmap (pc_floor_base);
  destroy_bitmap (pc_floor_corner_00);
  destroy_bitmap (pc_floor_corner_01);
  destroy_bitmap (pc_floor_corner_02);

  /* dungeon ega */
  destroy_bitmap (de_floor_left);
  destroy_bitmap (de_floor_right);
  destroy_bitmap (de_floor_base);
  destroy_bitmap (de_floor_corner_00);
  destroy_bitmap (de_floor_corner_01);
  destroy_bitmap (de_floor_corner_02);

  /* palace ega */
  destroy_bitmap (pe_floor_left);
  destroy_bitmap (pe_floor_right);
  destroy_bitmap (pe_floor_base);
  destroy_bitmap (pe_floor_corner_00);
  destroy_bitmap (pe_floor_corner_01);
  destroy_bitmap (pe_floor_corner_02);

  /* dungeon vga */
  destroy_bitmap (dv_floor_left);
  destroy_bitmap (dv_floor_right);
  destroy_bitmap (dv_floor_base);
  destroy_bitmap (dv_floor_corner_00);
  destroy_bitmap (dv_floor_corner_01);
  destroy_bitmap (dv_floor_corner_02);

  /* palace vga */
  destroy_bitmap (pv_floor_left);
  destroy_bitmap (pv_floor_right);
  destroy_bitmap (pv_floor_base);
  destroy_bitmap (pv_floor_corner_00);
  destroy_bitmap (pv_floor_corner_01);
  destroy_bitmap (pv_floor_corner_02);
}

void
draw_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
            enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, vm, em);
  draw_floor_left (bitmap, p, vm, em);
  draw_floor_right (bitmap, p, vm, em);
}

void
draw_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_base = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_base = dc_floor_base; break;
    case EGA: floor_base = de_floor_base; break;
    case VGA: floor_base = dv_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_base = pc_floor_base; break;
    case EGA: floor_base = pe_floor_base; break;
    case VGA: floor_base = pv_floor_base; break;
    }
    break;
  }

  if (vm == VGA) floor_base = apply_hue_palette (floor_base);
  if (hgc) floor_base = apply_palette (floor_base, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_base = apply_palette (floor_base, selection_palette);

  struct coord c;
  draw_bitmapc (floor_base, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_left = dc_floor_left; break;
    case EGA: floor_left = de_floor_left; break;
    case VGA: floor_left = dv_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_left = pc_floor_left; break;
    case EGA: floor_left = pe_floor_left; break;
    case VGA: floor_left = pv_floor_left; break;
    }
    break;
  }

  if (vm == VGA) floor_left = apply_hue_palette (floor_left);
  if (hgc) floor_left = apply_palette (floor_left, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_left = apply_palette (floor_left, selection_palette);

  struct coord c;
  draw_bitmapc (floor_left, bitmap, floor_left_coord (p, &c), 0);
}

void
draw_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_right = dc_floor_right; break;
    case EGA: floor_right = de_floor_right; break;
    case VGA: floor_right = dv_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_right = pc_floor_right; break;
    case EGA: floor_right = pe_floor_right; break;
    case VGA: floor_right = pv_floor_right; break;
    }
    break;
  }

  if (vm == VGA) floor_right = apply_hue_palette (floor_right);
  if (hgc) floor_right = apply_palette (floor_right, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_right = apply_palette (floor_right, selection_palette);

  struct coord c;
  draw_bitmapc (floor_right, bitmap, floor_right_coord (p, &c), 0);
}

void
draw_floor_corner_00 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_corner_00 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_corner_00 = dc_floor_corner_00; break;
    case EGA: floor_corner_00 = de_floor_corner_00; break;
    case VGA: floor_corner_00 = dv_floor_corner_00; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_corner_00 = pc_floor_corner_00; break;
    case EGA: floor_corner_00 = pe_floor_corner_00; break;
    case VGA: floor_corner_00 = pv_floor_corner_00; break;
    }
    break;
  }

  if (vm == VGA) floor_corner_00 = apply_hue_palette (floor_corner_00);
  if (hgc) floor_corner_00 = apply_palette (floor_corner_00, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_corner_00 = apply_palette (floor_corner_00, selection_palette);

  struct coord c; floor_corner_00_coord (p, &c);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_00, bitmap, &c, 0);
}

void
draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_corner_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_corner_01 = dc_floor_corner_01; break;
    case EGA: floor_corner_01 = de_floor_corner_01; break;
    case VGA: floor_corner_01 = dv_floor_corner_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_corner_01 = pc_floor_corner_01; break;
    case EGA: floor_corner_01 = pe_floor_corner_01; break;
    case VGA: floor_corner_01 = pv_floor_corner_01; break;
    }
    break;
  }

  if (vm == VGA) floor_corner_01 = apply_hue_palette (floor_corner_01);
  if (hgc) floor_corner_01 = apply_palette (floor_corner_01, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_corner_01 = apply_palette (floor_corner_01, selection_palette);

  struct coord c; floor_corner_01_coord (p, &c);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_01, bitmap, &c, 0);
}

void
draw_floor_corner_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_corner_02 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: floor_corner_02 = dc_floor_corner_02; break;
    case EGA: floor_corner_02 = de_floor_corner_02; break;
    case VGA: floor_corner_02 = dv_floor_corner_02; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: floor_corner_02 = pc_floor_corner_02; break;
    case EGA: floor_corner_02 = pe_floor_corner_02; break;
    case VGA: floor_corner_02 = pv_floor_corner_02; break;
    }
    break;
  }

  if (vm == VGA) floor_corner_02 = apply_hue_palette (floor_corner_02);
  if (hgc) floor_corner_02 = apply_palette (floor_corner_02, hgc_palette);
  if (peq (p, &mouse_pos))
    floor_corner_02 = apply_palette (floor_corner_02, selection_palette);

  struct coord c; floor_corner_02_coord (p, &c);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_02, bitmap, &c, 0);
}

struct coord *
floor_base_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * (p->floor + 1));
}

struct coord *
floor_left_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 50);
}

struct coord *
floor_right_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 50);
}

struct coord *
floor_corner_00_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 53);
}

struct coord *
floor_corner_01_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 53);
}

struct coord *
floor_corner_02_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 55);
}
