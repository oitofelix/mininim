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
#include "kernel/audio.h"
#include "kernel/video.h"
#include "kernel/array.h"
#include "physics.h"
#include "room.h"
#include "floor.h"
#include "kid.h"
#include "door.h"
#include "floor.h"
#include "opener-floor.h"

ALLEGRO_BITMAP *unpressed_opener_floor_left, *unpressed_opener_floor_base;

ALLEGRO_SAMPLE *opener_floor_sound;

static struct opener_floor *opener_floor = NULL;
static size_t opener_floor_nmemb = 0;

void
load_vdungeon_opener_floor (void)
{
  unpressed_opener_floor_left = load_bitmap (VDUNGEON_UNPRESSED_OPENER_FLOOR_LEFT);
  unpressed_opener_floor_base = load_bitmap (VDUNGEON_UNPRESSED_OPENER_FLOOR_BASE);
}

void
unload_opener_floor (void)
{
  al_destroy_bitmap (unpressed_opener_floor_left);
  al_destroy_bitmap (unpressed_opener_floor_base);
}

void
load_opener_floor_sounds (void)
{
  opener_floor_sound = load_sample (OPENER_FLOOR_SOUND);
}

void
unload_opener_floor_sounds (void)
{
  al_destroy_sample (opener_floor_sound);
}

void
register_opener_floor (struct pos p)
{
  struct opener_floor o;

  o.p = p;
  o.event = con (p)->ext.event;
  o.pressed = false;
  o.noise = false;
  o.broken = false;

  opener_floor =
    add_to_array (&o, 1, opener_floor, &opener_floor_nmemb,
                  opener_floor_nmemb, sizeof (o));
}

struct opener_floor *
opener_floor_at_pos (struct pos p)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++)
    if (peq (opener_floor[i].p, p)) return &opener_floor[i];
  return NULL;
}

void
press_opener_floor (struct pos p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  o->pressed = true;
}

void
break_opener_floor (struct pos p)
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
        play_sample (opener_floor_sound);
        o->noise = true;
      }
      open_door (o->event);
    } else o->noise = false;
  }
}

void
draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_pressed_opener_floor (bitmap, p);
  else draw_unpressed_opener_floor (bitmap, p);
}

void
draw_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_pressed_opener_floor_base (bitmap, p);
  else draw_unpressed_opener_floor_base (bitmap, p);
}

void
draw_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_pressed_opener_floor_left (bitmap, p);
  else draw_unpressed_opener_floor_left (bitmap, p);
}

void
draw_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->broken) return;
  if (o->pressed) draw_pressed_opener_floor_right (bitmap, p);
  else draw_unpressed_opener_floor_right (bitmap, p);
}

void
draw_pressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_floor (bitmap, p);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_pressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_floor_base (bitmap, p);
}

void
draw_pressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_floor_left (bitmap, p);
}

void
draw_pressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_floor_right (bitmap, p);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_bitmapc (unpressed_opener_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, p);
  draw_bitmapc (unpressed_opener_floor_left, bitmap, unpressed_opener_floor_left_coord (p), 0);
  draw_bitmapc (normal_floor_right, bitmap, unpressed_opener_floor_right_coord (p), 0);

  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, p);
  draw_bitmapc (unpressed_opener_floor_left, bitmap, unpressed_opener_floor_left_coord (p), 0);
}

void
draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_right, bitmap, unpressed_opener_floor_right_coord (p), 0);

  draw_con_left (bitmap, prel (p, 0, +1));
}

struct coord
unpressed_opener_floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}

struct coord
unpressed_opener_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 50 - 1;
  c.room = p.room;
  return c;
}
