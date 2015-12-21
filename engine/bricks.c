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
#include "bricks.h"

/* dungeon ega */
ALLEGRO_BITMAP *de_bricks_01, *de_bricks_02,
  *de_bricks_03, *de_bricks_04;

/* dungeon vga */
ALLEGRO_BITMAP *dv_bricks_01, *dv_bricks_02,
  *dv_bricks_03, *dv_bricks_04;

/* palace vga */
ALLEGRO_BITMAP *pv_bricks_01, *pv_bricks_02,
  *pv_bricks_03, *pv_bricks_04;

void
load_bricks (void)
{
  /* dungeon ega */
  de_bricks_01 = load_bitmap (DE_BRICKS_01);
  de_bricks_02 = load_bitmap (DE_BRICKS_02);
  de_bricks_03 = load_bitmap (DE_BRICKS_03);
  de_bricks_04 = load_bitmap (DE_BRICKS_04);

  /* dungeon vga */
  dv_bricks_01 = load_bitmap (DV_BRICKS_01);
  dv_bricks_02 = load_bitmap (DV_BRICKS_02);
  dv_bricks_03 = load_bitmap (DV_BRICKS_03);
  dv_bricks_04 = load_bitmap (DV_BRICKS_04);

  /* palace vga */
  pv_bricks_01 = load_bitmap (PV_BRICKS_01);
  pv_bricks_02 = load_bitmap (PV_BRICKS_02);
  pv_bricks_03 = load_bitmap (PV_BRICKS_03);
  pv_bricks_04 = load_bitmap (PV_BRICKS_04);
}

void
unload_bricks (void)
{
  /* dungeon ega */
  al_destroy_bitmap (de_bricks_01);
  al_destroy_bitmap (de_bricks_02);
  al_destroy_bitmap (de_bricks_03);
  al_destroy_bitmap (de_bricks_04);

  /* dungeon vga */
  al_destroy_bitmap (dv_bricks_01);
  al_destroy_bitmap (dv_bricks_02);
  al_destroy_bitmap (dv_bricks_03);
  al_destroy_bitmap (dv_bricks_04);

  /* palace vga */
  al_destroy_bitmap (pv_bricks_01);
  al_destroy_bitmap (pv_bricks_02);
  al_destroy_bitmap (pv_bricks_03);
  al_destroy_bitmap (pv_bricks_04);
}

void
draw_bricks_01 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord_01 = NULL;
  ALLEGRO_BITMAP *bricks_01 = NULL;

  switch (em) {
  case DUNGEON:
    bricks_coord_01 = d_bricks_coord_01;
    switch (vm) {
    case CGA: break;
    case EGA: bricks_01 = de_bricks_01; break;
    case VGA: bricks_01 = dv_bricks_01; break;
    }
    break;
  case PALACE:
    bricks_coord_01 = p_bricks_coord_01;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: bricks_01 = pv_bricks_01; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (bricks_01, bitmap, bricks_coord_01 (p, &c) , 0);
}

void
draw_bricks_02 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord_02 = NULL;
  ALLEGRO_BITMAP *bricks_02 = NULL;

  switch (em) {
  case DUNGEON:
    bricks_coord_02 = d_bricks_coord_02;
    switch (vm) {
    case CGA: break;
    case EGA: bricks_02 = de_bricks_02; break;
    case VGA: bricks_02 = dv_bricks_02; break;
    }
    break;
  case PALACE:
    bricks_coord_02 = p_bricks_coord_02;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: bricks_02 = pv_bricks_02; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (bricks_02, bitmap, bricks_coord_02 (p, &c) , 0);
}

void
draw_bricks_03 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord_03 = NULL;
  ALLEGRO_BITMAP *bricks_03 = NULL;

  switch (em) {
  case DUNGEON:
    bricks_coord_03 = d_bricks_coord_03;
    switch (vm) {
    case CGA: break;
    case EGA: bricks_03 = de_bricks_03; break;
    case VGA: bricks_03 = dv_bricks_03; break;
    }
    break;
  case PALACE:
    bricks_coord_03 = p_bricks_coord_03;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: bricks_03 = pv_bricks_03; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (bricks_03, bitmap, bricks_coord_03 (p, &c) , 0);
}

void
draw_bricks_04 (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  pos2coord_f bricks_coord_04 = NULL;
  ALLEGRO_BITMAP *bricks_04 = NULL;

  switch (em) {
  case DUNGEON:
    bricks_coord_04 = d_bricks_coord_04;
    switch (vm) {
    case CGA: break;
    case EGA: bricks_04 = de_bricks_04; break;
    case VGA: bricks_04 = dv_bricks_04; break;
    }
    break;
  case PALACE:
    bricks_coord_04 = p_bricks_coord_04;
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: bricks_04 = pv_bricks_04; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (bricks_04, bitmap, bricks_coord_04 (p, &c) , 0);
}

struct coord *
d_bricks_coord_01 (struct pos *p, struct coord *c)
{
  int h = al_get_bitmap_height (dv_bricks_01);
  int dy = prandom_pos (p, 49 - h);
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + dy;
  c->room = p->room;
  return c;
}

struct coord *
p_bricks_coord_01 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 29;
  c->room = p->room;
  return c;
}

struct coord *
d_bricks_coord_02 (struct pos *p, struct coord *c)
{
  int h = al_get_bitmap_height (dv_bricks_02);
  int dy = prandom_pos (p, 49 - h);
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + dy;
  c->room = p->room;
  return c;
}

struct coord *
p_bricks_coord_02 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 26;
  c->room = p->room;
  return c;
}

struct coord *
d_bricks_coord_03 (struct pos *p, struct coord *c)
{
  int h = al_get_bitmap_height (dv_bricks_03);
  int dy = prandom_pos (p, 49 - h);
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + dy;
  c->room = p->room;
  return c;
}

struct coord *
p_bricks_coord_03 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 29;
  c->room = p->room;
  return c;
}

struct coord *
d_bricks_coord_04 (struct pos *p, struct coord *c)
{
  int h = al_get_bitmap_height (dv_bricks_04);
  int dy = prandom_pos (p, 49 - h);
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + dy;
  c->room = p->room;
  return c;
}

struct coord *
p_bricks_coord_04 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 26;
  c->room = p->room;
  return c;
}
