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

#include <config.h>

#include <stdio.h>
#include "video.h"
#include "audio.h"
#include "array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid.h"
#include "level.h"
#include "arch.h"
#include "door.h"
#include "carpet.h"

static void get_carpet_bitmaps (struct pos *p, enum em em,
                                enum vm vm,
                                ALLEGRO_BITMAP **carpet,
                                ALLEGRO_BITMAP **carpet_top);

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
  al_destroy_bitmap (dc_carpet_00);
  al_destroy_bitmap (dc_carpet_top_00);
  al_destroy_bitmap (dc_carpet_01);
  al_destroy_bitmap (dc_carpet_top_01);

  /* palace cga */
  al_destroy_bitmap (pc_carpet_00);
  al_destroy_bitmap (pc_carpet_top_00);
  al_destroy_bitmap (pc_carpet_01);
  al_destroy_bitmap (pc_carpet_top_01);

  /* dungeon ega */
  al_destroy_bitmap (de_carpet_00);
  al_destroy_bitmap (de_carpet_top_00);
  al_destroy_bitmap (de_carpet_01);
  al_destroy_bitmap (de_carpet_top_01);

  /* palace ega */
  al_destroy_bitmap (pe_carpet_00);
  al_destroy_bitmap (pe_carpet_top_00);
  al_destroy_bitmap (pe_carpet_01);
  al_destroy_bitmap (pe_carpet_top_01);

  /* dungeon vga */
  al_destroy_bitmap (dv_carpet_00);
  al_destroy_bitmap (dv_carpet_top_00);
  al_destroy_bitmap (dv_carpet_01);
  al_destroy_bitmap (dv_carpet_top_01);

  /* palace vga */
  al_destroy_bitmap (pv_carpet_00);
  al_destroy_bitmap (pv_carpet_top_00);
  al_destroy_bitmap (pv_carpet_01);
  al_destroy_bitmap (pv_carpet_top_01);
}

static void
get_carpet_bitmaps (struct pos *p, enum em em, enum vm vm,
                    ALLEGRO_BITMAP **carpet,
                    ALLEGRO_BITMAP **carpet_top)
{
  ALLEGRO_BITMAP *carpet_00 = NULL,
    *carpet_top_00 = NULL,
    *carpet_01 = NULL,
    *carpet_top_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      carpet_00 = dc_carpet_00;
      carpet_top_00 = dc_carpet_top_00;
      carpet_01 = dc_carpet_01;
      carpet_top_01 = dc_carpet_top_01;
      break;
    case EGA:
      carpet_00 = de_carpet_00;
      carpet_top_00 = de_carpet_top_00;
      carpet_01 = de_carpet_01;
      carpet_top_01 = de_carpet_top_01;
      break;
    case VGA:
      carpet_00 = dv_carpet_00;
      carpet_top_00 = dv_carpet_top_00;
      carpet_01 = dv_carpet_01;
      carpet_top_01 = dv_carpet_top_01;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      carpet_00 = pc_carpet_00;
      carpet_top_00 = pc_carpet_top_00;
      carpet_01 = pc_carpet_01;
      carpet_top_01 = pc_carpet_top_01;
      break;
    case EGA:
      carpet_00 = pe_carpet_00;
      carpet_top_00 = pe_carpet_top_00;
      carpet_01 = pe_carpet_01;
      carpet_top_01 = pe_carpet_top_01;
      break;
    case VGA:
      carpet_00 = pv_carpet_00;
      carpet_top_00 = pv_carpet_top_00;
      carpet_01 = pv_carpet_01;
      carpet_top_01 = pv_carpet_top_01;
      break;
    }
    break;
  }

  switch (con (p)->ext.design) {
  case CARPET_00:
    *carpet = carpet_00;
    *carpet_top = carpet_top_00;
    break;
  case CARPET_01:
    *carpet = carpet_01;
    *carpet_top = carpet_top_01;
    break;
  case ARCH_CARPET_LEFT:
    *carpet = NULL;
    *carpet_top = NULL;
    break;
  case ARCH_CARPET_RIGHT_01:
    *carpet = carpet_00;
    *carpet_top = carpet_top_00;
    break;
  case ARCH_CARPET_RIGHT_02:
    *carpet = carpet_01;
    *carpet_top = carpet_top_01;
    break;
  }
}

void
draw_carpet_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL, *carpet_top = NULL;

  if (con (p)->ext.design == ARCH_CARPET_LEFT)
    draw_arch_top_left_end (bitmap, p, em, vm);
  else {
    get_carpet_bitmaps (p, em, vm, &carpet, &carpet_top);

    if (hgc) {
      carpet = apply_palette (carpet, hgc_palette);
      carpet_top = apply_palette (carpet_top, hgc_palette);
    }

    draw_bitmapc (carpet, bitmap, carpet_coord (p, &c), 0);
    draw_bitmapc (carpet_top, bitmap, carpet_top_coord (p, &c), 0);
  }
}

void
draw_carpet_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                struct frame *f, enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *carpet = NULL, *carpet_top = NULL;

  draw_door_pole (bitmap, p, em, vm);

  struct pos pr; prel (p, &pr, +0, +1);

  if (con (p)->ext.design == ARCH_CARPET_LEFT) {
    draw_arch_top_left_end (bitmap, p, em, vm);

    draw_confg_base (bitmap, &pr, em, vm);
    draw_confg_left (bitmap, &pr, em, vm, true);
  } else {
    get_carpet_bitmaps (p, em, vm, &carpet, &carpet_top);

    if (hgc) {
      carpet = apply_palette (carpet, hgc_palette);
      carpet_top = apply_palette (carpet_top, hgc_palette);
    }

    if (! should_draw_door_grid (p, f)) return;

    draw_bitmapc (carpet, bitmap, carpet_coord (p, &c), 0);

    draw_confg_base (bitmap, &pr, em, vm);
    draw_confg_left (bitmap, &pr, em, vm, true);
  }
}

struct coord *
carpet_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
carpet_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 9;
  c->room = p->room;
  return c;
}
