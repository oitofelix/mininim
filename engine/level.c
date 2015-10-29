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
#include "kernel/random.h"

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
  clear_bitmap (screen, BLACK);
  draw_room (1);
  draw_fire (1);
  kid.draw ();

  int w = al_get_bitmap_width (kid.frame);
  int h = al_get_bitmap_height (kid.frame);

  struct pos p;

  p = room_pos_xy (kid.room, kid.x, kid.y + h - 1);
  draw_room_fg (p);

  p = room_pos_xy (kid.room, kid.x + w - 1, kid.y + h - 1);
  draw_room_fg (p);

  p = room_pos_xy (kid.room, kid.x + w / 2, kid.y + h / 2);
  draw_room_fg (p);

  p = room_pos_xy (kid.room, kid.x, kid.y);
  draw_room_fg (p);

  p = room_pos_xy (kid.room, kid.x + w - 1, kid.y);
  draw_room_fg (p);
}
