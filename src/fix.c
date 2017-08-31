/*
  fix.c -- fix module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

#include "mininim.h"

union tile_state *
fix_tile (struct pos *p, void *data)
{
  struct pos pb; prel (p, &pb, +1, 0);

  /* traversable on top of wall -> floor */
  if (is_critical (p)
      && (fg (&pb) == WALL || fg (&pb) == PILLAR))
    set_fg (p, FLOOR);

  return NULL;
}

/* ------------------------------------------------------------------- */



/* fix: doors should not face a wall nor another door */
struct pos *
fix_door_adjacent_to_wall_or_door (struct pos *p)
{
  enum tile_fg f = fg (p);
  enum tile_fg fl = fg_rel (p, 0, -1);
  enum tile_fg fr = fg_rel (p, 0, +1);

  /* DOOR's perspective*/
  if (f == DOOR &&
      (fr == WALL || fr == DOOR || fl == WALL || fl == DOOR))
    set_fg (p, FLOOR);

  /* WALL's perspective */
  if (f == WALL && (fr == DOOR || fl == DOOR))
    set_fg (p, FLOOR);

  return p;
}

/* fix: skeleton and spikes floors can't be a hangable
   tile at left because there is no corner floor sprites to
   render the perspective when the kid is climbing them */
struct pos *
fix_skeleton_or_spikes_floor_with_no_or_loose_floor_at_left (struct pos *p)
{
  enum tile_fg f = fg (p);

  struct pos ph; prel (p, &ph, +1, -1);

  /* skeleton and spike's perspective */
  if ((f == SKELETON_FLOOR || f == SPIKES_FLOOR)
      && is_hangable (&ph, RIGHT))
    set_fg_rel (p, +0, -1, FLOOR);

  /* hangable position perspective  */
  enum tile_fg t = get_hanged_tile (p, RIGHT);
  if (is_hangable (p, RIGHT)
      && (t == SKELETON_FLOOR || t == SPIKES_FLOOR))
    set_fg_rel (p, -1, +0, FLOOR);

  return p;
}

/* fix: itens can't be adjacent */
struct pos *
fix_adjacent_itens (struct pos *p)
{
  enum tile_fg f = fg (p);
  enum tile_fg fl = fg_rel (p, 0, -1);
  enum tile_fg fr = fg_rel (p, 0, +1);

  int el = ext_rel (p, 0, -1);
  int er = ext_rel (p, 0, +1);

  /* item's perspective (symmetrical) */
  if (((item_fg_cs (fl) && el != NO_ITEM)
       || (item_fg_cs (fr) && er != NO_ITEM))
      && item_fg_cs (f)) set_ext (p, NO_ITEM);

  return p;
}

/* fix: doors should have an associated event and opener floor */
struct pos *
fix_door_lacking_opener (struct pos *p)
{
  int i;

  enum tile_fg f = fg (p);

  if (f == DOOR || f == LEVEL_DOOR) {
    for (i = 0; i < EVENTS; i++)
      if (peq (&event (p->l, i)->p, p)
          && is_there_event_handler (p->l, i)) return p;

    /* fprintf (stderr, "%s: replaced %s by %s at pos (%i, %i, %i)\n", */
    /*          __func__, "DOOR", "FLOOR", p->room, p->floor, p->place); */

    set_tile (p, FLOOR, BRICKS_5, NO_ITEM, NO_FAKE);
  }

  return p;
}

/* fix: opener and closer floors should have an associated
   event and door */
struct pos *
fix_opener_or_closer_lacking_door (struct pos *p)
{
  enum tile_fg f = fg (p);

  if (f == OPENER_FLOOR || f == CLOSER_FLOOR) {
    int i = ext (p);
    struct level_event *le;
    do {
      le = event (p->l, i);
      if (is_valid_pos (&le->p)
          && (fg (&le->p) == DOOR || fg (&le->p) == LEVEL_DOOR))
        return p;
    } while (le->next && ++i < EVENTS);

    /* fprintf (stderr, "%s: replaced %s (event %i) by %s at pos (%i, %i, %i)\n", */
    /*          __func__, fg (p) == OPENER_FLOOR ? "OPENER_FLOOR" : "CLOSER_FLOOR", */
    /*          ext (p), "FLOOR", p->room, p->floor, p->place); */

    set_tile (p, FLOOR, BRICKS_5, NO_ITEM, NO_FAKE);
  }

  return p;
}

/* fix: wall, pillars, big pillars, doors and level doors shouldn't have
   background */
struct pos *
fix_tile_fg_which_should_not_have_tile_bg (struct pos *p)
{
  enum tile_fg f = fg (p);
  if (f == WALL || f == PILLAR || f == BIG_PILLAR_TOP || f == BIG_PILLAR_BOTTOM
      || f == DOOR || f == LEVEL_DOOR)
    set_bg (p, BRICKS_5);

  return p;
}

/* fix: big pillar bottom must have big pillar top over it and
   vice-versa */
struct pos *
fix_partial_big_pillar (struct pos *p)
{
  enum tile_fg f = fg (p);
  enum tile_fg fa = fg_rel (p, -1, +0);
  enum tile_fg fb = fg_rel (p, +1, +0);

  if ((f == BIG_PILLAR_BOTTOM && fa != BIG_PILLAR_TOP)
      || (f == BIG_PILLAR_TOP && fb != BIG_PILLAR_BOTTOM))
    set_fg (p, FLOOR);

  return p;
}

void
make_links_locally_consistent (struct level *l, int prev_room, int current_room)
{
  if (roomd (l, prev_room, LEFT) == room_val (current_room))
    llink (l, current_room)->r = room_val (prev_room);
  else if (roomd (l, prev_room, RIGHT) == current_room)
    llink (l, current_room)->l = room_val (prev_room);
  else if (roomd (l, prev_room, ABOVE) == current_room)
    llink (l, current_room)->b = room_val (prev_room);
  else if (roomd (l, prev_room, BELOW) == current_room)
    llink (l, current_room)->a = room_val (prev_room);
}

bool
is_there_event_handler (struct level *l, int e)
{
  int i = 0;
  struct pos p; new_pos (&p, l, -1, -1, -1);
  for (p.room = 1; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {
        if (fg (&p) == OPENER_FLOOR) {
          i = ext (&p);
          do {
            if (i == e) return true;
          } while (event (p.l, i++)->next && i < EVENTS);
        }
      }
  return false;
}

void
fix_room_0 (struct level *l)
{
  link_room (l, 0, 0, LEFT);
  link_room (l, 0, 0, RIGHT);
  link_room (l, 0, 0, ABOVE);
  link_room (l, 0, 0, BELOW);

  struct pos p; new_pos (&p, l, 0, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      set_tile (&p, WALL, BRICKS_5, NO_ITEM, NO_FAKE);
}

/* fix rooms above room 0 in case they have traversable tiles at the
   bottom */
void
fix_traversable_above_room_0 (struct level *l)
{
  struct pos p; new_pos (&p, l, -1, 2, -1);
  for (p.room = 1; p.room < ROOMS; p.room++)
    for (p.place = 0; p.place < PLACES; p.place++) {
      if (roomd (p.l, p.room, BELOW) != 0) continue;
      if (is_critical (&p)) set_fg (&p, SPIKES_FLOOR);
    }
}



/****************
 * ROOM LINKING *
 ****************/

void
make_reciprocal_link (struct level *l, int room0, int room1, enum dir dir)
{
  link_room (l, room0, room1, dir);
  link_room (l, room1, room0, opposite_dir (dir));
}

void
make_link_locally_unique (struct level *l, int room, enum dir dir)
{
  if (dir != LEFT && roomd (l, room, LEFT) == roomd (l, room, dir))
    link_room (l, room, 0, LEFT);
  if (dir != RIGHT && roomd (l, room, RIGHT) == roomd (l, room, dir))
    link_room (l, room, 0, RIGHT);
  if (dir != ABOVE && roomd (l, room, ABOVE) == roomd (l, room, dir))
    link_room (l, room, 0, ABOVE);
  if (dir != BELOW && roomd (l, room, BELOW) == roomd (l, room, dir))
    link_room (l, room, 0, BELOW);
}

void
make_link_globally_unique (struct level *l, int room, enum dir dir)
{
  int i;
  for (i = 1; i < ROOMS; i++) {
    if (room != i && roomd (l, i, dir) == roomd (l, room, dir))
      link_room (l, i, 0, dir);
  }
}

void
make_semi_consistent_link (struct level *l, int room0, int room1, enum dir dir)
{
  make_reciprocal_link (l, room0, room1, dir);

  make_link_locally_unique (l, room0, dir);
  make_link_locally_unique (l, room1, opposite_dir (dir));

  make_link_globally_unique (l, room0, dir);
  make_link_globally_unique (l, room1, opposite_dir (dir));
}

void
make_link_adjacency_bound (struct level *l, int room, enum dir dir)
{
  int r = roomd (l, room, dir);
  int dir0 = perpendicular_dir (dir, 0);
  int dir1 = perpendicular_dir (dir, 1);

  int room0a = roomd (l, room, dir0);
  int room0b = roomd (l, r, dir0);

  int room1a = roomd (l, room, dir1);
  int room1b = roomd (l, r, dir1);

  make_semi_consistent_link (l, room0a, room0b, dir);
  make_semi_consistent_link (l, room1a, room1b, dir);
}

void
exchange_rooms  (struct level *l, int room0, int room1)
{
  int r0l = roomd (l, room0, LEFT);
  int r0r = roomd (l, room0, RIGHT);
  int r0a = roomd (l, room0, ABOVE);
  int r0b = roomd (l, room0, BELOW);

  int r0lr = roomd (l, r0l, RIGHT);
  int r0rl = roomd (l, r0r, LEFT);
  int r0ab = roomd (l, r0a, BELOW);
  int r0ba = roomd (l, r0b, ABOVE);

  int r1l = roomd (l, room1, LEFT);
  int r1r = roomd (l, room1, RIGHT);
  int r1a = roomd (l, room1, ABOVE);
  int r1b = roomd (l, room1, BELOW);

  int r1lr = roomd (l, r1l, RIGHT);
  int r1rl = roomd (l, r1r, LEFT);
  int r1ab = roomd (l, r1a, BELOW);
  int r1ba = roomd (l, r1b, ABOVE);

  link_room (l, room0, r1l == room0 ? room1 : r1l, LEFT);
  if (r1l != room0 && r1lr == room1) link_room (l, r1l, room0, RIGHT);
  link_room (l, room0, r1r == room0 ? room1 : r1r, RIGHT);
  if (r1r != room0 && r1rl == room1) link_room (l, r1r, room0, LEFT);
  link_room (l, room0, r1a == room0 ? room1 : r1a, ABOVE);
  if (r1a != room0 && r1ab == room1) link_room (l, r1a, room0, BELOW);
  link_room (l, room0, r1b == room0 ? room1 : r1b, BELOW);
  if (r1b != room0 && r1ba == room1) link_room (l, r1b, room0, ABOVE);

  link_room (l, room1, r0l == room1 ? room0 : r0l, LEFT);
  if (r0l != room1 && r0lr == room0) link_room (l, r0l, room1, RIGHT);
  link_room (l, room1, r0r == room1 ? room0 : r0r, RIGHT);
  if (r0r != room1 && r0rl == room0) link_room (l, r0r, room1, LEFT);
  link_room (l, room1, r0a == room1 ? room0 : r0a, ABOVE);
  if (r0a != room1 && r0ab == room0) link_room (l, r0a, room1, BELOW);
  link_room (l, room1, r0b == room1 ? room0 : r0b, BELOW);
  if (r0b != room1 && r0ba == room0) link_room (l, r0b, room1, ABOVE);
}

void
circular_linking (struct level *lv)
{
  int room, a, b, l, r, al, bl, ar, br, la, ra, lb, rb;

  llink (lv, 1)->l = 2;
  llink (lv, 2)->r = 1;

  struct pos p; new_pos (&p, lv, -1, -1, -1);
  for (p.room = 3; p.room < ROOMS; p.room++) {
    for (room = 1; room < ROOMS; room++) {
      if (p.room == room) continue;

      if (! llink (lv, room)->l) {
        llink (lv, room)->l = p.room;
        llink (lv, p.room)->r = room;

        a = llink (lv, room)->a;
        if (a) {
          al = llink (lv, a)->l;
          if (al) {
            llink (lv, p.room)->a = al;
            llink (lv, al)->b = p.room;
          }
        }

        b = llink (lv, room)->b;
        if (b) {
          bl = llink (lv, b)->l;
          if (bl) {
            llink (lv, p.room)->b = bl;
            llink (lv, bl)->a = p.room;
          }
        }

        break;
      } else if (! llink (lv, room)->r) {
        llink (lv, room)->r = p.room;
        llink (lv, p.room)->l = room;

        a = llink (lv, room)->a;
        if (a) {
          ar = llink (lv, a)->r;
          if (ar) {
            llink (lv, p.room)->a = ar;
            llink (lv, ar)->b = p.room;
          }
        }

        b = llink (lv, room)->b;
        if (b) {
          br = llink (lv, b)->r;
          if (br) {
            llink (lv, p.room)->b = br;
            llink (lv, br)->a = p.room;
          }
        }

        break;
      } else if (! llink (lv, room)->a) {
        llink (lv, room)->a = p.room;
        llink (lv, p.room)->b = room;

        l = llink (lv, room)->l;
        if (l) {
          la = llink (lv, l)->a;
          if (la) {
            llink (lv, p.room)->l = la;
            llink (lv, la)->r = p.room;
          }
        }

        r = llink (lv, room)->r;
        if (r) {
          ra = llink (lv, r)->a;
          if (ra) {
            llink (lv, p.room)->r = ra;
            llink (lv, ra)->l = p.room;
          }
        }
        break;
      } else if (! llink (lv, room)->b) {
        llink (lv, room)->b = p.room;
        llink (lv, p.room)->a = room;

        l = llink (lv, room)->l;
        if (l) {
          lb = llink (lv, l)->b;
          if (lb) {
            llink (lv, p.room)->l = lb;
            llink (lv, lb)->r = p.room;
          }
        }

        r = llink (lv, room)->r;
        if (r) {
          rb = llink (lv, r)->b;
          if (rb) {
            llink (lv, p.room)->r = rb;
            llink (lv, rb)->l = p.room;
          }
        }

        break;
      }
    }
  }
}
