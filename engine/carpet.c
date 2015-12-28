/*
  carpet.c -- carpet module;

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
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "carpet.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_carpet_01, *dc_carpet_top_01,
  *dc_carpet_02, *dc_carpet_top_02;

/* palace cga */
ALLEGRO_BITMAP *pc_carpet_01, *pc_carpet_top_01,
  *pc_carpet_02, *pc_carpet_top_02;

/* dungeon ega */
ALLEGRO_BITMAP *de_carpet_01, *de_carpet_top_01,
  *de_carpet_02, *de_carpet_top_02;

/* palace ega */
ALLEGRO_BITMAP *pe_carpet_01, *pe_carpet_top_01,
  *pe_carpet_02, *pe_carpet_top_02;

/* dungeon vga */
ALLEGRO_BITMAP *dv_carpet_01, *dv_carpet_top_01,
  *dv_carpet_02, *dv_carpet_top_02;

/* palace vga */
ALLEGRO_BITMAP *pv_carpet_01, *pv_carpet_top_01,
  *pv_carpet_02, *pv_carpet_top_02;

void
load_carpet (void)
{
  /* dungeon cga */
  dc_carpet_01 = load_bitmap (DC_CARPET_01);
  dc_carpet_top_01 = load_bitmap (DC_CARPET_TOP_01);
  dc_carpet_02 = load_bitmap (DC_CARPET_02);
  dc_carpet_top_02 = load_bitmap (DC_CARPET_TOP_02);

  /* palace cga */
  pc_carpet_01 = load_bitmap (PC_CARPET_01);
  pc_carpet_top_01 = load_bitmap (PC_CARPET_TOP_01);
  pc_carpet_02 = load_bitmap (PC_CARPET_02);
  pc_carpet_top_02 = load_bitmap (PC_CARPET_TOP_02);

  /* dungeon ega */
  de_carpet_01 = load_bitmap (DE_CARPET_01);
  de_carpet_top_01 = load_bitmap (DE_CARPET_TOP_01);
  de_carpet_02 = load_bitmap (DE_CARPET_02);
  de_carpet_top_02 = load_bitmap (DE_CARPET_TOP_02);

  /* palace ega */
  pe_carpet_01 = load_bitmap (PE_CARPET_01);
  pe_carpet_top_01 = load_bitmap (PE_CARPET_TOP_01);
  pe_carpet_02 = load_bitmap (PE_CARPET_02);
  pe_carpet_top_02 = load_bitmap (PE_CARPET_TOP_02);

  /* dungeon vga */
  dv_carpet_01 = load_bitmap (DV_CARPET_01);
  dv_carpet_top_01 = load_bitmap (DV_CARPET_TOP_01);
  dv_carpet_02 = load_bitmap (DV_CARPET_02);
  dv_carpet_top_02 = load_bitmap (DV_CARPET_TOP_02);

  /* palace vga */
  pv_carpet_01 = load_bitmap (PV_CARPET_01);
  pv_carpet_top_01 = load_bitmap (PV_CARPET_TOP_01);
  pv_carpet_02 = load_bitmap (PV_CARPET_02);
  pv_carpet_top_02 = load_bitmap (PV_CARPET_TOP_02);
}

void
unload_carpet (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_carpet_01);
  al_destroy_bitmap (dc_carpet_top_01);
  al_destroy_bitmap (dc_carpet_02);
  al_destroy_bitmap (dc_carpet_top_02);

  /* palace cga */
  al_destroy_bitmap (pc_carpet_01);
  al_destroy_bitmap (pc_carpet_top_01);
  al_destroy_bitmap (pc_carpet_02);
  al_destroy_bitmap (pc_carpet_top_02);

  /* dungeon ega */
  al_destroy_bitmap (de_carpet_01);
  al_destroy_bitmap (de_carpet_top_01);
  al_destroy_bitmap (de_carpet_02);
  al_destroy_bitmap (de_carpet_top_02);

  /* palace ega */
  al_destroy_bitmap (pe_carpet_01);
  al_destroy_bitmap (pe_carpet_top_01);
  al_destroy_bitmap (pe_carpet_02);
  al_destroy_bitmap (pe_carpet_top_02);

  /* dungeon vga */
  al_destroy_bitmap (dv_carpet_01);
  al_destroy_bitmap (dv_carpet_top_01);
  al_destroy_bitmap (dv_carpet_02);
  al_destroy_bitmap (dv_carpet_top_02);

  /* palace vga */
  al_destroy_bitmap (pv_carpet_01);
  al_destroy_bitmap (pv_carpet_top_01);
  al_destroy_bitmap (pv_carpet_02);
  al_destroy_bitmap (pv_carpet_top_02);
}

void
draw_carpet_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL,
    *carpet_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet = dc_carpet_01;
      carpet_top = dc_carpet_top_01;
      break;
    case EGA:
      carpet = de_carpet_01;
      carpet_top = de_carpet_top_01;
      break;
    case VGA:
      carpet = dv_carpet_01;
      carpet_top = dv_carpet_top_01;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet = pc_carpet_01;
      carpet_top = pc_carpet_top_01;
      break;
    case EGA:
      carpet = pe_carpet_01;
      carpet_top = pe_carpet_top_01;
      break;
    case VGA:
      carpet = pv_carpet_01;
      carpet_top = pv_carpet_top_01;
      break;
    }
    break;
  }

  if (hgc) {
    carpet = apply_palette (carpet, hgc_palette);
    carpet_top = apply_palette (carpet_top, hgc_palette);
  }

  draw_bitmapc (carpet, bitmap,
                carpet_coord (p, &c), 0);
  draw_bitmapc (carpet_top, bitmap,
                carpet_top_coord (p, &c), 0);
}

void
draw_carpet_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   struct frame *f, enum em em, enum vm vm)
{
  struct pos ph; prel (p, &ph, -1, -1);
  if (is_kid_hanging_at_pos (f, &ph)) return;

  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL,
    *carpet_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet = dc_carpet_01;
      carpet_top = dc_carpet_top_01;
      break;
    case EGA:
      carpet = de_carpet_01;
      carpet_top = de_carpet_top_01;
      break;
    case VGA:
      carpet = dv_carpet_01;
      carpet_top = dv_carpet_top_01;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet = pc_carpet_01;
      carpet_top = pc_carpet_top_01;
      break;
    case EGA:
      carpet = pe_carpet_01;
      carpet_top = pe_carpet_top_01;
      break;
    case VGA:
      carpet = pv_carpet_01;
      carpet_top = pv_carpet_top_01;
      break;
    }
    break;
  }

  if (hgc) {
    carpet = apply_palette (carpet, hgc_palette);
    carpet_top = apply_palette (carpet_top, hgc_palette);
  }

  int h = al_get_bitmap_height (carpet);
  draw_bitmap_regionc (carpet, bitmap, 0, 0, 27, h, carpet_coord (p, &c), 0);
  h = al_get_bitmap_height (carpet_top);
  draw_bitmap_regionc (carpet_top, bitmap, 0, 0, 18, h, carpet_top_coord (p, &c), 0);
}

void
draw_carpet_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL,
    *carpet_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet = dc_carpet_02;
      carpet_top = dc_carpet_top_02;
      break;
    case EGA:
      carpet = de_carpet_02;
      carpet_top = de_carpet_top_02;
      break;
    case VGA:
      carpet = dv_carpet_02;
      carpet_top = dv_carpet_top_02;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet = pc_carpet_02;
      carpet_top = pc_carpet_top_02;
      break;
    case EGA:
      carpet = pe_carpet_02;
      carpet_top = pe_carpet_top_02;
      break;
    case VGA:
      carpet = pv_carpet_02;
      carpet_top = pv_carpet_top_02;
      break;
    }
    break;
  }

  if (hgc) {
    carpet = apply_palette (carpet, hgc_palette);
    carpet_top = apply_palette (carpet_top, hgc_palette);
  }

  draw_bitmapc (carpet, bitmap,
                carpet_coord (p, &c), 0);
  draw_bitmapc (carpet_top, bitmap,
                carpet_top_coord (p, &c), 0);
}

void
draw_carpet_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   struct frame *f, enum em em, enum vm vm)
{
  struct pos ph; prel (p, &ph, -1, -1);
  if (is_kid_hanging_at_pos (f, &ph)) return;

  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL,
    *carpet_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet = dc_carpet_02;
      carpet_top = dc_carpet_top_02;
      break;
    case EGA:
      carpet = de_carpet_02;
      carpet_top = de_carpet_top_02;
      break;
    case VGA:
      carpet = dv_carpet_02;
      carpet_top = dv_carpet_top_02;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet = pc_carpet_02;
      carpet_top = pc_carpet_top_02;
      break;
    case EGA:
      carpet = pe_carpet_02;
      carpet_top = pe_carpet_top_02;
      break;
    case VGA:
      carpet = pv_carpet_02;
      carpet_top = pv_carpet_top_02;
      break;
    }
    break;
  }

  if (hgc) {
    carpet = apply_palette (carpet, hgc_palette);
    carpet_top = apply_palette (carpet_top, hgc_palette);
  }

  int h = al_get_bitmap_height (carpet);
  draw_bitmap_regionc (carpet, bitmap, 0, 0, 27, h, carpet_coord (p, &c), 0);
  h = al_get_bitmap_height (carpet_top);
  draw_bitmap_regionc (carpet_top, bitmap, 0, 0, 18, h, carpet_top_coord (p, &c), 0);
}

struct coord *
carpet_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place - 8;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
carpet_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 9;
  c->room = p->room;
  return c;
}
