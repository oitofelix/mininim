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
#include "kid.h"
#include "level.h"
#include "door.h"

ALLEGRO_BITMAP *door_left, *door_right, *door_pole, *door_top,
  *door_grid, *door_grid_tip, *door_grid_top;

ALLEGRO_SAMPLE *door_open_sound, *door_close_sound, *door_end_sound,
  *door_abruptly_close_sound;

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
  door_grid_top = load_bitmap (VDUNGEON_DOOR_GRID_TOP);
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
  al_destroy_bitmap (door_grid_top);
}

void
load_door_sounds (void)
{
  door_open_sound = load_sample (DOOR_OPEN_SOUND);
  door_close_sound = load_sample (DOOR_CLOSE_SOUND);
  door_end_sound = load_sample (DOOR_END_SOUND);
  door_abruptly_close_sound = load_sample (DOOR_ABRUPTLY_CLOSE_SOUND);
}

void
unload_door_sounds (void)
{
  al_destroy_sample (door_open_sound);
  al_destroy_sample (door_close_sound);
  al_destroy_sample (door_end_sound);
  al_destroy_sample (door_abruptly_close_sound);
}

void
register_door (struct pos p)
{
  struct door d;

  d.p = p;
  d.i = DOOR_MAX_STEP;
  d.action = NO_DOOR_ACTION;
  d.wait = DOOR_WAIT;
  d.noise = false;

  door =
    add_to_array (&d, 1, door, &door_nmemb, door_nmemb, sizeof (d));
}

struct door *
door_at_pos (struct pos p)
{
  size_t i;
  for (i = 0; i < door_nmemb; i++)
    if (peq (door[i].p, p)) return &door[i];
  return NULL;
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
          play_sample (door_end_sound);
          d->noise = true;
        }

        d->wait--;
      }
      else if (d->i > 0) {
        if (d->i % 2 == 0) play_sample (door_open_sound);
        d->i--;
        d->wait = DOOR_WAIT;
      }
      break;
    case CLOSE_DOOR:
      if (d->i < DOOR_MAX_STEP) {
        if (d->wait++ % 4 == 0) {
          play_sample (door_close_sound);
          d->i++;
          d->noise = false;
        }
      } else if (d->i == DOOR_MAX_STEP) {
        play_sample (door_end_sound);
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
          play_sample (door_abruptly_close_sound);
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
open_door (int e)
{
  do {
    struct door *d = door_at_pos (level->event[e].p);
    if (! d) continue;
    d->action = OPEN_DOOR;
    d->wait = DOOR_WAIT;
  } while (level->event[e++].next);
}

void
close_door (int e)
{
  do {
    struct door *d = door_at_pos (level->event[e].p);
    if (! d) continue;
    d->action = ABRUPTLY_CLOSE_DOOR;
  } while (level->event[e++].next);
}

int
door_grid_tip_y (struct pos p)
{
  int h = al_get_bitmap_height (door_grid_tip);
  return door_grid_tip_coord (p, door_at_pos (p)->i).y + h - 1;
}

void
draw_door (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct door *d = door_at_pos (p);
  if (! d) return;

  draw_door_grid (bitmap, p, d->i);
  draw_con_left (bitmap, prel (p, -1, +1));
  draw_con_left (bitmap, prel (p, +0, +1));
}

void
draw_door_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_bitmapc (normal_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_door_frame (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_door_base (bitmap, p);
  draw_bitmapc (door_left, bitmap, door_left_coord (p), 0);
  draw_bitmapc (door_right, bitmap, door_right_coord (p), 0);
  draw_bitmapc (door_top, bitmap, door_top_coord (p), 0);
}

void
draw_door_frame_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_door_base (bitmap, p);
  draw_bitmapc (door_left, bitmap, door_left_coord (p), 0);
}

void
draw_door_frame_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  draw_door_base (bitmap, p);
  draw_bitmapc (door_right, bitmap, door_right_coord (p), 0);
}

void
draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos p, struct anim a)
{
  draw_door_base (bitmap, p);
  draw_bitmapc (door_pole, screen, door_pole_coord (p), 0);

  if (is_kid_hanging_at_pos (a, p)) return;
  struct pos pbf = pos (coord_bf (a));
  struct pos pmbo = pos (coord_mbo (a));
  struct pos pbb = pos (coord_bb (a));

  if ((a.dir == RIGHT && peq (pbf, p))
      || (a.dir == RIGHT
          && a.action == kid_vjump
          && peq (pbb, p))
      || (a.dir == LEFT
          && a.action == kid_turn
          && peq (pmbo, p))
      || (a.dir == LEFT && peq (pbb, p))) {
    struct door *d = door_at_pos (p);
    draw_door_grid (screen, p, d->i);
    draw_con_left (screen, prel (p, -1, +1));
    draw_con_left (screen, prel (p, +0, +1));
  }
}

void
draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos p, int i)
{
  int q = i / 8;
  int r = i % 8;
  int w = al_get_bitmap_width (door_grid);
  int j;

  draw_bitmapc (door_grid_top, bitmap, door_grid_top_coord (p), 0);
  draw_bitmap_regionc (door_grid, bitmap, 0, 7 - r, w, r + 1,
                       door_grid_coord_base (p), 0);
  for (j = 0; j < q; j++)
    draw_bitmapc (door_grid, bitmap, door_grid_coord (p, j, i), 0);
  draw_bitmapc (door_grid_tip, bitmap, door_grid_tip_coord (p, i), 0);
}

struct coord
door_grid_coord_base (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 2;
  c.room = p.room;
  return c;
}

struct coord
door_grid_coord (struct pos p, int j, int i)
{
  int r = i % 8;

  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 2 + j * 8 + r + 1;
  c.room = p.room;
  return c;
}

struct coord
door_grid_tip_coord (struct pos p, int i)
{
  int r = i % 8;
  int q = i / 8;

  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 2 + q * 8 + r + 1;
  c.room = p.room;
  return c;
}

struct coord
door_grid_top_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor - 6;
  c.room = p.room;
  return c;
}

struct coord
door_pole_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place + 24;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
door_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
door_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 3;
  c.room = p.room;
  return c;
}

struct coord
door_top_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor - 10;
  c.room = p.room;
  return c;
}
