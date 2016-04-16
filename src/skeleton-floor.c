/*
  skeleton-floor.c -- skeleton floor module;

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
ALLEGRO_BITMAP *dc_skeleton_floor_left, *dc_skeleton_floor_right;

/* palace cga */
ALLEGRO_BITMAP *pc_skeleton_floor_left, *pc_skeleton_floor_right;

/* dungeon ega */
ALLEGRO_BITMAP *de_skeleton_floor_left, *de_skeleton_floor_right;

/* palace ega */
ALLEGRO_BITMAP *pe_skeleton_floor_left, *pe_skeleton_floor_right;

/* dungeon vga */
ALLEGRO_BITMAP *dv_skeleton_floor_left, *dv_skeleton_floor_right;

/* palace vga */
ALLEGRO_BITMAP *pv_skeleton_floor_left, *pv_skeleton_floor_right;

void
load_skeleton_floor (void)
{
  /* dungeon cga */
  dc_skeleton_floor_left = load_bitmap (DC_SKELETON_FLOOR_LEFT);
  dc_skeleton_floor_right = load_bitmap (DC_SKELETON_FLOOR_RIGHT);

  /* palace cga */
  pc_skeleton_floor_left = load_bitmap (PC_SKELETON_FLOOR_LEFT);
  pc_skeleton_floor_right = load_bitmap (PC_SKELETON_FLOOR_RIGHT);

  /* dungeon ega */
  de_skeleton_floor_left = load_bitmap (DE_SKELETON_FLOOR_LEFT);
  de_skeleton_floor_right = load_bitmap (DE_SKELETON_FLOOR_RIGHT);

  /* palace ega */
  pe_skeleton_floor_left = load_bitmap (PE_SKELETON_FLOOR_LEFT);
  pe_skeleton_floor_right = load_bitmap (PE_SKELETON_FLOOR_RIGHT);

  /* dungeon vga */
  dv_skeleton_floor_left = load_bitmap (DV_SKELETON_FLOOR_LEFT);
  dv_skeleton_floor_right = load_bitmap (DV_SKELETON_FLOOR_RIGHT);

  /* palace vga */
  pv_skeleton_floor_left = load_bitmap (PV_SKELETON_FLOOR_LEFT);
  pv_skeleton_floor_right = load_bitmap (PV_SKELETON_FLOOR_RIGHT);
}

void
unload_skeleton_floor (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_skeleton_floor_left);
  destroy_bitmap (dc_skeleton_floor_right);

  /* palace cga */
  destroy_bitmap (pc_skeleton_floor_left);
  destroy_bitmap (pc_skeleton_floor_right);

  /* dungeon ega */
  destroy_bitmap (de_skeleton_floor_left);
  destroy_bitmap (de_skeleton_floor_right);

  /* palace ega */
  destroy_bitmap (pe_skeleton_floor_left);
  destroy_bitmap (pe_skeleton_floor_right);

  /* dungeon vga */
  destroy_bitmap (dv_skeleton_floor_left);
  destroy_bitmap (dv_skeleton_floor_right);

  /* palace vga */
  destroy_bitmap (pv_skeleton_floor_left);
  destroy_bitmap (pv_skeleton_floor_right);
}

void
draw_skeleton_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_skeleton_floor_left (bitmap, p, em, vm);
  draw_skeleton_floor_right (bitmap, p, em, vm);
}

void
draw_skeleton_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *skeleton_floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: skeleton_floor_left = dc_skeleton_floor_left; break;
    case EGA: skeleton_floor_left = de_skeleton_floor_left; break;
    case VGA: skeleton_floor_left = dv_skeleton_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: skeleton_floor_left = pc_skeleton_floor_left; break;
    case EGA: skeleton_floor_left = pe_skeleton_floor_left; break;
    case VGA: skeleton_floor_left = pv_skeleton_floor_left; break;
    }
    break;
  }

  if (vm == VGA) skeleton_floor_left = apply_hue_palette (skeleton_floor_left);
  if (hgc) skeleton_floor_left = apply_palette (skeleton_floor_left, hgc_palette);
  if (peq (p, &mouse_pos))
    skeleton_floor_left = apply_palette (skeleton_floor_left, selection_palette);

  struct coord c;
  draw_bitmapc (skeleton_floor_left, bitmap, skeleton_floor_left_coord (p, &c), 0);
}

void
draw_skeleton_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *skeleton_floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: skeleton_floor_right = dc_skeleton_floor_right; break;
    case EGA: skeleton_floor_right = de_skeleton_floor_right; break;
    case VGA: skeleton_floor_right = dv_skeleton_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: skeleton_floor_right = pc_skeleton_floor_right; break;
    case EGA: skeleton_floor_right = pe_skeleton_floor_right; break;
    case VGA: skeleton_floor_right = pv_skeleton_floor_right; break;
    }
    break;
  }

  if (vm == VGA) skeleton_floor_right = apply_hue_palette (skeleton_floor_right);
  if (hgc) skeleton_floor_right = apply_palette (skeleton_floor_right, hgc_palette);
  if (peq (p, &mouse_pos))
    skeleton_floor_right = apply_palette (skeleton_floor_right, selection_palette);

  struct coord c;
  draw_bitmapc (skeleton_floor_right, bitmap, skeleton_floor_right_coord (p, &c), 0);
}

struct coord *
skeleton_floor_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 44;
  c->room = p->room;
  return c;
}

struct coord *
skeleton_floor_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 44;
  c->room = p->room;
  return c;
}
