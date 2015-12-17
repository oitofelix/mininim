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
#include <error.h>
#include "kernel/audio.h"
#include "kernel/array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "level-door.h"

ALLEGRO_BITMAP *level_door_top_left, *level_door_top_right,
  *level_door_bottom_left, *level_door_bottom_right,
  *level_door_floor, *level_door_stairs, *level_door_front;

ALLEGRO_SAMPLE *level_door_open_sample, *level_door_close_sample;

bool sample_level_door_open, sample_level_door_close;

static struct level_door *level_door = NULL;
static size_t level_door_nmemb = 0;

void
load_vdungeon_level_door (void)
{
  level_door_top_left = load_bitmap (VDUNGEON_LEVEL_DOOR_TOP_LEFT);
  level_door_top_right = load_bitmap (VDUNGEON_LEVEL_DOOR_TOP_RIGHT);
  level_door_bottom_left = load_bitmap (VDUNGEON_LEVEL_DOOR_BOTTOM_LEFT);
  level_door_bottom_right = load_bitmap (VDUNGEON_LEVEL_DOOR_BOTTOM_RIGHT);
  level_door_floor = load_bitmap (VDUNGEON_LEVEL_DOOR_FLOOR);
  level_door_stairs = load_bitmap (VDUNGEON_LEVEL_DOOR_STAIRS);
  level_door_front = load_bitmap (VDUNGEON_LEVEL_DOOR_FRONT);
}

void
unload_level_door (void)
{
  al_destroy_bitmap (level_door_top_left);
  al_destroy_bitmap (level_door_top_right);
  al_destroy_bitmap (level_door_bottom_left);
  al_destroy_bitmap (level_door_bottom_right);
  al_destroy_bitmap (level_door_floor);
  al_destroy_bitmap (level_door_stairs);
  al_destroy_bitmap (level_door_front);
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
  d.i = LEVEL_DOOR_MAX_STEP;
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
      if (d->i % 5 == 2 && d->i > 2) sample_level_door_open = true;
      if (d->i > 0) d->i--;
      else d->action = NO_LEVEL_DOOR_ACTION;
      break;
    case CLOSE_LEVEL_DOOR:
      if (d->i == 0) sample_level_door_open = true;
      if (d->i < LEVEL_DOOR_MAX_STEP) {
        int r = 14 - (d->i % 15);
        d->i += r ? r : 15;
        if (d->i >= LEVEL_DOOR_MAX_STEP) {
          d->i = LEVEL_DOOR_MAX_STEP;
          d->action = NO_LEVEL_DOOR_ACTION;
          sample_level_door_close = true;
        }
      }
      break;
    default:
      break;
    }
  }
}

void
sample_level_doors (void)
{
  if (sample_level_door_open) play_sample (level_door_open_sample);
  if (sample_level_door_close) play_sample (level_door_close_sample);

  sample_level_door_open = sample_level_door_close = false;
}

void
draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_floor_base (bitmap, p);
  draw_level_door_left (bitmap, p);
  draw_level_door_right (bitmap, p);
}

void
draw_level_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (level_door_bottom_left, bitmap,
                level_door_bottom_left_coord (p, &c), 0);
  draw_bitmapc (level_door_top_left, bitmap,
                level_door_top_left_coord (p, &c), 0);
}

void
draw_level_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  struct level_door *d = level_door_at_pos (p);
  if (! d) return;
  draw_bitmapc (level_door_floor, bitmap,
                level_door_floor_coord (p, &c), 0);
  draw_bitmapc (level_door_stairs, bitmap,
                level_door_stairs_coord (p, &c), 0);
  draw_bitmapc (level_door_bottom_right, bitmap,
                level_door_bottom_right_coord (p, &c), 0);
  draw_bitmapc (level_door_top_right, bitmap,
                level_door_top_right_coord (p, &c), 0);
  draw_level_door_front (bitmap, p, d->i);
}

void
draw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i)
{
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
level_door_top_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 13;
  c->room = p->room;
  return c;
}

struct coord *
level_door_top_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor - 13;
  c->room = p->room;
  return c;
}

struct coord *
level_door_bottom_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
level_door_bottom_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
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
