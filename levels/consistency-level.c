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

#include <stdio.h>
#include <time.h>
#include "kernel/video.h"
#include "kernel/random.h"
#include "engine/level.h"

static struct level consistency_level;

static void
init_consistency_level (void)
{
  struct pos p;

  random_seed = 1;
  /* random_seed = time (NULL); */
  printf ("LEVEL NUMBER: %u\n", random_seed);

  struct level *lv = &consistency_level;

  lv->type = DUNGEON;

  /* generate room 0 (delimiter room) */
  p.room = 0;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++) {
      struct con *c = &lv->con[p.room][p.floor][p.place];
      c->fg = WALL;
      c->bg = NO_BG;
    }

  for (p.room = 1; p.room < ROOMS; p.room++) {
    lv->link[p.room].l = 0;
    lv->link[p.room].r = 0;
    lv->link[p.room].a = 0;
    lv->link[p.room].b = 0;

    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        struct con *c = &lv->con[p.room][p.floor][p.place];
        c->fg = prandom (DOOR);
        c->bg = prandom (WINDOW);
        c->ext.item = prandom (SWORD);

        int r = prandom (255);
        if (c->fg == OPENER_FLOOR
            || c->fg == CLOSER_FLOOR) c->ext.event = r;
        if (c->fg == DOOR) {
          lv->event[r].p = p;
          lv->event[r].next = true;
        }

      }
  }

  int room, a, b, l, r, al, bl, ar, br, la, ra, lb, rb;

  lv->link[1].l = 2;
  lv->link[2].r = 1;

  for (p.room = 3; p.room < ROOMS; p.room++) {
    for (room = 1; room < ROOMS; room++) {
      if (p.room == room) continue;

      if (! lv->link[room].l) {
        lv->link[room].l = p.room;
        lv->link[p.room].r = room;

        a = lv->link[room].a;
        if (a) {
          al = lv->link[a].l;
          if (al) {
            lv->link[p.room].a = al;
            lv->link[al].b = p.room;
          }
        }

        b = lv->link[room].b;
        if (b) {
          bl = lv->link[b].l;
          if (bl) {
            lv->link[p.room].b = bl;
            lv->link[bl].a = p.room;
          }
        }

        break;
      } else if (! lv->link[room].r) {
        lv->link[room].r = p.room;
        lv->link[p.room].l = room;

        a = lv->link[room].a;
        if (a) {
          ar = lv->link[a].r;
          if (ar) {
            lv->link[p.room].a = ar;
            lv->link[ar].b = p.room;
          }
        }

        b = lv->link[room].b;
        if (b) {
          br = lv->link[b].r;
          if (br) {
            lv->link[p.room].b = br;
            lv->link[br].a = p.room;
          }
        }

        break;
      } else if (! lv->link[room].a) {
        lv->link[room].a = p.room;
        lv->link[p.room].b = room;

        l = lv->link[room].l;
        if (l) {
          la = lv->link[l].a;
          if (la) {
            lv->link[p.room].l = la;
            lv->link[la].r = p.room;
          }
        }

        r = lv->link[room].r;
        if (r) {
          ra = lv->link[r].a;
          if (ra) {
            lv->link[p.room].r = ra;
            lv->link[ra].l = p.room;
          }
        }
        break;
      } else if (! lv->link[room].b) {
        lv->link[room].b = p.room;
        lv->link[p.room].a = room;

        l = lv->link[room].l;
        if (l) {
          lb = lv->link[l].b;
          if (lb) {
            lv->link[p.room].l = lb;
            lv->link[lb].r = p.room;
          }
        }

        r = lv->link[room].r;
        if (r) {
          rb = lv->link[r].b;
          if (rb) {
            lv->link[p.room].r = rb;
            lv->link[rb].l = p.room;
          }
        }

        break;
      }
    }
  }
}

void
play_consistency_level (void)
{
  init_consistency_level ();
  play_level (&consistency_level);
}
