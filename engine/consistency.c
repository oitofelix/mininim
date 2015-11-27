/*
  consistency.c -- consistency module;

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
#include "pos.h"
#include "level.h"
#include "physics.h"

static void fix_rigid_con_no_floor_top (struct pos p);
static void fix_door_adjacent_to_wall_or_door (struct pos p);
static void fix_broken_floor_lacking_no_floor_on_top (struct pos p);
static void fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos p);
static void fix_adjacent_itens (struct pos p);
static void fix_item_on_non_normal_floor (struct pos p);
static void fix_sword_at_right_of_wall_or_door (struct pos p);
static bool is_rigid_con (struct pos p);

void
fix_level ()
{
  struct pos p;

  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        fix_rigid_con_no_floor_top (p);
        fix_door_adjacent_to_wall_or_door (p);
        fix_broken_floor_lacking_no_floor_on_top (p);
        fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (p);
        fix_adjacent_itens (p);
        fix_item_on_non_normal_floor (p);
        fix_sword_at_right_of_wall_or_door (p);
      }
}

/* consistency: rigid constructions (pillar, wall, door) must have
   something non-traversable lying on it */
static void
fix_rigid_con_no_floor_top (struct pos p)
{
  struct con *ca = crel (p, -1, 0);
  if (is_rigid_con (p) &&
      (ca->fg == NO_FLOOR
       || ca->fg == LOOSE_FLOOR)) ca->fg = FLOOR;
}

/* consistency: doors should not face a wall nor another door */
static void
fix_door_adjacent_to_wall_or_door (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);
  if (c->fg == DOOR &&
      (cr->fg == WALL || cr->fg == DOOR
       || cl->fg == WALL || cl->fg == DOOR)) c->fg = FLOOR;
}

/* conssitency: broken floors must have no construction above them */
static void
fix_broken_floor_lacking_no_floor_on_top (struct pos p)
{
  struct con *c = con (p);
  struct con *ca = crel (p, -1, 0);
  if (c->fg == BROKEN_FLOOR && ca->fg != NO_FLOOR) c->fg = FLOOR;
}

/* conssitency: skeleton and spikes floors must not have no or loose
   floor at left, because there is no corner floor sprites to render
   the perspective when the kid is climbing them */
static void
fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  if ((c->fg == SKELETON_FLOOR
       || c->fg == SPIKES_FLOOR)
      && (cl->fg == NO_FLOOR
          || cl->fg == LOOSE_FLOOR)) cl->fg = FLOOR;
}

/* consistency: itens can't be adjacent */
static void
fix_adjacent_itens (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  if (cl->fg == FLOOR && cl->ext.item != NO_ITEM
      && c->fg != OPENER_FLOOR
      && c->fg != CLOSER_FLOOR) c->ext.item = NO_ITEM;
}

/* consistency: itens can only lay in normal floors */
static void
fix_item_on_non_normal_floor (struct pos p)
{
  struct con *c = con (p);
  if (c->fg != FLOOR
      && c->fg != OPENER_FLOOR
      && c->fg != CLOSER_FLOOR
      && c->ext.item != NO_ITEM) c->ext.item = NO_ITEM;
}

/* consistency: swords can't be placed on the right of walls or
   doors */
static void
fix_sword_at_right_of_wall_or_door (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  if (c->fg == FLOOR
      && c->ext.item == SWORD
      && (cl->fg == WALL
          || cl->fg == DOOR)) c->ext.item = NO_ITEM;
}

static bool
is_rigid_con (struct pos p)
{
  enum confg fg = con (p)->fg;
  return fg == PILLAR || fg == WALL || fg == DOOR;
}
