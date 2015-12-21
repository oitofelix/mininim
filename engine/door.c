/*
  door.c -- door module;

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
#include "kernel/audio.h"
#include "kernel/array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "door.h"
#include "level-door.h"

/* dungeon ega */
ALLEGRO_BITMAP *de_door_left, *de_door_right, *de_door_pole,
  *de_door_top, *de_door_grid, *de_door_grid_tip;

/* dungeon vga */
ALLEGRO_BITMAP *dv_door_left, *dv_door_right, *dv_door_pole,
  *dv_door_top, *dv_door_grid, *dv_door_grid_tip;

/* palace vga */
ALLEGRO_BITMAP *pv_door_left, *pv_door_right, *pv_door_pole,
  *pv_door_top, *pv_door_grid, *pv_door_grid_tip;

ALLEGRO_SAMPLE *door_open_sample, *door_close_sample, *door_end_sample,
  *door_abruptly_close_sample;

bool sample_door_open, sample_door_close, sample_door_end,
  sample_door_abruptly_close;

static struct door *door = NULL;
static size_t door_nmemb = 0;

void
load_door (void)
{
  /* dungeon ega */
  de_door_left = load_bitmap (DE_DOOR_LEFT);
  de_door_right = load_bitmap (DE_DOOR_RIGHT);
  de_door_pole = load_bitmap (DE_DOOR_POLE);
  de_door_top = load_bitmap (DE_DOOR_TOP);
  de_door_grid = load_bitmap (DE_DOOR_GRID);
  de_door_grid_tip = load_bitmap (DE_DOOR_GRID_TIP);

  /* dungeon vga */
  dv_door_left = load_bitmap (DV_DOOR_LEFT);
  dv_door_right = load_bitmap (DV_DOOR_RIGHT);
  dv_door_pole = load_bitmap (DV_DOOR_POLE);
  dv_door_top = load_bitmap (DV_DOOR_TOP);
  dv_door_grid = load_bitmap (DV_DOOR_GRID);
  dv_door_grid_tip = load_bitmap (DV_DOOR_GRID_TIP);

  /* palace vga */
  pv_door_left = load_bitmap (PV_DOOR_LEFT);
  pv_door_right = load_bitmap (PV_DOOR_RIGHT);
  pv_door_pole = load_bitmap (PV_DOOR_POLE);
  pv_door_top = load_bitmap (PV_DOOR_TOP);
  pv_door_grid = load_bitmap (PV_DOOR_GRID);
  pv_door_grid_tip = load_bitmap (PV_DOOR_GRID_TIP);
}

void
unload_door (void)
{
  /* dungeon ega */
  al_destroy_bitmap (de_door_left);
  al_destroy_bitmap (de_door_right);
  al_destroy_bitmap (de_door_pole);
  al_destroy_bitmap (de_door_top);
  al_destroy_bitmap (de_door_grid);
  al_destroy_bitmap (de_door_grid_tip);

  /* dungeon vga */
  al_destroy_bitmap (dv_door_left);
  al_destroy_bitmap (dv_door_right);
  al_destroy_bitmap (dv_door_pole);
  al_destroy_bitmap (dv_door_top);
  al_destroy_bitmap (dv_door_grid);
  al_destroy_bitmap (dv_door_grid_tip);

  /* palace vga */
  al_destroy_bitmap (pv_door_left);
  al_destroy_bitmap (pv_door_right);
  al_destroy_bitmap (pv_door_pole);
  al_destroy_bitmap (pv_door_top);
  al_destroy_bitmap (pv_door_grid);
  al_destroy_bitmap (pv_door_grid_tip);
}

void
load_door_samples (void)
{
  door_open_sample = load_sample (DOOR_OPEN_SAMPLE);
  door_close_sample = load_sample (DOOR_CLOSE_SAMPLE);
  door_end_sample = load_sample (DOOR_END_SAMPLE);
  door_abruptly_close_sample = load_sample (DOOR_ABRUPTLY_CLOSE_SAMPLE);
}

void
unload_door_samples (void)
{
  al_destroy_sample (door_open_sample);
  al_destroy_sample (door_close_sample);
  al_destroy_sample (door_end_sample);
  al_destroy_sample (door_abruptly_close_sample);
}

void
register_door (struct pos *p)
{
  struct door d;

  d.p = *p;
  d.i = DOOR_MAX_STEP;
  d.action = NO_DOOR_ACTION;
  d.wait = DOOR_WAIT;
  d.noise = false;

  door =
    add_to_array (&d, 1, door, &door_nmemb, door_nmemb, sizeof (d));

  qsort (door, door_nmemb, sizeof (d), compare_doors);
}

int
compare_doors (const void *d0, const void *d1)
{
  return cpos (&((struct door *) d0)->p, &((struct door *) d1)->p);
}

struct door *
door_at_pos (struct pos *p)
{
  struct door d;
  d.p = *p;

  return bsearch (&d, door, door_nmemb, sizeof (d), compare_doors);
}

void
compute_doors (void)
{
  size_t i;

  for (i = 0; i < door_nmemb; i++) {
    struct door *d = &door[i];
    if (d->p.room == -1) {
      /* remove_door (d); i--; */
      continue;
    }
    switch (d->action) {
    case OPEN_DOOR:
      if (d->i == 0 && d->wait == 0) d->action = CLOSE_DOOR;
      else if (d->i == 0 && d->wait > 0) {
        if (! d->noise) {
          sample_door_end = true;
          d->noise = true;
        }

        d->wait--;
      }
      else if (d->i > 0) {
        if (d->i % 2 == 0) sample_door_open = true;
        d->i--;
        d->wait = DOOR_WAIT;
      }
      break;
    case CLOSE_DOOR:
      if (d->i < DOOR_MAX_STEP) {
        if (d->wait++ % 4 == 0) {
          sample_door_close = true;
          d->i++;
          d->noise = false;
        }
      } else if (d->i == DOOR_MAX_STEP) {
        sample_door_end = true;
        d->action = NO_DOOR_ACTION;
        d->wait = DOOR_WAIT;
        d->noise = false;
      }
      break;
    case ABRUPTLY_CLOSE_DOOR:
      if (d->i < DOOR_MAX_STEP) {
        int r = 11 - (d->i % 12);
        d->i += r ? r : 12;
        if (d->i >= DOOR_MAX_STEP) {
          d->i = DOOR_MAX_STEP;
          sample_door_abruptly_close = true;
        }
      } else {
        d->action = NO_DOOR_ACTION;
        d->wait = DOOR_WAIT;
        d->noise = false;
      }
      break;
    default:
      break;
    }
  }
}

void
sample_doors (void)
{
  if (sample_door_open) play_sample (door_open_sample);
  if (sample_door_close) play_sample (door_close_sample);
  if (sample_door_end) play_sample (door_end_sample);
  if (sample_door_abruptly_close) play_sample (door_abruptly_close_sample);

  sample_door_open = sample_door_close = sample_door_end =
    sample_door_abruptly_close = false;
}

void
open_door (int e)
{
  struct pos *p;

  struct door *d;
  struct level_door *ld;

  do {
    p = &level->event[e].p;
    switch (con (p)->fg) {
    case DOOR:
      d = door_at_pos (p);
      if (! d) continue;
      d->action = OPEN_DOOR;
      d->wait = DOOR_WAIT;
      break;
    case LEVEL_DOOR:
      ld = level_door_at_pos (p);
      if (! ld) continue;
      ld->action = OPEN_LEVEL_DOOR;
      break;
    default:
      break;
    }
  } while (level->event[e++].next);
}

void
close_door (int e)
{
  struct pos *p;

  struct door *d;
  struct level_door *ld;

  do {
    p = &level->event[e].p;
    switch (con (p)->fg) {
    case DOOR:
      d = door_at_pos (p);
      if (! d) continue;
      d->action = ABRUPTLY_CLOSE_DOOR;
      break;
    case LEVEL_DOOR:
      ld = level_door_at_pos (p);
      if (! ld) continue;
      ld->action = CLOSE_LEVEL_DOOR;
      break;
    default:
      break;
    }
  } while (level->event[e++].next);
}

int
door_grid_tip_y (struct pos *p)
{
  struct coord c;
  int h = al_get_bitmap_height (dv_door_grid_tip);
  return door_grid_tip_coord (p, &c, door_at_pos (p)->i)->y + h - 1;
}

void
draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
           enum em em, enum vm vm)
{
  draw_floor_base (bitmap, p, em, vm);
  draw_door_left (bitmap, p, em, vm);
  draw_door_right (bitmap, p, em, vm);
}

void
draw_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *door_left = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: door_left = de_door_left; break;
    case VGA: door_left = dv_door_left; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: door_left = pv_door_left; break;
    }
    break;
  }

  struct coord c;
  draw_bitmapc (door_left, bitmap, door_left_coord (p, &c), 0);
}

void
draw_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;
  struct door *d = door_at_pos (p);
  if (! d) return;

  ALLEGRO_BITMAP *door_right = NULL,
    *door_top = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA:
      door_right = de_door_right;
      door_top = de_door_top;
      break;
    case VGA:
      door_right = dv_door_right;
      door_top = dv_door_top;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      door_right = pv_door_right;
      door_top = pv_door_top;
      break;
    }
    break;
  }

  draw_bitmapc (door_right, bitmap, door_right_coord (p, &c), 0);
  draw_bitmapc (door_top, bitmap, door_top_coord (p, &c), 0);
  draw_door_grid (bitmap, p, d->i, em, vm);
}

void
draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
              enum em em, enum vm vm)
{
  struct pos par, pr, pl, pa;
  struct coord nc; struct pos np, ptf, pmt, ptb;

  ALLEGRO_BITMAP *door_pole = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: door_pole = de_door_pole; break;
    case VGA: door_pole = dv_door_pole; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: door_pole = pv_door_pole; break;
    }
    break;
  }

  draw_floor_base (bitmap, p, em, vm);
  draw_bitmapc (door_pole, screen, door_pole_coord (p, &nc), 0);

  survey (_tf, pos, f, &nc, &ptf, &np);
  survey (_mt, pos, f, &nc, &pmt, &np);
  survey (_tb, pos, f, &nc, &ptb, &np);

  prel (p, &pr, +0, +1);
  prel (p, &pl, +0, -1);
  prel (p, &pa, -1, +0);
  prel (p, &par, -1, +1);

  struct pos *hang_pos = &((struct anim *)f->id)->hang_pos;

  if ((((peq (&ptb, p) || peq (&ptb, &pl)
        || peq (&ptb, &pa))
       && (peq (&pmt, p) || peq (&pmt, &pl)
           || peq (&pmt, &pa)))
      || is_kid_successfully_climbing_at_pos
       (f, hang_pos, p)
      || (is_kid_turn_run (f)
          && f->dir == LEFT))
      && ! is_kid_successfully_climbing_at_pos
      (f, hang_pos, &pa)) {
    struct door *d = door_at_pos (p);
    draw_door_grid (screen, p, d->i, em, vm);
    draw_confg_right (bitmap, &pa, em, vm, true);
    draw_confg_base (bitmap, &par, em, vm);
    draw_confg_left (bitmap, &par, em, vm, true);
    draw_confg_fg (bitmap, &pr, em, vm, f);
  }
}

void
draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                enum em em, enum vm vm)
{
  ALLEGRO_BITMAP *door_grid = NULL,
    *door_grid_tip = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA:
      door_grid = de_door_grid;
      door_grid_tip = de_door_grid_tip;
      break;
    case VGA:
      door_grid = dv_door_grid;
      door_grid_tip = dv_door_grid_tip;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA:
      door_grid = pv_door_grid;
      door_grid_tip = pv_door_grid_tip;
      break;
    }
    break;
  }

  int q = i / 8;
  int r = i % 8;
  int w = al_get_bitmap_width (door_grid);
  int j;

  struct coord c;
  draw_bitmap_regionc (door_grid, bitmap, 0, 7 - r, w, r + 1,
                       door_grid_coord_base (p, &c), 0);
  for (j = 0; j <= q; j++)
    draw_bitmapc (door_grid, bitmap, door_grid_coord (p, &c, j, i), 0);
  draw_bitmapc (door_grid_tip, bitmap, door_grid_tip_coord (p, &c, i), 0);
}

struct coord *
door_grid_coord_base (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 6;
  c->room = p->room;
  return c;
}

struct coord *
door_grid_coord (struct pos *p, struct coord *c, int j, int i)
{
  int r = i % 8;
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 6 + j * 8 + r + 1;
  c->room = p->room;
  return c;
}

struct coord *
door_grid_tip_coord (struct pos *p, struct coord *c, int i)
{
  int r = i % 8;
  int q = i / 8;
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 2 + q * 8 + r + 1;
  c->room = p->room;
  return c;
}

struct coord *
door_pole_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 24;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
door_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
door_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
door_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 10;
  c->room = p->room;
  return c;
}
