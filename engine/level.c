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
#include "level.h"

/* functions */
static void load_level (void);
static void unload_level (void);
static void level_anim (void);

/* current level */
struct level *level;

void
play_level (struct level *_level)
{
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
  static unsigned int room = 1;

  unsigned int prev_room = room;

  if (was_key_pressed (ALLEGRO_KEY_H, true))
    room = level->link[room].l;
  if (was_key_pressed (ALLEGRO_KEY_J, true))
    room = level->link[room].r;
  if (was_key_pressed (ALLEGRO_KEY_U, true))
    room = level->link[room].a;
  if (was_key_pressed (ALLEGRO_KEY_N, true))
    room = level->link[room].b;

  if (room == 0) room = prev_room;

  clear_bitmap (screen, BLACK);
  draw_room (room);
  draw_fire (room);
  if (room == kid.room) {
    kid.draw ();
    draw_room_anim_fg (kid);
  }
}
