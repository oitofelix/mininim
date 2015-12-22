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

#include <stdio.h>
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/random.h"
#include "physics.h"
#include "kid/kid.h"
#include "pillar.h"
#include "level.h"
#include "room.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "spikes-floor.h"
#include "floor.h"

/* dungeon ega */
ALLEGRO_BITMAP *de_floor_base, *de_floor_left,
  *de_floor_right, *de_floor_corner_01,
  *de_floor_corner_02, *de_floor_corner_03;

/* palace ega */
ALLEGRO_BITMAP *pe_floor_base, *pe_floor_left,
  *pe_floor_right, *pe_floor_corner_01,
  *pe_floor_corner_02, *pe_floor_corner_03;

/* dungeon vga */
ALLEGRO_BITMAP *dv_floor_base, *dv_floor_left,
  *dv_floor_right, *dv_floor_corner_01,
  *dv_floor_corner_02, *dv_floor_corner_03;

/* palace vga */
ALLEGRO_BITMAP *pv_floor_base, *pv_floor_left,
  *pv_floor_right, *pv_floor_corner_01,
  *pv_floor_corner_02, *pv_floor_corner_03;

void
load_floor (void)
{
  /* dungeon ega */
  de_floor_base = load_bitmap (DE_FLOOR_BASE);
  de_floor_left = load_bitmap (DE_FLOOR_LEFT);
  de_floor_right = load_bitmap (DE_FLOOR_RIGHT);
  de_floor_corner_01 = load_bitmap (DE_FLOOR_CORNER_01);
  de_floor_corner_02 = load_bitmap (DE_FLOOR_CORNER_02);
  de_floor_corner_03 = load_bitmap (DE_FLOOR_CORNER_03);

  /* palace ega */
  pe_floor_base = load_bitmap (PE_FLOOR_BASE);
  pe_floor_left = load_bitmap (PE_FLOOR_LEFT);
  pe_floor_right = load_bitmap (PE_FLOOR_RIGHT);
  pe_floor_corner_01 = load_bitmap (PE_FLOOR_CORNER_01);
  pe_floor_corner_02 = load_bitmap (PE_FLOOR_CORNER_02);
  pe_floor_corner_03 = load_bitmap (PE_FLOOR_CORNER_03);

  /* dungeon vga */
  dv_floor_base = load_bitmap (DV_FLOOR_BASE);
  dv_floor_left = load_bitmap (DV_FLOOR_LEFT);
  dv_floor_right = load_bitmap (DV_FLOOR_RIGHT);
  dv_floor_corner_01 = load_bitmap (DV_FLOOR_CORNER_01);
  dv_floor_corner_02 = load_bitmap (DV_FLOOR_CORNER_02);
  dv_floor_corner_03 = load_bitmap (DV_FLOOR_CORNER_03);

  /* palace vga */
  pv_floor_base = load_bitmap (PV_FLOOR_BASE);
  pv_floor_left = load_bitmap (PV_FLOOR_LEFT);
  pv_floor_right = load_bitmap (PV_FLOOR_RIGHT);
  pv_floor_corner_01 = load_bitmap (PV_FLOOR_CORNER_01);
  pv_floor_corner_02 = load_bitmap (PV_FLOOR_CORNER_02);
  pv_floor_corner_03 = load_bitmap (PV_FLOOR_CORNER_03);
}

void
unload_floor (void)
{
  /* dungeon ega */
  al_destroy_bitmap (de_floor_left);
  al_destroy_bitmap (de_floor_right);
  al_destroy_bitmap (de_floor_base);
  al_destroy_bitmap (de_floor_corner_01);
  al_destroy_bitmap (de_floor_corner_02);
  al_destroy_bitmap (de_floor_corner_03);

  /* palace ega */
  al_destroy_bitmap (pe_floor_left);
  al_destroy_bitmap (pe_floor_right);
  al_destroy_bitmap (pe_floor_base);
  al_destroy_bitmap (pe_floor_corner_01);
  al_destroy_bitmap (pe_floor_corner_02);
  al_destroy_bitmap (pe_floor_corner_03);

  /* dungeon vga */
  al_destroy_bitmap (dv_floor_left);
  al_destroy_bitmap (dv_floor_right);
  al_destroy_bitmap (dv_floor_base);
  al_destroy_bitmap (dv_floor_corner_01);
  al_destroy_bitmap (dv_floor_corner_02);
  al_destroy_bitmap (dv_floor_corner_03);

  /* palace vga */
  al_destroy_bitmap (pv_floor_left);
  al_destroy_bitmap (pv_floor_right);
  al_destroy_bitmap (pv_floor_base);
  al_destroy_bitmap (pv_floor_corner_01);
  al_destroy_bitmap (pv_floor_corner_02);
  al_destroy_bitmap (pv_floor_corner_03);
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
    case CGA: break;
    case EGA: floor_base = de_floor_base; break;
    case VGA: floor_base = dv_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_base = pe_floor_base; break;
    case VGA: floor_base = pv_floor_base; break;
    }
    break;
  }

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
    case CGA: break;
    case EGA: floor_left = de_floor_left; break;
    case VGA: floor_left = dv_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_left = pe_floor_left; break;
    case VGA: floor_left = pv_floor_left; break;
    }
    break;
  }

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
    case CGA: break;
    case EGA: floor_right = de_floor_right; break;
    case VGA: floor_right = dv_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_right = pe_floor_right; break;
    case VGA: floor_right = pv_floor_right; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (floor_right, bitmap, floor_right_coord (p, &c), 0);
}

void
draw_floor_corner_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_corner_01 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: floor_corner_01 = de_floor_corner_01; break;
    case VGA: floor_corner_01 = dv_floor_corner_01; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_corner_01 = pe_floor_corner_01; break;
    case VGA: floor_corner_01 = pv_floor_corner_01; break;
    }
    break;
  }

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
    case CGA: break;
    case EGA: floor_corner_02 = de_floor_corner_02; break;
    case VGA: floor_corner_02 = dv_floor_corner_02; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_corner_02 = pe_floor_corner_02; break;
    case VGA: floor_corner_02 = pv_floor_corner_02; break;
    }
    break;
  }

  struct coord c; floor_corner_02_coord (p, &c);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_02, bitmap, &c, 0);
}

void
draw_floor_corner_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *floor_corner_03 = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: floor_corner_03 = de_floor_corner_03; break;
    case VGA: floor_corner_03 = dv_floor_corner_03; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: floor_corner_03 = pe_floor_corner_03; break;
    case VGA: floor_corner_03 = pv_floor_corner_03; break;
    }
    break;
  }

  struct coord c; floor_corner_03_coord (p, &c);
  if (con (p)->fg == CLOSER_FLOOR) c.y += 1;
  draw_bitmapc (floor_corner_03, bitmap, &c, 0);
}

struct coord *
floor_base_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * (p->floor + 1);
  c->room = p->room;
  return c;
}

struct coord *
floor_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 50;
  c->room = p->room;
  return c;
}

struct coord *
floor_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 50;
  c->room = p->room;
  return c;
}

struct coord *
floor_corner_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 53;
  c->room = p->room;
  return c;
}

struct coord *
floor_corner_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 53;
  c->room = p->room;
  return c;
}

struct coord *
floor_corner_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 55;
  c->room = p->room;
  return c;
}
