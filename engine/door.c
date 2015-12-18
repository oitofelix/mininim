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

ALLEGRO_BITMAP *door_left, *door_right, *door_pole, *door_top,
  *door_grid, *door_grid_tip;

ALLEGRO_SAMPLE *door_open_sample, *door_close_sample, *door_end_sample,
  *door_abruptly_close_sample;

bool sample_door_open, sample_door_close, sample_door_end,
  sample_door_abruptly_close;

static struct door *door = NULL;
static size_t door_nmemb = 0;

void
load_vdungeon_door (void)
{
  door_left = load_bitmap (VDUNGEON_DOOR_LEFT);
  door_right = load_bitmap (VDUNGEON_DOOR_RIGHT);
  door_pole = load_bitmap (VDUNGEON_DOOR_POLE);
  door_top = load_bitmap (VDUNGEON_DOOR_TOP);
  door_grid = load_bitmap (VDUNGEON_DOOR_GRID);
  door_grid_tip = load_bitmap (VDUNGEON_DOOR_GRID_TIP);
}

void
unload_door (void)
{
  al_destroy_bitmap (door_left);
  al_destroy_bitmap (door_right);
  al_destroy_bitmap (door_pole);
  al_destroy_bitmap (door_top);
  al_destroy_bitmap (door_grid);
  al_destroy_bitmap (door_grid_tip);
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
      /* error (-1, 0, "%s: requested to open non-door (%i, %i, %i)", */
      /*        __func__, p->room, p->floor, p->place); */
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
      /* error (-1, 0, "%s: requested to close non-door (%i, %i, %i)", */
      /*        __func__, p->room, p->floor, p->place); */
      break;
    }
  } while (level->event[e++].next);
}

int
door_grid_tip_y (struct pos *p)
{
  struct coord c;
  int h = al_get_bitmap_height (door_grid_tip);
  return door_grid_tip_coord (p, &c, door_at_pos (p)->i)->y + h - 1;
}

void
draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_floor_base (bitmap, p, DUNGEON, VGA);
  draw_door_left (bitmap, p);
  draw_door_right (bitmap, p);
}

void
draw_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (door_left, bitmap, door_left_coord (p, &c), 0);
}

void
draw_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  struct door *d = door_at_pos (p);
  if (! d) return;
  draw_bitmapc (door_right, bitmap, door_right_coord (p, &c), 0);
  draw_bitmapc (door_top, bitmap, door_top_coord (p, &c), 0);
  draw_door_grid (bitmap, p, d->i);
}

void
draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  struct pos par, pr, pl, pa;
  struct coord nc; struct pos np, ptf, pmt, ptb;

  draw_floor_base (bitmap, p, DUNGEON, VGA);
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
    draw_door_grid (screen, p, d->i);
    draw_confg_right (bitmap, &pa, DUNGEON, VGA, true);
    draw_confg_base (bitmap, &par, DUNGEON, VGA);
    draw_confg_left (bitmap, &par, DUNGEON, VGA, true);
    draw_confg_fg (bitmap, &pr, DUNGEON, VGA, f);
  }
}

void
draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos *p, int i)
{
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
