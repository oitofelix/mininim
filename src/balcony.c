/*
  balcony.c -- balcony module;

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
ALLEGRO_BITMAP *dc_balcony_top_left, *dc_balcony_top_right,
  *dc_balcony_bottom_left, *dc_balcony_bottom_right;

/* palace cga */
ALLEGRO_BITMAP *pc_balcony_top_left, *pc_balcony_top_right,
  *pc_balcony_bottom_left, *pc_balcony_bottom_right;

/* dungeon ega */
ALLEGRO_BITMAP *de_balcony_top_left, *de_balcony_top_right,
  *de_balcony_bottom_left, *de_balcony_bottom_right;

/* palace ega */
ALLEGRO_BITMAP *pe_balcony_top_left, *pe_balcony_top_right,
  *pe_balcony_bottom_left, *pe_balcony_bottom_right;

/* dungeon vga */
ALLEGRO_BITMAP *dv_balcony_top_left, *dv_balcony_top_right,
  *dv_balcony_bottom_left, *dv_balcony_bottom_right;

/* palace vga */
ALLEGRO_BITMAP *pv_balcony_top_left, *pv_balcony_top_right,
  *pv_balcony_bottom_left, *pv_balcony_bottom_right;

void
load_balcony (void)
{
  /* dungeon cga */
  dc_balcony_top_left = load_bitmap (DC_BALCONY_TOP_LEFT);
  dc_balcony_top_right = load_bitmap (DC_BALCONY_TOP_RIGHT);
  dc_balcony_bottom_left = load_bitmap (DC_BALCONY_BOTTOM_LEFT);
  dc_balcony_bottom_right = load_bitmap (DC_BALCONY_BOTTOM_RIGHT);

  /* palace cga */
  pc_balcony_top_left = load_bitmap (PC_BALCONY_TOP_LEFT);
  pc_balcony_top_right = load_bitmap (PC_BALCONY_TOP_RIGHT);
  pc_balcony_bottom_left = load_bitmap (PC_BALCONY_BOTTOM_LEFT);
  pc_balcony_bottom_right = load_bitmap (PC_BALCONY_BOTTOM_RIGHT);

  /* dungeon ega */
  de_balcony_top_left = load_bitmap (DE_BALCONY_TOP_LEFT);
  de_balcony_top_right = load_bitmap (DE_BALCONY_TOP_RIGHT);
  de_balcony_bottom_left = load_bitmap (DE_BALCONY_BOTTOM_LEFT);
  de_balcony_bottom_right = load_bitmap (DE_BALCONY_BOTTOM_RIGHT);

  /* palace ega */
  pe_balcony_top_left = load_bitmap (PE_BALCONY_TOP_LEFT);
  pe_balcony_top_right = load_bitmap (PE_BALCONY_TOP_RIGHT);
  pe_balcony_bottom_left = load_bitmap (PE_BALCONY_BOTTOM_LEFT);
  pe_balcony_bottom_right = load_bitmap (PE_BALCONY_BOTTOM_RIGHT);

  /* dungeon vga */
  dv_balcony_top_left = load_bitmap (DV_BALCONY_TOP_LEFT);
  dv_balcony_top_right = load_bitmap (DV_BALCONY_TOP_RIGHT);
  dv_balcony_bottom_left = load_bitmap (DV_BALCONY_BOTTOM_LEFT);
  dv_balcony_bottom_right = load_bitmap (DV_BALCONY_BOTTOM_RIGHT);

  /* palace vga */
  pv_balcony_top_left = load_bitmap (PV_BALCONY_TOP_LEFT);
  pv_balcony_top_right = load_bitmap (PV_BALCONY_TOP_RIGHT);
  pv_balcony_bottom_left = load_bitmap (PV_BALCONY_BOTTOM_LEFT);
  pv_balcony_bottom_right = load_bitmap (PV_BALCONY_BOTTOM_RIGHT);
}

void
unload_balcony (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_balcony_top_left);
  destroy_bitmap (dc_balcony_top_right);
  destroy_bitmap (dc_balcony_bottom_left);
  destroy_bitmap (dc_balcony_bottom_right);

  /* palace cga */
  destroy_bitmap (pc_balcony_top_left);
  destroy_bitmap (pc_balcony_top_right);
  destroy_bitmap (pc_balcony_bottom_left);
  destroy_bitmap (pc_balcony_bottom_right);

  /* dungeon ega */
  destroy_bitmap (de_balcony_top_left);
  destroy_bitmap (de_balcony_top_right);
  destroy_bitmap (de_balcony_bottom_left);
  destroy_bitmap (de_balcony_bottom_right);

  /* palace ega */
  destroy_bitmap (pe_balcony_top_left);
  destroy_bitmap (pe_balcony_top_right);
  destroy_bitmap (pe_balcony_bottom_left);
  destroy_bitmap (pe_balcony_bottom_right);

  /* dungeon vga */
  destroy_bitmap (dv_balcony_top_left);
  destroy_bitmap (dv_balcony_top_right);
  destroy_bitmap (dv_balcony_bottom_left);
  destroy_bitmap (dv_balcony_bottom_right);

  /* palace vga */
  destroy_bitmap (pv_balcony_top_left);
  destroy_bitmap (pv_balcony_top_right);
  destroy_bitmap (pv_balcony_bottom_left);
  destroy_bitmap (pv_balcony_bottom_right);
}

void
draw_balcony (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *balcony_bottom_left = NULL,
    *balcony_bottom_right = NULL,
    *balcony_top_left = NULL,
    *balcony_top_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      balcony_bottom_left = dc_balcony_bottom_left;
      balcony_bottom_right = dc_balcony_bottom_right;
      balcony_top_left = dc_balcony_top_left;
      balcony_top_right = dc_balcony_top_right;
      break;
    case EGA:
      balcony_bottom_left = de_balcony_bottom_left;
      balcony_bottom_right = de_balcony_bottom_right;
      balcony_top_left = de_balcony_top_left;
      balcony_top_right = de_balcony_top_right;
      break;
    case VGA:
      balcony_bottom_left = dv_balcony_bottom_left;
      balcony_bottom_right = dv_balcony_bottom_right;
      balcony_top_left = dv_balcony_top_left;
      balcony_top_right = dv_balcony_top_right;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      balcony_bottom_left = pc_balcony_bottom_left;
      balcony_bottom_right = pc_balcony_bottom_right;
      balcony_top_left = pc_balcony_top_left;
      balcony_top_right = pc_balcony_top_right;
      break;
    case EGA:
      balcony_bottom_left = pe_balcony_bottom_left;
      balcony_bottom_right = pe_balcony_bottom_right;
      balcony_top_left = pe_balcony_top_left;
      balcony_top_right = pe_balcony_top_right;
      break;
    case VGA:
      balcony_bottom_left = pv_balcony_bottom_left;
      balcony_bottom_right = pv_balcony_bottom_right;
      balcony_top_left = pv_balcony_top_left;
      balcony_top_right = pv_balcony_top_right;
      break;
    }
    break;
  }

  if (vm == VGA) {
    balcony_bottom_left = apply_hue_palette (balcony_bottom_left);
    balcony_bottom_right = apply_hue_palette (balcony_bottom_right);
    balcony_top_left = apply_hue_palette (balcony_top_left);
    balcony_top_right = apply_hue_palette (balcony_top_right);
  }

  if (hgc) {
    balcony_bottom_left = apply_palette (balcony_bottom_left, hgc_palette);
    balcony_bottom_right = apply_palette (balcony_bottom_right, hgc_palette);
    balcony_top_left = apply_palette (balcony_top_left, hgc_palette);
    balcony_top_right = apply_palette (balcony_top_right, hgc_palette);
  }

  if (peq (p, &mouse_pos)) {
    balcony_bottom_left = apply_palette (balcony_bottom_left, selection_palette);
    balcony_bottom_right = apply_palette (balcony_bottom_right, selection_palette);
    balcony_top_left = apply_palette (balcony_top_left, selection_palette);
    balcony_top_right = apply_palette (balcony_top_right, selection_palette);
  }

  draw_bitmapc (balcony_bottom_left, bitmap,
                balcony_bottom_left_coord (p, &c), 0);
  draw_bitmapc (balcony_bottom_right, bitmap,
                balcony_bottom_right_coord (p, &c), 0);
  draw_bitmapc (balcony_top_left, bitmap,
                balcony_top_left_coord (p, &c), 0);
  draw_bitmapc (balcony_top_right, bitmap,
                balcony_top_right_coord (p, &c), 0);
}

struct coord *
balcony_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
balcony_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
balcony_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 17;
  c->room = p->room;
  return c;
}

struct coord *
balcony_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 17;
  c->room = p->room;
  return c;
}
