/*
  carpet.c -- carpet module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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
ALLEGRO_BITMAP *dc_carpet_00, *dc_carpet_top_00,
  *dc_carpet_01, *dc_carpet_top_01;

/* palace cga */
ALLEGRO_BITMAP *pc_carpet_00, *pc_carpet_top_00,
  *pc_carpet_01, *pc_carpet_top_01;

/* dungeon ega */
ALLEGRO_BITMAP *de_carpet_00, *de_carpet_top_00,
  *de_carpet_01, *de_carpet_top_01;

/* palace ega */
ALLEGRO_BITMAP *pe_carpet_00, *pe_carpet_top_00,
  *pe_carpet_01, *pe_carpet_top_01;

/* dungeon vga */
ALLEGRO_BITMAP *dv_carpet_00, *dv_carpet_top_00,
  *dv_carpet_01, *dv_carpet_top_01;

/* palace vga */
ALLEGRO_BITMAP *pv_carpet_00, *pv_carpet_top_00,
  *pv_carpet_01, *pv_carpet_top_01;

void
load_carpet (void)
{
  /* dungeon cga */
  dc_carpet_00 = load_bitmap (DC_CARPET_00);
  dc_carpet_top_00 = load_bitmap (DC_CARPET_TOP_00);
  dc_carpet_01 = load_bitmap (DC_CARPET_01);
  dc_carpet_top_01 = load_bitmap (DC_CARPET_TOP_01);

  /* palace cga */
  pc_carpet_00 = load_bitmap (PC_CARPET_00);
  pc_carpet_top_00 = load_bitmap (PC_CARPET_TOP_00);
  pc_carpet_01 = load_bitmap (PC_CARPET_01);
  pc_carpet_top_01 = load_bitmap (PC_CARPET_TOP_01);

  /* dungeon ega */
  de_carpet_00 = load_bitmap (DE_CARPET_00);
  de_carpet_top_00 = load_bitmap (DE_CARPET_TOP_00);
  de_carpet_01 = load_bitmap (DE_CARPET_01);
  de_carpet_top_01 = load_bitmap (DE_CARPET_TOP_01);

  /* palace ega */
  pe_carpet_00 = load_bitmap (PE_CARPET_00);
  pe_carpet_top_00 = load_bitmap (PE_CARPET_TOP_00);
  pe_carpet_01 = load_bitmap (PE_CARPET_01);
  pe_carpet_top_01 = load_bitmap (PE_CARPET_TOP_01);

  /* dungeon vga */
  dv_carpet_00 = load_bitmap (DV_CARPET_00);
  dv_carpet_top_00 = load_bitmap (DV_CARPET_TOP_00);
  dv_carpet_01 = load_bitmap (DV_CARPET_01);
  dv_carpet_top_01 = load_bitmap (DV_CARPET_TOP_01);

  /* palace vga */
  pv_carpet_00 = load_bitmap (PV_CARPET_00);
  pv_carpet_top_00 = load_bitmap (PV_CARPET_TOP_00);
  pv_carpet_01 = load_bitmap (PV_CARPET_01);
  pv_carpet_top_01 = load_bitmap (PV_CARPET_TOP_01);
}

void
unload_carpet (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_carpet_00);
  destroy_bitmap (dc_carpet_top_00);
  destroy_bitmap (dc_carpet_01);
  destroy_bitmap (dc_carpet_top_01);

  /* palace cga */
  destroy_bitmap (pc_carpet_00);
  destroy_bitmap (pc_carpet_top_00);
  destroy_bitmap (pc_carpet_01);
  destroy_bitmap (pc_carpet_top_01);

  /* dungeon ega */
  destroy_bitmap (de_carpet_00);
  destroy_bitmap (de_carpet_top_00);
  destroy_bitmap (de_carpet_01);
  destroy_bitmap (de_carpet_top_01);

  /* palace ega */
  destroy_bitmap (pe_carpet_00);
  destroy_bitmap (pe_carpet_top_00);
  destroy_bitmap (pe_carpet_01);
  destroy_bitmap (pe_carpet_top_01);

  /* dungeon vga */
  destroy_bitmap (dv_carpet_00);
  destroy_bitmap (dv_carpet_top_00);
  destroy_bitmap (dv_carpet_01);
  destroy_bitmap (dv_carpet_top_01);

  /* palace vga */
  destroy_bitmap (pv_carpet_00);
  destroy_bitmap (pv_carpet_top_00);
  destroy_bitmap (pv_carpet_01);
  destroy_bitmap (pv_carpet_top_01);
}

ALLEGRO_BITMAP *
carpet_bitmap (struct pos *p, enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *carpet_00 = NULL,
    *carpet_01 = NULL,
    *arch_carpet = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet_00 = dc_carpet_00;
      carpet_01 = dc_carpet_01;
      arch_carpet = dc_arch_top_left_end;
      break;
    case EGA:
      carpet_00 = de_carpet_00;
      carpet_01 = de_carpet_01;
      arch_carpet = de_arch_top_left_end;
      break;
    case VGA:
      carpet_00 = dv_carpet_00;
      carpet_01 = dv_carpet_01;
      arch_carpet = dv_arch_top_left_end;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet_00 = pc_carpet_00;
      carpet_01 = pc_carpet_01;
      arch_carpet = pc_arch_top_left_end;
      break;
    case EGA:
      carpet_00 = pe_carpet_00;
      carpet_01 = pe_carpet_01;
      arch_carpet = pe_arch_top_left_end;
      break;
    case VGA:
      carpet_00 = pv_carpet_00;
      carpet_01 = pv_carpet_01;
      arch_carpet = pv_arch_top_left_end;
      break;
    }
    break;
  }

  switch (fake_ext (p)) {
  case CARPET_00: return carpet_00;
  case CARPET_01: return carpet_01;
  case ARCH_CARPET_LEFT_00: case ARCH_CARPET_LEFT_01:
    return arch_carpet;
  case ARCH_CARPET_RIGHT_00: return carpet_00;
  case ARCH_CARPET_RIGHT_01: return carpet_01;
  default: assert (false); break;
  }
  assert (false);
  return NULL;
}

ALLEGRO_BITMAP *
carpet_top_bitmap (struct pos *p, enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *carpet_top_00 = NULL,
    *carpet_top_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet_top_00 = dc_carpet_top_00;
      carpet_top_01 = dc_carpet_top_01;
      break;
    case EGA:
      carpet_top_00 = de_carpet_top_00;
      carpet_top_01 = de_carpet_top_01;
      break;
    case VGA:
      carpet_top_00 = dv_carpet_top_00;
      carpet_top_01 = dv_carpet_top_01;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet_top_00 = pc_carpet_top_00;
      carpet_top_01 = pc_carpet_top_01;
      break;
    case EGA:
      carpet_top_00 = pe_carpet_top_00;
      carpet_top_01 = pe_carpet_top_01;
      break;
    case VGA:
      carpet_top_00 = pv_carpet_top_00;
      carpet_top_01 = pv_carpet_top_01;
      break;
    }
    break;
  }

  switch (fake_ext (p)) {
  case CARPET_00: return carpet_top_00;
  case CARPET_01: return carpet_top_01;
  case ARCH_CARPET_LEFT_00: case ARCH_CARPET_LEFT_01:
    return NULL;
  case ARCH_CARPET_RIGHT_00: return carpet_top_00;
  case ARCH_CARPET_RIGHT_01: return carpet_top_01;
  default: assert (false); break;
  }
  assert (false);
  return NULL;
}

void
draw_carpet_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   int w, enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet = carpet_bitmap (p, em, vm);

  if (vm == VGA) carpet = apply_hue_palette (carpet);
  if (hgc) carpet = apply_palette (carpet, hgc_palette);
  if (peq (p, &mouse_pos))
    carpet = apply_palette (carpet, selection_palette);

  draw_bitmap_regionc (carpet, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (carpet) : w,
                       al_get_bitmap_height (carpet),
                       carpet_coord (p, &c), 0);
}

void
draw_carpet_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 int w, enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet_top = carpet_top_bitmap (p, em, vm);

  if (! carpet_top) return;

  if (vm == VGA) carpet_top = apply_hue_palette (carpet_top);
  if (hgc) carpet_top = apply_palette (carpet_top, hgc_palette);
  if (peq (p, &mouse_pos))
    carpet_top = apply_palette (carpet_top, selection_palette);

  draw_bitmap_regionc (carpet_top, bitmap, 0, 0,
                       w < 0 ? al_get_bitmap_width (carpet_top) : w,
                       al_get_bitmap_height (carpet_top),
                       carpet_top_coord (p, &c), 0);
}

void
draw_carpet_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                struct frame *f, enum em em, enum vm vm)
{
  if (fg (p) == TCARPET)
    draw_door_pole_base (bitmap, p, em, vm);

  draw_door_pole (bitmap, p, em, vm);

  struct pos par; prel (p, &par, -1, +1);

  switch (should_draw_face (p, f)) {
  case SHOULD_DRAW_FULL:
    push_drawn_rectangle (bitmap);
    draw_confg_right (bitmap, p, em, vm);
    draw_confg_top (bitmap, p, em, vm);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p, em, vm);
    break;
  case SHOULD_DRAW_PART:
    push_drawn_rectangle (bitmap);
    draw_carpet_right (bitmap, p, CARPET_FG_WIDTH, em, vm);
    if (fake (&par) == NO_FLOOR)
      draw_carpet_top (bitmap, p, CARPET_FG_WIDTH, em, vm);
    redraw_drawn_rectangle (pop_drawn_rectangle (), p, em, vm);
    break;
  case SHOULD_DRAW_NONE: default: break;
  }
}

struct coord *
carpet_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor + 3);
}

struct coord *
carpet_top_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * (p->place + 1),
               PLACE_HEIGHT * p->floor - 9);
}
