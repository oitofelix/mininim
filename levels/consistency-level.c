/*
  consistency-level.c -- consistency level module;

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

#include "kernel/video.h"
#include "kernel/random.h"
#include "engine/level.h"

static struct level consistency_level;

static void
init_consistency_level (void)
{
  struct pos p;

  random_seed = 1;

  struct level *lv = &consistency_level;

  lv->type = DUNGEON;
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        struct con *c = &lv->con[p.room][p.floor][p.place];
        c->fg = prandom (DOOR);
        c->bg = prandom (WINDOW);
        c->ext.item = prandom (SWORD);
      }

  for (p.room = 0; p.room < ROOMS; p.room++) {
    lv->link[p.room].l = prandom (ROOMS - 1);
    lv->link[p.room].r = prandom (ROOMS - 1);
    lv->link[p.room].a = prandom (ROOMS - 1);
    lv->link[p.room].b = prandom (ROOMS - 1);
  }

}

void
play_consistency_level (void)
{
  init_consistency_level ();
  play_level (&consistency_level);
}
