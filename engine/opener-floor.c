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
#include "physics.h"
#include "room.h"
#include "floor.h"
#include "kid.h"
#include "door.h"
#include "opener-floor.h"

static struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  int resist_on;
  int resist_off;
} opener_floor[OPENER_FLOORS];

ALLEGRO_SAMPLE *opener_floor_sound;

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
  int i;
  for (i = 0; i < OPENER_FLOORS; i++) if (peq (opener_floor[i].p, p)) return;
  for (i = 0; opener_floor[i].p.room && i < OPENER_FLOORS; i++);
  if (i == OPENER_FLOORS)
    error (-1, 0, "%s: no free opener floor slot (%i)",
           __func__, construct (p).fg);

  opener_floor[i].p = p;
  opener_floor[i].event = construct (p).event;
  opener_floor[i].pressed = false;
  opener_floor[i].noise = false;
  opener_floor[i].resist_on = 1;
  opener_floor[i].resist_off = 0;
}

struct opener_floor *
opener_floor_at_pos (struct pos p)
{
  int i;
  for (i = 0; i < OPENER_FLOORS; i++)
    if (peq (opener_floor[i].p, p)) return &opener_floor[i];
  error (-1, 0, "%s: no opener floor at position has been registered (%i, %i, %i)",
         __func__, p.room, p.floor, p.place);
  return &opener_floor[0];
}

void
draw_opener_floors (void)
{
  int i;
  for (i = 0; i < OPENER_FLOORS; i++) {
    struct opener_floor *o = &opener_floor[i];
    if (o->p.room && is_pos_visible (o->p)) {
      if (is_on_floor (kid, OPENER_FLOOR)
          && peq (floor_pos, o->p)) {

        o->resist_off = 3;

        if (o->resist_on > 0) {
          o->resist_on--; return;
        }

        if (! o->noise) {
          play_sample (opener_floor_sound);
          o->noise = true;
        }

        draw_opener_floors_0 (o);
        open_door (o->event);
      } else {
        if (o->resist_off > 0) {
          o->resist_off--;
          draw_opener_floors_0 (o);
          return;
        }

        o->pressed = false;
        o->noise = false;
        o->resist_on = 1;
      }
    }
  }
}


void
draw_opener_floors_0 (struct opener_floor *o)
{
  draw_no_floor (screen, o->p);
  draw_floor (screen, o->p);
  draw_construct_left (screen, prel (o->p, 0, +1));
  draw_construct_right (screen, prel (o->p, +1, 0));
}
