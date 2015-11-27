/*
  closer-floor.c -- closer floor module;

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
#include "closer-floor.h"

ALLEGRO_BITMAP *unpressed_closer_floor_base, *pressed_closer_floor_base,
  *pressed_closer_floor_right;

ALLEGRO_SAMPLE *closer_floor_sound;

static struct closer_floor *closer_floor = NULL;
static size_t closer_floor_nmemb = 0;

void
load_vdungeon_closer_floor (void)
{
  unpressed_closer_floor_base = load_bitmap (VDUNGEON_UNPRESSED_CLOSER_FLOOR_BASE);
  pressed_closer_floor_base = load_bitmap (VDUNGEON_PRESSED_CLOSER_FLOOR_BASE);
  pressed_closer_floor_right = load_bitmap (VDUNGEON_PRESSED_CLOSER_FLOOR_RIGHT);
}

void
unload_closer_floor (void)
{
  al_destroy_bitmap (unpressed_closer_floor_base);
  al_destroy_bitmap (pressed_closer_floor_base);
  al_destroy_bitmap (pressed_closer_floor_right);
}

void
load_closer_floor_sounds (void)
{
  closer_floor_sound = load_sample (CLOSER_FLOOR_SOUND);
}

void
unload_closer_floor_sounds (void)
{
  al_destroy_sample (closer_floor_sound);
}

void
register_closer_floor (struct pos p)
{
  struct closer_floor c;

  c.p = p;
  c.event = con (p)->ext.event;
  c.pressed = false;
  c.noise = false;
  c.broken = false;

  closer_floor =
    add_to_array (&c, 1, closer_floor, &closer_floor_nmemb,
                  closer_floor_nmemb, sizeof (c));
}

struct closer_floor *
closer_floor_at_pos (struct pos p)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++)
    if (peq (closer_floor[i].p, p)) return &closer_floor[i];
  return NULL;
}

void
press_closer_floor (struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  c->pressed = true;
}

void
break_closer_floor (struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  c->broken = true;
  press_closer_floor (p);
}

void
unpress_closer_floors (void)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++)
    closer_floor[i].pressed = false;
}

void
compute_closer_floors (void)
{
  size_t i;

  for (i = 0; i < closer_floor_nmemb; i++) {
    struct closer_floor *c = &closer_floor[i];
    if (c->p.room == -1) {
      /* remove_closer_floor (o); i--; */
      continue;
    }
    if (c->pressed) {
      if (! c->noise) {
        play_sample (closer_floor_sound);
        c->noise = true;
      }
      close_door (c->event);
    } else c->noise = false;
  }
}

void
draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) return;
  if (c->pressed) draw_pressed_closer_floor (bitmap, p);
  else draw_unpressed_closer_floor (bitmap, p);
}

void
draw_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) return;
  if (c->pressed) draw_pressed_closer_floor_base (bitmap, p);
  else draw_unpressed_closer_floor_base (bitmap, p);
}

void
draw_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) return;
  if (c->pressed) draw_pressed_closer_floor_left (bitmap, p);
  else draw_unpressed_closer_floor_left (bitmap, p);
}

void
draw_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->broken) return;
  if (c->pressed) draw_pressed_closer_floor_right (bitmap, p);
  else draw_unpressed_closer_floor_right (bitmap, p);
}

void
draw_pressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_bitmapc (pressed_closer_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_pressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_pressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, pressed_closer_floor_left_coord (p), 0);
  draw_bitmapc (normal_floor_right, bitmap,
                pressed_closer_floor_right_coord (p), 0);
  if (crel (p, 0, +1)->fg != NO_FLOOR)
    draw_bitmapc (pressed_closer_floor_right, bitmap,
                  floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_pressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_pressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, pressed_closer_floor_left_coord (p), 0);
}

void
draw_pressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_pressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_right, bitmap,
                pressed_closer_floor_right_coord (p), 0);
  if (crel (p, 0, +1)->fg != NO_FLOOR)
    draw_bitmapc (pressed_closer_floor_right, bitmap,
                  floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_unpressed_closer_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_bitmapc (unpressed_closer_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_unpressed_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, floor_left_coord (p), 0);
  draw_bitmapc (normal_floor_right, bitmap, floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

void
draw_unpressed_closer_floor_left (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_left, bitmap, floor_left_coord (p), 0);
}

void
draw_unpressed_closer_floor_right (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_unpressed_closer_floor_base (bitmap, p);
  draw_bitmapc (normal_floor_right, bitmap, floor_right_coord (p), 0);
  draw_con_left (bitmap, prel (p, 0, +1));
}

struct coord
pressed_closer_floor_left_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 51;
  c.room = p.room;
  return c;
}

struct coord
pressed_closer_floor_right_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * (p.place + 1);
  c.y = PLACE_HEIGHT * p.floor + 51;
  c.room = p.room;
  return c;
}
