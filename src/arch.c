/*
  arch.c -- arch module;

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
ALLEGRO_BITMAP *dc_arch_bottom, *dc_arch_top_left, *dc_arch_top_left_end,
  *dc_arch_top_right, *dc_arch_top_right_end, *dc_arch_top_mid,
  *dc_arch_top_small, *dc_arch_top_top;

/* palace cga */
ALLEGRO_BITMAP *pc_arch_bottom, *pc_arch_top_left, *pc_arch_top_left_end,
  *pc_arch_top_right, *pc_arch_top_right_end, *pc_arch_top_mid,
  *pc_arch_top_small, *pc_arch_top_top;

/* dungeon ega */
ALLEGRO_BITMAP *de_arch_bottom, *de_arch_top_left, *de_arch_top_left_end,
  *de_arch_top_right, *de_arch_top_right_end, *de_arch_top_mid,
  *de_arch_top_small, *de_arch_top_top;

/* palace ega */
ALLEGRO_BITMAP *pe_arch_bottom, *pe_arch_top_left, *pe_arch_top_left_end,
  *pe_arch_top_right, *pe_arch_top_right_end, *pe_arch_top_mid,
  *pe_arch_top_small, *pe_arch_top_top;

/* dungeon vga */
ALLEGRO_BITMAP *dv_arch_bottom, *dv_arch_top_left, *dv_arch_top_left_end,
  *dv_arch_top_right, *dv_arch_top_right_end, *dv_arch_top_mid,
  *dv_arch_top_small, *dv_arch_top_top;

/* palace vga */
ALLEGRO_BITMAP *pv_arch_bottom, *pv_arch_top_left, *pv_arch_top_left_end,
  *pv_arch_top_right, *pv_arch_top_right_end, *pv_arch_top_mid,
  *pv_arch_top_small, *pv_arch_top_top;

void
load_arch (void)
{
  /* dungeon cga */
  dc_arch_bottom = load_bitmap (DC_ARCH_BOTTOM);
  dc_arch_top_left = load_bitmap (DC_ARCH_TOP_LEFT);
  dc_arch_top_left_end = load_bitmap (DC_ARCH_TOP_LEFT_END);
  dc_arch_top_right = load_bitmap (DC_ARCH_TOP_RIGHT);
  dc_arch_top_right_end = load_bitmap (DC_ARCH_TOP_RIGHT_END);
  dc_arch_top_mid = load_bitmap (DC_ARCH_TOP_MID);
  dc_arch_top_small = load_bitmap (DC_ARCH_TOP_SMALL);
  dc_arch_top_top = load_bitmap (DC_ARCH_TOP_TOP);

  /* palace cga */
  pc_arch_bottom = load_bitmap (PC_ARCH_BOTTOM);
  pc_arch_top_left = load_bitmap (PC_ARCH_TOP_LEFT);
  pc_arch_top_left_end = load_bitmap (PC_ARCH_TOP_LEFT_END);
  pc_arch_top_right = load_bitmap (PC_ARCH_TOP_RIGHT);
  pc_arch_top_right_end = load_bitmap (PC_ARCH_TOP_RIGHT_END);
  pc_arch_top_mid = load_bitmap (PC_ARCH_TOP_MID);
  pc_arch_top_small = load_bitmap (PC_ARCH_TOP_SMALL);
  pc_arch_top_top = load_bitmap (PC_ARCH_TOP_TOP);

  /* dungeon ega */
  de_arch_bottom = load_bitmap (DE_ARCH_BOTTOM);
  de_arch_top_left = load_bitmap (DE_ARCH_TOP_LEFT);
  de_arch_top_left_end = load_bitmap (DE_ARCH_TOP_LEFT_END);
  de_arch_top_right = load_bitmap (DE_ARCH_TOP_RIGHT);
  de_arch_top_right_end = load_bitmap (DE_ARCH_TOP_RIGHT_END);
  de_arch_top_mid = load_bitmap (DE_ARCH_TOP_MID);
  de_arch_top_small = load_bitmap (DE_ARCH_TOP_SMALL);
  de_arch_top_top = load_bitmap (DE_ARCH_TOP_TOP);

  /* palace ega */
  pe_arch_bottom = load_bitmap (PE_ARCH_BOTTOM);
  pe_arch_top_left = load_bitmap (PE_ARCH_TOP_LEFT);
  pe_arch_top_left_end = load_bitmap (PE_ARCH_TOP_LEFT_END);
  pe_arch_top_right = load_bitmap (PE_ARCH_TOP_RIGHT);
  pe_arch_top_right_end = load_bitmap (PE_ARCH_TOP_RIGHT_END);
  pe_arch_top_mid = load_bitmap (PE_ARCH_TOP_MID);
  pe_arch_top_small = load_bitmap (PE_ARCH_TOP_SMALL);
  pe_arch_top_top = load_bitmap (PE_ARCH_TOP_TOP);

  /* dungeon vga */
  dv_arch_bottom = load_bitmap (DV_ARCH_BOTTOM);
  dv_arch_top_left = load_bitmap (DV_ARCH_TOP_LEFT);
  dv_arch_top_left_end = load_bitmap (DV_ARCH_TOP_LEFT_END);
  dv_arch_top_right = load_bitmap (DV_ARCH_TOP_RIGHT);
  dv_arch_top_right_end = load_bitmap (DV_ARCH_TOP_RIGHT_END);
  dv_arch_top_mid = load_bitmap (DV_ARCH_TOP_MID);
  dv_arch_top_small = load_bitmap (DV_ARCH_TOP_SMALL);
  dv_arch_top_top = load_bitmap (DV_ARCH_TOP_TOP);

  /* palace vga */
  pv_arch_bottom = load_bitmap (PV_ARCH_BOTTOM);
  pv_arch_top_left = load_bitmap (PV_ARCH_TOP_LEFT);
  pv_arch_top_left_end = load_bitmap (PV_ARCH_TOP_LEFT_END);
  pv_arch_top_right = load_bitmap (PV_ARCH_TOP_RIGHT);
  pv_arch_top_right_end = load_bitmap (PV_ARCH_TOP_RIGHT_END);
  pv_arch_top_mid = load_bitmap (PV_ARCH_TOP_MID);
  pv_arch_top_small = load_bitmap (PV_ARCH_TOP_SMALL);
  pv_arch_top_top = load_bitmap (PV_ARCH_TOP_TOP);
}

void
unload_arch (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_arch_bottom);
  destroy_bitmap (dc_arch_top_left);
  destroy_bitmap (dc_arch_top_left_end);
  destroy_bitmap (dc_arch_top_right);
  destroy_bitmap (dc_arch_top_right_end);
  destroy_bitmap (dc_arch_top_mid);
  destroy_bitmap (dc_arch_top_small);
  destroy_bitmap (dc_arch_top_top);

  /* palace cga */
  destroy_bitmap (pc_arch_bottom);
  destroy_bitmap (pc_arch_top_left);
  destroy_bitmap (pc_arch_top_left_end);
  destroy_bitmap (pc_arch_top_right);
  destroy_bitmap (pc_arch_top_right_end);
  destroy_bitmap (pc_arch_top_mid);
  destroy_bitmap (pc_arch_top_small);
  destroy_bitmap (pc_arch_top_top);

  /* dungeon ega */
  destroy_bitmap (de_arch_bottom);
  destroy_bitmap (de_arch_top_left);
  destroy_bitmap (de_arch_top_left_end);
  destroy_bitmap (de_arch_top_right);
  destroy_bitmap (de_arch_top_right_end);
  destroy_bitmap (de_arch_top_mid);
  destroy_bitmap (de_arch_top_small);
  destroy_bitmap (de_arch_top_top);

  /* palace ega */
  destroy_bitmap (pe_arch_bottom);
  destroy_bitmap (pe_arch_top_left);
  destroy_bitmap (pe_arch_top_left_end);
  destroy_bitmap (pe_arch_top_right);
  destroy_bitmap (pe_arch_top_right_end);
  destroy_bitmap (pe_arch_top_mid);
  destroy_bitmap (pe_arch_top_small);
  destroy_bitmap (pe_arch_top_top);

  /* dungeon vga */
  destroy_bitmap (dv_arch_bottom);
  destroy_bitmap (dv_arch_top_left);
  destroy_bitmap (dv_arch_top_left_end);
  destroy_bitmap (dv_arch_top_right);
  destroy_bitmap (dv_arch_top_right_end);
  destroy_bitmap (dv_arch_top_mid);
  destroy_bitmap (dv_arch_top_small);
  destroy_bitmap (dv_arch_top_top);

  /* palace vga */
  destroy_bitmap (pv_arch_bottom);
  destroy_bitmap (pv_arch_top_left);
  destroy_bitmap (pv_arch_top_left_end);
  destroy_bitmap (pv_arch_top_right);
  destroy_bitmap (pv_arch_top_right_end);
  destroy_bitmap (pv_arch_top_mid);
  destroy_bitmap (pv_arch_top_small);
  destroy_bitmap (pv_arch_top_top);
}

void
draw_arch_bottom (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_bottom = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_bottom = dc_arch_bottom; break;
    case EGA: arch_bottom = de_arch_bottom; break;
    case VGA: arch_bottom = dv_arch_bottom; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_bottom = pc_arch_bottom; break;
    case EGA: arch_bottom = pe_arch_bottom; break;
    case VGA: arch_bottom = pv_arch_bottom; break;
    }
    break;
  }

  if (vm == VGA) arch_bottom = apply_hue_palette (arch_bottom);
  if (hgc) arch_bottom = apply_palette (arch_bottom, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_bottom = apply_palette (arch_bottom, selection_palette);

  struct coord c;
  draw_bitmapc (arch_bottom, bitmap, arch_bottom_coord (p, &c), 0);
}

void
draw_arch_bottom_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm)
{
  struct pos pa; prel (p, &pa, -1, +0);
  draw_arch_bottom (bitmap, p, em, vm);
  draw_confg_base (bitmap, &pa, em, vm);
}

void
draw_arch_top_mid (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_mid = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_mid = dc_arch_top_mid; break;
    case EGA: arch_top_mid = de_arch_top_mid; break;
    case VGA: arch_top_mid = dv_arch_top_mid; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_mid = pc_arch_top_mid; break;
    case EGA: arch_top_mid = pe_arch_top_mid; break;
    case VGA: arch_top_mid = pv_arch_top_mid; break;
    }
    break;
  }

  if (vm == VGA) arch_top_mid = apply_hue_palette (arch_top_mid);
  if (hgc) arch_top_mid = apply_palette (arch_top_mid, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_mid = apply_palette (arch_top_mid, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_mid, bitmap, arch_top_coord (p, &c), 0);
  draw_arch_top_top (bitmap, p, em, vm);
}

void
draw_arch_top_small (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_small = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_small = dc_arch_top_small; break;
    case EGA: arch_top_small = de_arch_top_small; break;
    case VGA: arch_top_small = dv_arch_top_small; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_small = pc_arch_top_small; break;
    case EGA: arch_top_small = pe_arch_top_small; break;
    case VGA: arch_top_small = pv_arch_top_small; break;
    }
    break;
  }

  if (vm == VGA) arch_top_small = apply_hue_palette (arch_top_small);
  if (hgc) arch_top_small = apply_palette (arch_top_small, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_small = apply_palette (arch_top_small, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_small, bitmap, arch_top_coord (p, &c), 0);
  draw_arch_top_top (bitmap, p, em, vm);
}

void
draw_arch_top_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_left = dc_arch_top_left; break;
    case EGA: arch_top_left = de_arch_top_left; break;
    case VGA: arch_top_left = dv_arch_top_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_left = pc_arch_top_left; break;
    case EGA: arch_top_left = pe_arch_top_left; break;
    case VGA: arch_top_left = pv_arch_top_left; break;
    }
    break;
  }

  if (vm == VGA) arch_top_left = apply_hue_palette (arch_top_left);
  if (hgc) arch_top_left = apply_palette (arch_top_left, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_left = apply_palette (arch_top_left, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_left, bitmap, arch_top_coord (p, &c), 0);
  draw_arch_top_top (bitmap, p, em, vm);
}

void
draw_arch_top_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_right = dc_arch_top_right; break;
    case EGA: arch_top_right = de_arch_top_right; break;
    case VGA: arch_top_right = dv_arch_top_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_right = pc_arch_top_right; break;
    case EGA: arch_top_right = pe_arch_top_right; break;
    case VGA: arch_top_right = pv_arch_top_right; break;
    }
    break;
  }

  if (vm == VGA) arch_top_right = apply_hue_palette (arch_top_right);
  if (hgc) arch_top_right = apply_palette (arch_top_right, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_right = apply_palette (arch_top_right, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_right, bitmap, arch_top_coord (p, &c), 0);
  draw_arch_top_top (bitmap, p, em, vm);
}

void
draw_arch_top_left_end (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_left_end = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_left_end = dc_arch_top_left_end; break;
    case EGA: arch_top_left_end = de_arch_top_left_end; break;
    case VGA: arch_top_left_end = dv_arch_top_left_end; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_left_end = pc_arch_top_left_end; break;
    case EGA: arch_top_left_end = pe_arch_top_left_end; break;
    case VGA: arch_top_left_end = pv_arch_top_left_end; break;
    }
    break;
  }

  if (vm == VGA) arch_top_left_end = apply_hue_palette (arch_top_left_end);
  if (hgc) arch_top_left_end = apply_palette (arch_top_left_end, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_left_end = apply_palette (arch_top_left_end, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_left_end, bitmap, arch_top_left_end_coord (p, &c), 0);
}

void
draw_arch_top_right_end (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_right_end = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_right_end = dc_arch_top_right_end; break;
    case EGA: arch_top_right_end = de_arch_top_right_end; break;
    case VGA: arch_top_right_end = dv_arch_top_right_end; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_right_end = pc_arch_top_right_end; break;
    case EGA: arch_top_right_end = pe_arch_top_right_end; break;
    case VGA: arch_top_right_end = pv_arch_top_right_end; break;
    }
    break;
  }

  if (vm == VGA) arch_top_right_end = apply_hue_palette (arch_top_right_end);
  if (hgc) arch_top_right_end = apply_palette (arch_top_right_end, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_right_end = apply_palette (arch_top_right_end, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_right_end, bitmap, arch_top_top_coord (p, &c), 0);
}

void
draw_arch_top_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *arch_top_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: arch_top_top = dc_arch_top_top; break;
    case EGA: arch_top_top = de_arch_top_top; break;
    case VGA: arch_top_top = dv_arch_top_top; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: arch_top_top = pc_arch_top_top; break;
    case EGA: arch_top_top = pe_arch_top_top; break;
    case VGA: arch_top_top = pv_arch_top_top; break;
    }
    break;
  }

  if (vm == VGA) arch_top_top = apply_hue_palette (arch_top_top);
  if (hgc) arch_top_top = apply_palette (arch_top_top, hgc_palette);
  if (peq (p, &mouse_pos))
    arch_top_top = apply_palette (arch_top_top, selection_palette);

  struct coord c;
  draw_bitmapc (arch_top_top, bitmap, arch_top_top_coord (p, &c), 0);
}

struct coord *
arch_bottom_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor;
  c->room = p->room;
  return c;
}

struct coord *
arch_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 10;
  c->room = p->room;
  return c;
}

struct coord *
arch_top_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
arch_top_left_end_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}
