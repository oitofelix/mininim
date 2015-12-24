/*
  opener-floor.c -- opener floor module;

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

#include <error.h>
#include <stdlib.h>
#include "kernel/audio.h"
#include "kernel/video.h"
#include "kernel/array.h"
#include "physics.h"
#include "room.h"
#include "floor.h"
#include "kid/kid.h"
#include "door.h"
#include "floor.h"
#include "opener-floor.h"

/* dungeon cga */
ALLEGRO_BITMAP *dc_unpressed_opener_floor_base,
  *dc_unpressed_opener_floor_left;

/* palace cga */
ALLEGRO_BITMAP *pc_unpressed_opener_floor_base,
  *pc_unpressed_opener_floor_left;

/* dungeon ega */
ALLEGRO_BITMAP *de_unpressed_opener_floor_base,
  *de_unpressed_opener_floor_left;

/* palace ega */
ALLEGRO_BITMAP *pe_unpressed_opener_floor_base,
  *pe_unpressed_opener_floor_left;

/* dungeon vga */
ALLEGRO_BITMAP *dv_unpressed_opener_floor_base,
  *dv_unpressed_opener_floor_left;

/* palace vga */
ALLEGRO_BITMAP *pv_unpressed_opener_floor_base,
  *pv_unpressed_opener_floor_left;

ALLEGRO_SAMPLE *opener_floor_sample;
bool sample_opener_floor;

struct opener_floor *opener_floor = NULL;
size_t opener_floor_nmemb = 0;

void
load_opener_floor (void)
{
  /* dungeon cga */
  dc_unpressed_opener_floor_base = load_bitmap (DC_UNPRESSED_OPENER_FLOOR_BASE);
  dc_unpressed_opener_floor_left = load_bitmap (DC_UNPRESSED_OPENER_FLOOR_LEFT);

  /* palace cga */
  pc_unpressed_opener_floor_base = load_bitmap (PC_UNPRESSED_OPENER_FLOOR_BASE);
  pc_unpressed_opener_floor_left = load_bitmap (PC_UNPRESSED_OPENER_FLOOR_LEFT);

  /* dungeon ega */
  de_unpressed_opener_floor_base = load_bitmap (DE_UNPRESSED_OPENER_FLOOR_BASE);
  de_unpressed_opener_floor_left = load_bitmap (DE_UNPRESSED_OPENER_FLOOR_LEFT);

  /* palace ega */
  pe_unpressed_opener_floor_base = load_bitmap (PE_UNPRESSED_OPENER_FLOOR_BASE);
  pe_unpressed_opener_floor_left = load_bitmap (PE_UNPRESSED_OPENER_FLOOR_LEFT);

  /* dungeon vga */
  dv_unpressed_opener_floor_base = load_bitmap (DV_UNPRESSED_OPENER_FLOOR_BASE);
  dv_unpressed_opener_floor_left = load_bitmap (DV_UNPRESSED_OPENER_FLOOR_LEFT);

  /* palace vga */
  pv_unpressed_opener_floor_base = load_bitmap (PV_UNPRESSED_OPENER_FLOOR_BASE);
  pv_unpressed_opener_floor_left = load_bitmap (PV_UNPRESSED_OPENER_FLOOR_LEFT);
}

void
unload_opener_floor (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_unpressed_opener_floor_base);
  al_destroy_bitmap (dc_unpressed_opener_floor_left);

  /* palace cga */
  al_destroy_bitmap (pc_unpressed_opener_floor_base);
  al_destroy_bitmap (pc_unpressed_opener_floor_left);

  /* dungeon ega */
  al_destroy_bitmap (de_unpressed_opener_floor_base);
  al_destroy_bitmap (de_unpressed_opener_floor_left);

  /* palace ega */
  al_destroy_bitmap (pe_unpressed_opener_floor_base);
  al_destroy_bitmap (pe_unpressed_opener_floor_left);

  /* dungeon vga */
  al_destroy_bitmap (dv_unpressed_opener_floor_base);
  al_destroy_bitmap (dv_unpressed_opener_floor_left);

  /* palace vga */
  al_destroy_bitmap (pv_unpressed_opener_floor_base);
  al_destroy_bitmap (pv_unpressed_opener_floor_left);
}

void
load_opener_floor_samples (void)
{
  opener_floor_sample = load_sample (OPENER_FLOOR_SAMPLE);
}

void
unload_opener_floor_samples (void)
{
  al_destroy_sample (opener_floor_sample);
}

void
register_opener_floor (struct pos *p)
{
  struct opener_floor o;

  o.p = *p;
  o.event = con (p)->ext.event;
  o.pressed = false;
  o.noise = false;
  o.broken = false;

  opener_floor =
    add_to_array (&o, 1, opener_floor, &opener_floor_nmemb,
                  opener_floor_nmemb, sizeof (o));

  qsort (opener_floor, opener_floor_nmemb, sizeof (o),
         compare_opener_floors);
}

int
compare_opener_floors (const void *o0, const void *o1)
{
  return cpos (&((struct opener_floor *) o0)->p,
               &((struct opener_floor *) o1)->p);
}

struct opener_floor *
opener_floor_at_pos (struct pos *p)
{
  struct opener_floor o;
  o.p = *p;

  return bsearch (&o, opener_floor, opener_floor_nmemb, sizeof (o),
                  compare_opener_floors);
}

void
press_opener_floor (struct pos *p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  o->pressed = true;
}

void
break_opener_floor (struct pos *p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  o->broken = true;
}

void
unpress_opener_floors (void)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++)
    opener_floor[i].pressed = false;
}

void
compute_opener_floors (void)
{
  size_t i;

  for (i = 0; i < opener_floor_nmemb; i++) {
    struct opener_floor *o = &opener_floor[i];
    if (o->p.room == -1) {
      /* remove_opener_floor (o); i--; */
      continue;
    }
    if (o->pressed || o->broken) {
      if (! o->noise) {
        sample_opener_floor = true;
        o->noise = true;
      }
      open_door (o->event);
    } else o->noise = false;
  }
}

void
sample_opener_floors (void)
{
  if (sample_opener_floor) play_sample (opener_floor_sample);
  sample_opener_floor = false;
}

void
draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_floor (bitmap, p, em, vm);
  else draw_unpressed_opener_floor (bitmap, p, em, vm);
}

void
draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_floor_base (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_base (bitmap, p, em, vm);
}

void
draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_floor_left (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_left (bitmap, p, em, vm);
}

void
draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_floor_right (bitmap, p, em, vm);
  else draw_unpressed_opener_floor_right (bitmap, p, em, vm);
}

void
draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p,
                             enum em em, enum vm vm)
{
  draw_unpressed_opener_floor_base (bitmap, p, em, vm);
  draw_unpressed_opener_floor_left (bitmap, p, em, vm);
  draw_unpressed_opener_floor_right (bitmap, p, em, vm);
}

void
draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_base = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_base = dc_unpressed_opener_floor_base; break;
    case EGA: unpressed_opener_floor_base = de_unpressed_opener_floor_base; break;
    case VGA: unpressed_opener_floor_base = dv_unpressed_opener_floor_base; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_base = pc_unpressed_opener_floor_base; break;
    case EGA: unpressed_opener_floor_base = pe_unpressed_opener_floor_base; break;
    case VGA: unpressed_opener_floor_base = pv_unpressed_opener_floor_base; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_base, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                  enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_left = dc_unpressed_opener_floor_left; break;
    case EGA: unpressed_opener_floor_left = de_unpressed_opener_floor_left; break;
    case VGA: unpressed_opener_floor_left = dv_unpressed_opener_floor_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_left = pc_unpressed_opener_floor_left; break;
    case EGA: unpressed_opener_floor_left = pe_unpressed_opener_floor_left; break;
    case VGA: unpressed_opener_floor_left = pv_unpressed_opener_floor_left; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_left, bitmap,
                unpressed_opener_floor_left_coord (p, &c), 0);
}

void
draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                   enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *unpressed_opener_floor_right = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: unpressed_opener_floor_right = dc_floor_right; break;
    case EGA: unpressed_opener_floor_right = de_floor_right; break;
    case VGA: unpressed_opener_floor_right = dv_floor_right; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: unpressed_opener_floor_right = pc_floor_right; break;
    case EGA: unpressed_opener_floor_right = pe_floor_right; break;
    case VGA: unpressed_opener_floor_right = pv_floor_right; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (unpressed_opener_floor_right, bitmap,
                unpressed_opener_floor_right_coord (p, &c), 0);
}

struct coord *
unpressed_opener_floor_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}

struct coord *
unpressed_opener_floor_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}
