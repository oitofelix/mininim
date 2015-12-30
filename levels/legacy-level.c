/*
  legacy-level.c -- legacy level module;

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
#include <stdio.h>
#include <time.h>
#include "kernel/video.h"
#include "kernel/random.h"
#include "engine/level.h"
#include "engine/kid/kid.h"
#include "levels/legacy-level.h"

static struct level legacy_level;

static struct con room_0[FLOORS][PLACES] =
  {{{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}},
   {{WALL}, {WALL}, {WALL}, {WALL}, {WALL},
    {WALL}, {WALL}, {WALL}, {WALL}, {WALL}}};

static struct {
  uint8_t foretable[LROOMS][FLOORS][PLACES];
  uint8_t backtable[LROOMS][FLOORS][PLACES];
  uint8_t door_1[LEVENTS];
  uint8_t door_2[LEVENTS];
  uint8_t link[LROOMS][4];
  uint8_t unknown_1[64];
  uint8_t start_position[3];
  uint8_t unknown_2[3];
  uint8_t unknown_3;
  uint8_t guard_location[LROOMS];
  uint8_t guard_direction[LROOMS];
  uint8_t unknown_4a[LROOMS];
  uint8_t unknown_4b[LROOMS];
  uint8_t guard_skill[LROOMS];
  uint8_t unknown_4c[LROOMS];
  uint8_t guard_color[LROOMS];
  uint8_t unknown_4d[16];
  uint8_t signature[2];
} lv;

static enum ltile get_tile (struct pos *p);
static enum lgroup get_group (enum ltile t);

static void next_level (int lv, struct pos *exit_door_pos);
static void start (void);

void
play_legacy_level (void)
{
  next_level (1, NULL);
  play_level (&legacy_level);
}

static void
start (void)
{
  create_kid (NULL);
  /* create_kid (&kid[0]); */
}

static void
next_level (int number, struct pos *exit_door_pos)
{
  FILE *lvf;
  lvf = fopen ("data/legacy-levels/02", "r");
  fread (&lv, sizeof (lv), 1, lvf);
  fclose (lvf);

  struct pos p;

  memset (&legacy_level, 0, sizeof (legacy_level));
  legacy_level.start = start;
  legacy_level.number = number;
  memcpy (&legacy_level.con[0], &room_0, sizeof (room_0));

  for (p.room = 1; p.room <= LROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        struct con *c = &legacy_level.con[p.room][p.floor][p.place];

        uint8_t f = lv.foretable[p.room - 1][p.floor][p.place];
        uint8_t b = lv.backtable[p.room - 1][p.floor][p.place];

        bool mlf = f & 0x20 ? true : false;   /* loose floor modifier */
        enum ltile t = get_tile (&p);
        enum lgroup g = get_group (t);

        switch (t) {
        case LT_EMPTY: c->fg = NO_FLOOR; break;
        case LT_FLOOR: c->fg = FLOOR; break;
        case LT_SPIKES: c->fg = SPIKES_FLOOR; break;
        case LT_PILLAR: c->fg = PILLAR; break;
        case LT_GATE: c->fg = DOOR; break;
        case LT_STUCK_BUTTON:
          printf ("(%i, %i, %i, %i): LT_STUCK_BUTTON\n",
                  number, p.room, p.floor, p.place);
          break;
        case LT_DROP_BUTTON: c->fg = CLOSER_FLOOR; break;
        case LT_TAPESTRY:
          printf ("(%i, %i, %i, %i): LT_TAPESTRY\n",
                  number, p.room, p.floor, p.place);
          break;
        case LT_BOTTOM_BIG_PILLAR: c->fg = BIG_PILLAR_BOTTOM; break;
        case LT_TOP_BIG_PILLAR: c->fg = BIG_PILLAR_TOP; break;
        case LT_POTION: c->fg = FLOOR; break;
        case LT_LOOSE_BOARD: c->fg = LOOSE_FLOOR; break;
        case LT_TAPESTRY_TOP:
          printf ("(%i, %i, %i, %i): LT_TAPESTRY_TOP\n",
                  number, p.room, p.floor, p.place);
          break;
        case LT_MIRROR: c->fg = MIRROR; break;
        case LT_DEBRIS: c->fg = BROKEN_FLOOR; break;
        case LT_RAISE_BUTTON: c->fg = OPENER_FLOOR; break;
        case LT_EXIT_LEFT: c->fg = FLOOR; c->bg = NO_BRICKS; break;
        case LT_EXIT_RIGHT: c->fg = LEVEL_DOOR; break;
        case LT_CHOPPER: c->fg = CHOPPER; break;
        case LT_TORCH: c->fg = FLOOR; c->bg = TORCH; break;
        case LT_WALL: c->fg = WALL; break;
        case LT_SKELETON: c->fg = SKELETON_FLOOR; break;
        case LT_SWORD: c->fg = FLOOR; c->ext.item = SWORD; break;
        case LT_BALCONY_LEFT:
          printf ("(%i, %i, %i, %i): LT_BALCONY_LEFT\n",
                  number, p.room, p.floor, p.place);
          break;
        case LT_BALCONY_RIGHT:
          printf ("(%i, %i, %i, %i): LT_BALCONY_RIGHT\n",
                  number, p.room, p.floor, p.place);
          break;
        case LT_LATTICE_PILLAR: c->fg = ARCH_BOTTOM; break;
        case LT_LATTICE_SUPPORT: c->fg = ARCH_TOP_MID; break;
        case LT_SMALL_LATTICE: c->fg = ARCH_TOP_SMALL; break;
        case LT_LATTICE_LEFT: c->fg = ARCH_TOP_LEFT; break;
        case LT_LATTICE_RIGHT: c->fg = ARCH_TOP_RIGHT; break;
        case LT_TORCH_WITH_DEBRIS: c->fg = BROKEN_FLOOR; c->bg = TORCH; break;
        case LT_NULL:
          printf ("(%i, %i, %i, %i): LT_NULL\n",
                  number, p.room, p.floor, p.place);
          break;
        default:
          error (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, t, number, p.room, p.floor, p.place);
        }

        switch (g) {
        case LG_NONE: break;
        case LG_FREE: break;
        case LG_SPIKE: break;
        case LG_GATE: break;
        case LG_TAPEST: break;
        case LG_POTION: break;
        case LG_TTOP: break;
        case LG_CHOMP: break;
        case LG_WALL: break;
        case LG_EXIT: break;
        case LG_EVENT:
          c->ext.event = b;
          printf ("event floor (%i, %i, %i, %i)\n",
                  p.room, p.floor, p.place, b);
          break;
        default:
          error (-1, 0, "%s: unknown tile group (%i) at position (%i, %i, %i, %i)",
                 __func__, g, number, p.room, p.floor, p.place);
        }
      }

  int i;
  for (i = 0; i < LEVENTS; i++) {
    struct level_event *e = &legacy_level.event[i];
    e->p.room = (lv.door_2[i] >> 3) | ((lv.door_1[i] & 0x60) >> 5);
    int l = lv.door_1[i] & 0x1F;
    e->p.floor = l / PLACES;
    e->p.place = l % PLACES;
    if (get_tile (&e->p) == LT_EXIT_LEFT) e->p.place++;
    e->next = lv.door_1[i] & 0x80 ? false : true;
  }

  for (p.room = 1; p.room <= LROOMS; p.room++) {
    legacy_level.link[p.room].l = lv.link[p.room - 1][LD_LEFT];
    legacy_level.link[p.room].r = lv.link[p.room - 1][LD_RIGHT];
    legacy_level.link[p.room].a = lv.link[p.room - 1][LD_ABOVE];
    legacy_level.link[p.room].b = lv.link[p.room - 1][LD_BELOW];
  }


}

static enum ltile
get_tile (struct pos *p)
{
  return lv.foretable[p->room - 1][p->floor][p->place] & 0x1F;
}

static enum lgroup
get_group (enum ltile t)
{
  switch (t) {
  case LT_PILLAR: case LT_STUCK_BUTTON: case LT_BOTTOM_BIG_PILLAR:
  case LT_TOP_BIG_PILLAR: case LT_LOOSE_BOARD: case LT_MIRROR:
  case LT_DEBRIS: case LT_TORCH: case LT_SKELETON: case LT_SWORD:
  case LT_BALCONY_LEFT: case LT_BALCONY_RIGHT: case LT_LATTICE_PILLAR:
  case LT_LATTICE_SUPPORT: case LT_SMALL_LATTICE: case LT_LATTICE_LEFT:
  case LT_LATTICE_RIGHT: case LT_TORCH_WITH_DEBRIS: case LT_NULL:
    return LG_NONE;
  case LT_EMPTY: case LT_FLOOR: return LG_FREE;
  case LT_SPIKES: return LG_SPIKE;
  case LT_GATE: return LG_GATE;
  case LT_TAPESTRY: return LG_TAPEST;
  case LT_POTION: return LG_POTION;
  case LT_TAPESTRY_TOP: return LG_TTOP;
  case LT_CHOPPER: return LG_CHOMP;
  case LT_WALL: return LG_WALL;
  case LT_EXIT_LEFT: case LT_EXIT_RIGHT: return LG_EXIT;
  case LT_DROP_BUTTON: case LT_RAISE_BUTTON: return LG_EVENT;
  default:
    error (-1, 0, "%s: unknown tile (%i)", __func__, t);
  }
  return LG_NONE;
}
