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
  o.event = con (p).event;
  o.noise = false;
  o.resist_on = OPENER_FLOOR_ON_RESISTENCE;
  o.resist_off = OPENER_FLOOR_OFF_RESISTENCE;
  o.draw = draw_unpressed_opener_floor;
  o.draw_left = draw_unpressed_opener_floor_left;
  o.draw_right = draw_unpressed_opener_floor_right;

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
  error (-1, 0, "%s: no opener floor at position has been registered (%i, %i, %i)",
         __func__, p.room, p.floor, p.place);
  return &opener_floor[0];
}

void
draw_opener_floors (void)
{
  if (opener_floor_nmemb == 0) return;

  size_t i;
  for (i = opener_floor_nmemb - 1; (int) i >= 0; i--) {
    struct opener_floor *o = &opener_floor[i];
    if (is_pos_visible (o->p)) {
      if (kids.cmbo == OPENER_FLOOR
          && peq (kids.pmbo, o->p)) {

        o->resist_off = OPENER_FLOOR_OFF_RESISTENCE;

        if (o->resist_on > 0) o->resist_on--;
        else {
          if (! o->noise) {
            play_sample (opener_floor_sound);
            o->noise = true;
          }

          open_door (o->event);
        }
      } else {
        if (o->resist_off > 0) o->resist_off--;
        else {
          o->noise = false;
          o->resist_on = OPENER_FLOOR_ON_RESISTENCE;
        }
      }

      if (kids.cmbo == OPENER_FLOOR
          && peq (kids.pmbo, o->p))
        draw_pressed_opener_floor (screen, o);
      else draw_unpressed_opener_floor (screen, o);
    }
  }
}

void
draw_pressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_pressed_opener_floor;
  o->draw_left = draw_pressed_opener_floor_left;
  o->draw_right = draw_pressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_floor (bitmap, o->p);
  draw_con_left (bitmap, prel (o->p, 0, +1));
}

void
draw_pressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_pressed_opener_floor;
  o->draw_left = draw_pressed_opener_floor_left;
  o->draw_right = draw_pressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_floor_left (bitmap, o->p);
}

void
draw_pressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_pressed_opener_floor;
  o->draw_left = draw_pressed_opener_floor_left;
  o->draw_right = draw_pressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_floor_right (bitmap, o->p);
  draw_con_left (bitmap, prel (o->p, 0, +1));
}

void
draw_unpressed_opener_floor_base (ALLEGRO_BITMAP *bitmap, struct pos p)
{
  if (bitmap == room_bg) return;
  draw_bitmapc (unpressed_opener_floor_base, bitmap, floor_base_coord (p), 0);
}

void
draw_unpressed_opener_floor (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_unpressed_opener_floor;
  o->draw_left = draw_unpressed_opener_floor_left;
  o->draw_right = draw_unpressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, o->p);
  draw_bitmapc (unpressed_opener_floor_left, bitmap, unpressed_opener_floor_left_coord (o->p), 0);
  draw_bitmapc (normal_floor_right, bitmap, unpressed_opener_floor_right_coord (o->p), 0);

  draw_con_left (bitmap, prel (o->p, 0, +1));
}

void
draw_unpressed_opener_floor_left (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_unpressed_opener_floor;
  o->draw_left = draw_unpressed_opener_floor_left;
  o->draw_right = draw_unpressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, o->p);
  draw_bitmapc (unpressed_opener_floor_left, bitmap, unpressed_opener_floor_left_coord (o->p), 0);
}

void
draw_unpressed_opener_floor_right (ALLEGRO_BITMAP *bitmap, struct opener_floor *o)
{
  o->draw = draw_unpressed_opener_floor;
  o->draw_left = draw_unpressed_opener_floor_left;
  o->draw_right = draw_unpressed_opener_floor_right;

  if (bitmap == room_bg) return;
  draw_unpressed_opener_floor_base (bitmap, o->p);
  draw_bitmapc (normal_floor_right, bitmap, unpressed_opener_floor_right_coord (o->p), 0);

  draw_con_left (bitmap, prel (o->p, 0, +1));
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
