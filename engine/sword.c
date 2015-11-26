/*
  sword.c -- sword module;

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
#include "level.h"
#include "physics.h"
#include "room.h"
#include "sword.h"

static ALLEGRO_BITMAP *normal_sword, *shiny_sword;

static struct coord sword_coord (struct pos p);

void
load_sword (void)
{
  normal_sword = load_bitmap (NORMAL_SWORD);
  shiny_sword = load_bitmap (SHINY_SWORD);
}

void
unload_sword (void)
{
  al_destroy_bitmap (normal_sword);
  al_destroy_bitmap (shiny_sword);
}

void
draw_sword (ALLEGRO_BITMAP *bitmap, struct pos p, int i)
{
  if (! is_sword (p)) return;
  ALLEGRO_BITMAP *sword = i % 60 ? normal_sword : shiny_sword;
  seedp (p);
  draw_bitmapc (sword, bitmap, sword_coord (p),
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  unseedp ();
}

bool
is_sword (struct pos p)
{
  return con (p).fg != OPENER_FLOOR
    && con (p).fg != CLOSER_FLOOR
    && con (p).ext.item == SWORD;
}

struct coord
sword_coord (struct pos p)
{
  struct coord c;
  c.x = PLACE_WIDTH * p.place;
  c.y = PLACE_HEIGHT * p.floor + 50;
  c.room = p.room;
  return c;
}
