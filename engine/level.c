/*
  level.c -- level module;

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

#include "kernel/video.h"
#include "kernel/keyboard.h"

#include "anim.h"
#include "physics.h"
#include "room.h"
#include "fire.h"
#include "kid.h"
#include "floor.h"
#include "spikes.h"
#include "door.h"
#include "level.h"

/* functions */
static void load_level (void);
static void unload_level (void);
static void level_anim (void);

/* current level */
struct level *level;

static bool no_room_drawing = false;

int room_view = 6;

void
play_level (struct level *_level)
{
  cutscene = false;

  level = _level;
  load_level ();
  play_anim (level_anim, 12);
  unload_level ();
}

static void
load_level (void)
{
  load_room ();
  load_fire ();
  load_kid ();
}

static void
unload_level (void)
{
  unload_room ();
  unload_fire ();
  unload_kid ();
}

static void
level_anim (void)
{
  int prev_room = room_view;

  if (was_key_pressed (ALLEGRO_KEY_H, true))
    room_view = level->link[room_view].l;
  if (was_key_pressed (ALLEGRO_KEY_J, true))
    room_view = level->link[room_view].r;
  if (was_key_pressed (ALLEGRO_KEY_U, true))
    room_view = level->link[room_view].a;
  if (was_key_pressed (ALLEGRO_KEY_N, true))
    room_view = level->link[room_view].b;

  if (was_key_pressed (ALLEGRO_KEY_B, true))
    no_room_drawing = ! no_room_drawing;

  if (room_view == 0) room_view = prev_room;

  level_draw_base ();

  if (is_visible (kid)) {
    prev_room = kid.c.room;
    kid.draw ();
    if (kid.c.room != prev_room) {
      room_view = kid.c.room;
      level_draw_base ();
      redraw_anim (kid);
    }
  }

  draw_room_anim_fg (kid);


  /* level_draw_base (); */
  /* if (is_visible (kid)) { */
  /*   prev_room = kid.c.room; */
  /*   kid.draw (); */
  /*   if (kid.c.room != prev_room) { */
  /*     room_view = kid.c.room; */
  /*     level_draw_base (); */
  /*     redraw_anim (kid); */
  /*   } */
  /* } */

  /* draw_shake_floor (); */
  /* draw_release_loose_floor (); */
  /* draw_spikes (); */
  /* draw_doors (); */
  /* draw_room_anim_fg (kid); */
}

void
level_draw_base (void)
{
  clear_bitmap (screen, BLACK);
  if (! no_room_drawing) draw_room (room_view);
  draw_fire (room_view);
  draw_shake_floor ();
  draw_release_loose_floor ();
  draw_spikes ();
  draw_doors ();
}
