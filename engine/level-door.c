/*
  level-door.c -- level door module;

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
#include "kernel/audio.h"
#include "kernel/array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "level-door.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_level_door_top_left, *dc_level_door_top_right,
  *dc_level_door_bottom, *dc_level_door_floor, *dc_level_door_stairs,
  *dc_level_door_front;

/* palace cga */
ALLEGRO_BITMAP *pc_level_door_top_left, *pc_level_door_top_right,
  *pc_level_door_bottom, *pc_level_door_floor, *pc_level_door_stairs,
  *pc_level_door_front;

/* dungeon ega */
ALLEGRO_BITMAP *de_level_door_top_left, *de_level_door_top_right,
  *de_level_door_bottom, *de_level_door_floor, *de_level_door_stairs,
  *de_level_door_front;

/* palace ega */
ALLEGRO_BITMAP *pe_level_door_top_left, *pe_level_door_top_right,
  *pe_level_door_bottom, *pe_level_door_floor, *pe_level_door_stairs,
  *pe_level_door_front;

/* dungeon vga */
ALLEGRO_BITMAP *dv_level_door_top_left, *dv_level_door_top_right,
  *dv_level_door_bottom, *dv_level_door_floor, *dv_level_door_stairs,
  *dv_level_door_front;

/* palace vga */
ALLEGRO_BITMAP *pv_level_door_top_left, *pv_level_door_top_right,
  *pv_level_door_bottom, *pv_level_door_floor, *pv_level_door_stairs,
  *pv_level_door_front;

ALLEGRO_SAMPLE *level_door_open_sample, *level_door_close_sample;

struct level_door *level_door = NULL;
size_t level_door_nmemb = 0;

void
load_level_door (void)
{
  /* dungeon cga */
  dc_level_door_top_left = load_bitmap (DC_LEVEL_DOOR_TOP_LEFT);
  dc_level_door_top_right = load_bitmap (DC_LEVEL_DOOR_TOP_RIGHT);
  dc_level_door_bottom = load_bitmap (DC_LEVEL_DOOR_BOTTOM);
  dc_level_door_floor = load_bitmap (DC_LEVEL_DOOR_FLOOR);
  dc_level_door_stairs = load_bitmap (DC_LEVEL_DOOR_STAIRS);
  dc_level_door_front = load_bitmap (DC_LEVEL_DOOR_FRONT);

  /* palace cga */
  pc_level_door_top_left = load_bitmap (PC_LEVEL_DOOR_TOP_LEFT);
  pc_level_door_top_right = load_bitmap (PC_LEVEL_DOOR_TOP_RIGHT);
  pc_level_door_bottom = load_bitmap (PC_LEVEL_DOOR_BOTTOM);
  pc_level_door_floor = load_bitmap (PC_LEVEL_DOOR_FLOOR);
  pc_level_door_stairs = load_bitmap (PC_LEVEL_DOOR_STAIRS);
  pc_level_door_front = load_bitmap (PC_LEVEL_DOOR_FRONT);

  /* dungeon ega */
  de_level_door_top_left = load_bitmap (DE_LEVEL_DOOR_TOP_LEFT);
  de_level_door_top_right = load_bitmap (DE_LEVEL_DOOR_TOP_RIGHT);
  de_level_door_bottom = load_bitmap (DE_LEVEL_DOOR_BOTTOM);
  de_level_door_floor = load_bitmap (DE_LEVEL_DOOR_FLOOR);
  de_level_door_stairs = load_bitmap (DE_LEVEL_DOOR_STAIRS);
  de_level_door_front = load_bitmap (DE_LEVEL_DOOR_FRONT);

  /* palace ega */
  pe_level_door_top_left = load_bitmap (PE_LEVEL_DOOR_TOP_LEFT);
  pe_level_door_top_right = load_bitmap (PE_LEVEL_DOOR_TOP_RIGHT);
  pe_level_door_bottom = load_bitmap (PE_LEVEL_DOOR_BOTTOM);
  pe_level_door_floor = load_bitmap (PE_LEVEL_DOOR_FLOOR);
  pe_level_door_stairs = load_bitmap (PE_LEVEL_DOOR_STAIRS);
  pe_level_door_front = load_bitmap (PE_LEVEL_DOOR_FRONT);

  /* dungeon vga */
  dv_level_door_top_left = load_bitmap (DV_LEVEL_DOOR_TOP_LEFT);
  dv_level_door_top_right = load_bitmap (DV_LEVEL_DOOR_TOP_RIGHT);
  dv_level_door_bottom = load_bitmap (DV_LEVEL_DOOR_BOTTOM);
  dv_level_door_floor = load_bitmap (DV_LEVEL_DOOR_FLOOR);
  dv_level_door_stairs = load_bitmap (DV_LEVEL_DOOR_STAIRS);
  dv_level_door_front = load_bitmap (DV_LEVEL_DOOR_FRONT);

  /* palace vga */
  pv_level_door_top_left = load_bitmap (PV_LEVEL_DOOR_TOP_LEFT);
  pv_level_door_top_right = load_bitmap (PV_LEVEL_DOOR_TOP_RIGHT);
  pv_level_door_bottom = load_bitmap (PV_LEVEL_DOOR_BOTTOM);
  pv_level_door_floor = load_bitmap (PV_LEVEL_DOOR_FLOOR);
  pv_level_door_stairs = load_bitmap (PV_LEVEL_DOOR_STAIRS);
  pv_level_door_front = load_bitmap (PV_LEVEL_DOOR_FRONT);
}

void
unload_level_door (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_level_door_top_left);
  al_destroy_bitmap (dc_level_door_top_right);
  al_destroy_bitmap (dc_level_door_bottom);
  al_destroy_bitmap (dc_level_door_floor);
  al_destroy_bitmap (dc_level_door_stairs);
  al_destroy_bitmap (dc_level_door_front);

  /* palace cga */
  al_destroy_bitmap (pc_level_door_top_left);
  al_destroy_bitmap (pc_level_door_top_right);
  al_destroy_bitmap (pc_level_door_bottom);
  al_destroy_bitmap (pc_level_door_floor);
  al_destroy_bitmap (pc_level_door_stairs);
  al_destroy_bitmap (pc_level_door_front);

  /* dungeon ega */
  al_destroy_bitmap (de_level_door_top_left);
  al_destroy_bitmap (de_level_door_top_right);
  al_destroy_bitmap (de_level_door_bottom);
  al_destroy_bitmap (de_level_door_floor);
  al_destroy_bitmap (de_level_door_stairs);
  al_destroy_bitmap (de_level_door_front);

  /* palace ega */
  al_destroy_bitmap (pe_level_door_top_left);
  al_destroy_bitmap (pe_level_door_top_right);
  al_destroy_bitmap (pe_level_door_bottom);
  al_destroy_bitmap (pe_level_door_floor);
  al_destroy_bitmap (pe_level_door_stairs);
  al_destroy_bitmap (pe_level_door_front);

  /* dungeon vga */
  al_destroy_bitmap (dv_level_door_top_left);
  al_destroy_bitmap (dv_level_door_top_right);
  al_destroy_bitmap (dv_level_door_bottom);
  al_destroy_bitmap (dv_level_door_floor);
  al_destroy_bitmap (dv_level_door_stairs);
  al_destroy_bitmap (dv_level_door_front);

  /* palace vga */
  al_destroy_bitmap (pv_level_door_top_left);
  al_destroy_bitmap (pv_level_door_top_right);
  al_destroy_bitmap (pv_level_door_bottom);
  al_destroy_bitmap (pv_level_door_floor);
  al_destroy_bitmap (pv_level_door_stairs);
  al_destroy_bitmap (pv_level_door_front);
}

void
load_level_door_samples (void)
{
  level_door_open_sample = load_sample (LEVEL_DOOR_OPEN_SAMPLE);
  level_door_close_sample = load_sample (LEVEL_DOOR_CLOSE_SAMPLE);
}

void
unload_level_door_samples (void)
{
  al_destroy_sample (level_door_open_sample);
  al_destroy_sample (level_door_close_sample);
}

void
register_level_door (struct pos *p)
{
  struct level_door d;

  d.p = *p;
  d.i = con (p)->ext.step;
  d.action = NO_LEVEL_DOOR_ACTION;

  level_door =
    add_to_array (&d, 1, level_door, &level_door_nmemb, level_door_nmemb, sizeof (d));

  qsort (level_door, level_door_nmemb, sizeof (d), compare_level_doors);
}

int
compare_level_doors (const void *d0, const void *d1)
{
  return cpos (&((struct level_door *) d0)->p, &((struct level_door *) d1)->p);
}

struct level_door *
level_door_at_pos (struct pos *p)
{
  struct level_door d;
  d.p = *p;

  return bsearch (&d, level_door, level_door_nmemb, sizeof (d), compare_level_doors);
}

void
compute_level_doors (void)
{
  size_t i;

  for (i = 0; i < level_door_nmemb; i++) {
    struct level_door *d = &level_door[i];
    if (d->p.room == -1) {
      /* remove_level_door (d); i--; */
      continue;
    }
    switch (d->action) {
    case OPEN_LEVEL_DOOR:
      if (d->i % 5 == 2 && d->i > 2) play_sample (level_door_open_sample, d->p.room);
      if (d->i > 0) d->i--;
      else d->action = NO_LEVEL_DOOR_ACTION;
      break;
    case CLOSE_LEVEL_DOOR:
      if (d->i == 0) play_sample (level_door_open_sample, d->p.room);
      if (d->i < LEVEL_DOOR_MAX_STEP) {
        int r = 14 - (d->i % 15);
        d->i += r ? r : 15;
        if (d->i >= LEVEL_DOOR_MAX_STEP) {
          d->i = LEVEL_DOOR_MAX_STEP;
          d->action = NO_LEVEL_DOOR_ACTION;
          play_sample (level_door_close_sample, d->p.room);
        }
      }
      break;
    default:
      break;
    }
  }
}

void
draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;
  struct level_door *d = level_door_at_pos (p);
  if (! d) return;

  pos2coord_f level_door_top_left_coord = NULL,
    level_door_top_right_coord = NULL,
    level_door_bottom_left_coord = NULL,
    level_door_bottom_right_coord = NULL;

  ALLEGRO_BITMAP *level_door_bottom = NULL,
    *level_door_top_left = NULL,
    *level_door_floor = NULL,
    *level_door_stairs = NULL,
    *level_door_top_right = NULL;

  switch (em) {
  case DUNGEON:
    level_door_top_left_coord = d_level_door_top_left_coord;
    level_door_top_right_coord = d_level_door_top_right_coord;
    level_door_bottom_left_coord = d_level_door_bottom_left_coord;
    level_door_bottom_right_coord = d_level_door_bottom_right_coord;
    switch (vm) {
    case CGA:
      level_door_bottom = dc_level_door_bottom;
      level_door_top_left = dc_level_door_top_left;
      level_door_floor = dc_level_door_floor;
      level_door_stairs = dc_level_door_stairs;
      level_door_top_right = dc_level_door_top_right;
      break;
    case EGA:
      level_door_bottom = de_level_door_bottom;
      level_door_top_left = de_level_door_top_left;
      level_door_floor = de_level_door_floor;
      level_door_stairs = de_level_door_stairs;
      level_door_top_right = de_level_door_top_right;
      break;
    case VGA:
      level_door_bottom = dv_level_door_bottom;
      level_door_top_left = dv_level_door_top_left;
      level_door_floor = dv_level_door_floor;
      level_door_stairs = dv_level_door_stairs;
      level_door_top_right = dv_level_door_top_right;
      break;
    }
    break;
  case PALACE:
    level_door_top_left_coord = p_level_door_top_left_coord;
    level_door_top_right_coord = p_level_door_top_right_coord;
    level_door_bottom_left_coord = p_level_door_bottom_left_coord;
    level_door_bottom_right_coord = p_level_door_bottom_right_coord;
    switch (vm) {
    case CGA:
      level_door_bottom = pc_level_door_bottom;
      level_door_top_left = pc_level_door_top_left;
      level_door_floor = pc_level_door_floor;
      level_door_stairs = pc_level_door_stairs;
      level_door_top_right = pc_level_door_top_right;
      break;
    case EGA:
      level_door_bottom = pe_level_door_bottom;
      level_door_top_left = pe_level_door_top_left;
      level_door_floor = pe_level_door_floor;
      level_door_stairs = pe_level_door_stairs;
      level_door_top_right = pe_level_door_top_right;
      break;
    case VGA:
      level_door_bottom = pv_level_door_bottom;
      level_door_top_left = pv_level_door_top_left;
      level_door_floor = pv_level_door_floor;
      level_door_stairs = pv_level_door_stairs;
      level_door_top_right = pv_level_door_top_right;
      break;
    }
    break;
  }

  if (hgc) {
    level_door_bottom = apply_palette (level_door_bottom, hgc_palette);
    level_door_top_left = apply_palette (level_door_top_left, hgc_palette);
    level_door_floor = apply_palette (level_door_floor, hgc_palette);
    level_door_stairs = apply_palette (level_door_stairs, hgc_palette);
    level_door_top_right = apply_palette (level_door_top_right, hgc_palette);
  }

  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_left_coord (p, &c), 0);
  draw_bitmapc (level_door_top_left, bitmap,
                level_door_top_left_coord (p, &c), 0);
  draw_bitmapc (level_door_floor, bitmap,
                level_door_floor_coord (p, &c), 0);
  draw_bitmapc (level_door_stairs, bitmap,
                level_door_stairs_coord (p, &c), 0);
  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_right_coord (p, &c), 0);
  draw_bitmapc (level_door_top_right, bitmap,
                level_door_top_right_coord (p, &c), 0);
  draw_level_door_front (bitmap, p, d->i, em, vm);
}

void
draw_level_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                    enum em em, enum vm vm)
{
  struct pos pl, pr, pa, par;

  pos2coord_f level_door_bottom_right_coord = NULL;

  ALLEGRO_BITMAP *level_door_bottom = NULL;

  switch (em) {
  case DUNGEON:
    level_door_bottom_right_coord = d_level_door_bottom_right_coord;
    switch (vm) {
    case CGA: level_door_bottom = dc_level_door_bottom; break;
    case EGA: level_door_bottom = de_level_door_bottom; break;
    case VGA: level_door_bottom = dv_level_door_bottom; break;
    }
    break;
  case PALACE:
    level_door_bottom_right_coord = p_level_door_bottom_right_coord;
    switch (vm) {
    case CGA: level_door_bottom = pc_level_door_bottom; break;
    case EGA: level_door_bottom = pe_level_door_bottom; break;
    case VGA: level_door_bottom = pv_level_door_bottom; break;
    }
    break;
  }

  if (hgc) level_door_bottom = apply_palette (level_door_bottom, hgc_palette);

  if (! is_kid_stairs (f)) return;
  prel (p, &pl, +0, -1);
  prel (p, &pr, +0, +1);
  prel (p, &pa, -1, +0);
  prel (p, &par, -1, +1);

  struct coord c;
  draw_bitmapc (level_door_bottom, bitmap,
                level_door_bottom_right_coord (p, &c), 0);
  draw_level_door_front (bitmap, p, 0, em, vm);
  /* draw_confg (bitmap, &pl, em, vm, true); */
  draw_confg (bitmap, &pr, em, vm, true);
  draw_confg (bitmap, &pa, em, vm, true);
  draw_confg_base (bitmap, &par, em, vm);
  draw_confg_left (bitmap, &par, em, vm, true);
}

void
draw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                       enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *level_door_front = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: level_door_front = dc_level_door_front; break;
    case EGA: level_door_front = de_level_door_front; break;
    case VGA: level_door_front = dv_level_door_front; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: level_door_front = pc_level_door_front; break;
    case EGA: level_door_front = pe_level_door_front; break;
    case VGA: level_door_front = pv_level_door_front; break;
    }
    break;
  }

  if (hgc) level_door_front = apply_palette (level_door_front, hgc_palette);

  int q = (i + 1) / 4;
  int r = (i + 1) % 4;
  int w = al_get_bitmap_width (level_door_front);
  int j;

  struct coord c;
  draw_bitmap_regionc (level_door_front, bitmap, 0, 3 - r, w, r + 1,
                       level_door_front_coord_base (p, &c), 0);
  for (j = 0; j <= q; j++)
    draw_bitmapc (level_door_front, bitmap, level_door_front_coord (p, &c, j, i), 0);
}

struct coord *
level_door_front_coord_base (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor - 1;
  c->room = p->room;
  return c;
}

struct coord *
level_door_front_coord (struct pos *p, struct coord *c, int j, int i)
{
  int r = (i + 1) % 4;
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor - 1 + j * 4 + r + 1;
  c->room = p->room;
  return c;
}

struct coord *
d_level_door_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 13;
  c->room = p->room;
  return c;
}

struct coord *
p_level_door_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 10;
  c->room = p->room;
  return c;
}

struct coord *
d_level_door_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 13;
  c->room = p->room;
  return c;
}

struct coord *
p_level_door_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 10;
  c->room = p->room;
  return c;
}

struct coord *
d_level_door_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 1;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
p_level_door_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place - 1;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
d_level_door_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) + 17;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
p_level_door_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) + 23;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
level_door_floor_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 46;
  c->room = p->room;
  return c;
}

struct coord *
level_door_stairs_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 8;
  c->y = PLACE_HEIGHT * p->floor + 1;
  c->room = p->room;
  return c;
}
