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

#include <error.h>
#include <stdio.h>
#include "pos.h"
#include "level.h"
#include "physics.h"

static void fix_single_walls_at_place_0 (struct pos p);
static void fix_impenetrable_row (struct pos p);
static void fix_rigid_con_no_floor_top (struct pos p);
static void fix_door_adjacent_to_wall_or_door (struct pos p);
static void fix_broken_floor_lacking_no_floor_on_top (struct pos p);
static void fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos p);
static void fix_adjacent_itens (struct pos p);
static void fix_item_on_non_normal_floor (struct pos p);
static void fix_sword_at_right_of_wall_or_door (struct pos p);
static void fix_door_lacking_event (struct pos p);
static bool is_rigid_con (struct pos p);
static bool is_there_event_handler (int e);

void
fix_level ()
{
  struct pos p;

  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        fix_door_lacking_event (p);
        fix_single_walls_at_place_0 (p);
        fix_impenetrable_row (p);
        fix_rigid_con_no_floor_top (p);
        fix_door_adjacent_to_wall_or_door (p);
        fix_broken_floor_lacking_no_floor_on_top (p);
        fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (p);
        fix_adjacent_itens (p);
        fix_item_on_non_normal_floor (p);
        fix_sword_at_right_of_wall_or_door (p);
      }
}

/* Important: the corrections bellow are all made for two perspectives
   (but just one when symmetrical), because we don't assume levels
   have consistent linking (if room A is at left from room B's
   perspective, it doesn't imply room B is necessarily at right from
   room A's perspective).  If this wasn't the case, we would get an
   inconsistent behavior, because the correction algorithms would only
   work from the perspective of the corrected room, and not
   necessairly from any other adjacent rooms.  Summarizing: without
   consistent linking the space is relative, and we shoud account for
   this. */

/* consistency: wall can't be single at place 0, otherwise the kid
   disappears from view behind it, or him might view room 0, what may
   lead to several problems, room on-demand local linking consistency
   resolution being one example */
void
fix_single_walls_at_place_0 (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);

  /* wall's perspective */
  if (p.place == 0
      && c->fg == WALL
      && cl->fg != WALL) cl->fg = WALL;

  /* wall left's perspective */
  if (p.place == 9
      && c->fg != WALL
      && cr->fg == WALL) c->fg = WALL;
}


/* consistency: walls should delimit accessible places or walls */
void
fix_impenetrable_row (struct pos p)
{
  struct pos q;
  int i = 1;
  bool imp = false;

  struct con *c = con (p);
  if (c->fg == WALL)
    for (i = 1, q = prel (p, 0, i); i < PLACES; q = prel (p, 0, ++i)) {
      struct con *ca = crel (q, -1, +0);
      struct con *cq = con (q);

      if (cq->fg == WALL) {
        if (i >= 2) imp = true;
        break;
      }

      if (ca->fg == NO_FLOOR
          || ca->fg == LOOSE_FLOOR
          || cq->fg == NO_FLOOR
          || cq->fg == LOOSE_FLOOR) break;
    }

  if (imp)
    for (i = 1, q = prel (p, 0, i); i < PLACES; q = prel (p, 0, ++i)) {

      if (con (q)->fg == WALL) break;

      con (q)->fg = WALL;
      con (q)->bg = NO_BG;
      con (q)->ext.item = NO_ITEM;
    }
}

/* consistency: rigid constructions (pillar, wall, door) must have
   something non-traversable lying on it */
static void
fix_rigid_con_no_floor_top (struct pos p)
{
  struct pos pa = prel (p, -1, 0);
  struct con *ca = con (pa);

  /* rigid construction's perspective */
  if (is_rigid_con (p) &&
      (ca->fg == NO_FLOOR
       || ca->fg == LOOSE_FLOOR)) ca->fg = FLOOR;

  /* traversable's perspective */
  struct pos pb = prel (p, +1, 0);
  if ((con (p)->fg == NO_FLOOR
       || con (p)->fg  == LOOSE_FLOOR)
      && is_rigid_con (pb)) con (p)->fg = FLOOR;
}

/* consistency: doors should not face a wall nor another door */
static void
fix_door_adjacent_to_wall_or_door (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);

  /* DOOR's perspective*/
  if (c->fg == DOOR &&
      (cr->fg == WALL || cr->fg == DOOR
       || cl->fg == WALL || cl->fg == DOOR)) c->fg = FLOOR;

  /* WALL's perspective */
  if (c->fg == WALL
      && (cr->fg == DOOR || cl->fg == DOOR)) c->fg = FLOOR;
}

/* consistency: broken floors must have no construction above them */
static void
fix_broken_floor_lacking_no_floor_on_top (struct pos p)
{
  struct con *c = con (p);
  struct con *ca = crel (p, -1, 0);
  struct con *cb = crel (p, +1, 0);

  /* broken floor's perspective */
  if (c->fg == BROKEN_FLOOR && ca->fg != NO_FLOOR) c->fg = FLOOR;

  /* non-no-floor's perspective */
  if (c->fg != NO_FLOOR && cb->fg == BROKEN_FLOOR) cb->fg = FLOOR;
}

/* conssitency: skeleton and spikes floors must not have no or loose
   floor at left, because there is no corner floor sprites to render
   the perspective when the kid is climbing them */
static void
fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);

  /* skeleton and spike's perspective */
  if ((c->fg == SKELETON_FLOOR
       || c->fg == SPIKES_FLOOR)
      && (cl->fg == NO_FLOOR
          || cl->fg == LOOSE_FLOOR)) cl->fg = FLOOR;

  /* no and loose floor's perspective  */
  if ((c->fg == NO_FLOOR
       || c->fg == LOOSE_FLOOR)
      && (cr->fg == SKELETON_FLOOR
          || cr->fg == SPIKES_FLOOR)) cr->fg = FLOOR;
}

/* consistency: itens can't be adjacent */
static void
fix_adjacent_itens (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);

  /* item's perspective (symmetrical) */
  if (((cl->fg == FLOOR && cl->ext.item != NO_ITEM)
       || (cr->fg == FLOOR && cr->ext.item != NO_ITEM))
      && c->fg != OPENER_FLOOR
      && c->fg != CLOSER_FLOOR) c->ext.item = NO_ITEM;
}

/* consistency: itens can only lay in normal floors */
static void
fix_item_on_non_normal_floor (struct pos p)
{
  struct con *c = con (p);

  /* non-no-floor's perspective (symmetrical) */
  if (c->fg != FLOOR
      && c->fg != OPENER_FLOOR
      && c->fg != CLOSER_FLOOR) c->ext.item = NO_ITEM;
}

/* consistency: swords can't be placed on the right of walls or
   doors */
static void
fix_sword_at_right_of_wall_or_door (struct pos p)
{
  struct con *c = con (p);
  struct con *cl = crel (p, 0, -1);
  struct con *cr = crel (p, 0, +1);

  /* sword's perspective */
  if (c->fg == FLOOR
      && c->ext.item == SWORD
      && (cl->fg == WALL
          || cl->fg == DOOR)) c->ext.item = NO_ITEM;

  /* wall and door's perspective  */
  if ((c->fg == WALL
       || c->fg == DOOR)
      && cr->fg == FLOOR
      && cr->ext.item == SWORD) c->ext.item = NO_ITEM;
}

/* consistency: doors should have an associated event */
static void
fix_door_lacking_event (struct pos p)
{
  int i;

  struct con *c = con (p);
  if (c->fg == DOOR) {
    for (i = 0; i < EVENTS; i++)
      if (peq (level->event[i].p, p)
          && is_there_event_handler (i)) return;

    c->fg = FLOOR;
    c->bg = NO_BG;
    c->ext.item = NO_ITEM;
  }
}

void
make_links_locally_consistent (int prev_room, int current_room)
{
  if (roomd (prev_room, LEFT) == current_room)
    level->link[current_room].r = prev_room;
  else if (roomd (prev_room, RIGHT) == current_room)
    level->link[current_room].l = prev_room;
  else if (roomd (prev_room, ABOVE) == current_room)
    level->link[current_room].b = prev_room;
  else if (roomd (prev_room, BELOW) == current_room)
    level->link[current_room].a = prev_room;
  else
    error (-1, 0, "%s: internal inconsistency at room linking",
           __func__);
}

static bool
is_there_event_handler (int e)
{
  struct pos p;

  for (p.room = 1; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        if ((con (p)->fg == OPENER_FLOOR
             || con (p)->fg == CLOSER_FLOOR)
            && con (p)->ext.event == e) return true;
          }
  return false;
}

static bool
is_rigid_con (struct pos p)
{
  enum confg fg = con (p)->fg;
  return fg == PILLAR || fg == WALL || fg == DOOR;
}
