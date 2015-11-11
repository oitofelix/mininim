/*
  physics.c -- physics module;

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
#include <error.h>
#include "kernel/random.h"
#include "level.h"
#include "kid.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "pos.h"
#include "door.h"
#include "physics.h"

struct pos loose_floor_pos;
struct pos hang_pos;
enum construct_fg collision_construct;

struct construct
construct (struct pos p)
{
  p = npos (p);
  return level->construct[p.room][p.floor][p.place];
}

struct construct
construct_rel (struct pos p, int floor, int place)
{
  p.floor += floor;
  p.place += place;
  return construct (p);
}

void
set_construct_fg (struct pos p, enum construct_fg fg)
{
  p = npos (p);
  level->construct[p.room][p.floor][p.place].fg = fg;
}

bool
is_colliding (struct anim a)
{
  if (! a.collision) return false;

  struct coord tf = coord_tf (a);
  struct coord tb = coord_tb (a);
  struct coord mt = coord_mt (a);

  struct pos ptf = pos (tf);
  struct pos ptb = pos (tb);
  struct pos pmt = pos (mt);
  struct pos pmf = pos (coord_mf (a));
  struct pos pbf = pos (coord_bf (a));

  struct construct ctf = construct (ptf);
  struct construct ctb = construct (ptb);
  struct construct cmf = construct (pmf);
  struct construct cbf = construct (pbf);
  struct construct cmt = construct (pmt);

  if (ctf.fg == WALL || cmf.fg == WALL || cbf.fg == WALL) {
    if (a.id == &kid && is_kid_vjump ()) return false;
    if (a.id == &kid && is_kid_start_vjump ()) return false;
    collision_construct = WALL;
    return true;
  }

  int dy = 10;

  /* if (a.draw == draw_kid_couch) dy = 20; */

  if (ctf.fg == DOOR && a.dir == LEFT
      && tf.y <= door_grid_tip_y (ptf) - dy
      && ! peq (ptf, pmt)) {
    collision_construct = DOOR;
    return true;
  }


  if (cmt.fg == DOOR && a.dir == RIGHT
      && mt.y <= door_grid_tip_y (pmt) - dy
      && ! peq (pmt, ptf)) {
    collision_construct = DOOR;
    return true;
  }

  struct coord tfk = coord_tf (kid);
  struct pos ptfk = pos (tfk);
  struct construct ctfk = construct (ptfk);

  /* fix bug in which the kid would pass through a closed door */
  if (a.frame == kid_turn_run_05 && a.dir == LEFT
      && cmt.fg == DOOR) {
    collision_construct = DOOR;
    return true;
  }

  if (a.frame == kid_turn_run_05 && a.dir == RIGHT
      && ctb.fg == DOOR && ! peq (ptb, ptf)) {
    collision_construct = DOOR;
    return true;
  }

  /* if (a.id == &kid */
  /*     && ctf.fg == DOOR && a.dir == LEFT */
  /*     && tf.y <= door_grid_tip_y (ptf) - dy) return true; */

  if (a.id == &kid
      && ctf.fg == DOOR && a.dir == LEFT
      && tf.y <= door_grid_tip_y (ptf) - dy
      && ! peq (ptf, ptfk)) {
    collision_construct = DOOR;
    return true;
  }

  if (a.id == &kid
      && ctfk.fg == DOOR && a.dir == RIGHT
      && tf.y <= door_grid_tip_y (ptfk) - dy
      && ! peq (ptfk, ptf)) {
    collision_construct = DOOR;
    return true;
  }

  return false;
}

bool
is_back_colliding (struct anim a)
{
  if (! a.back_collision) return false;

  struct coord tb = coord_tb (a);
  struct coord bb = coord_bb (a);
  struct coord mba = coord_mba (a);
  struct coord mt = coord_mt (a);

  struct pos ptb = pos (tb);
  struct pos pbb = pos (bb);
  struct pos pmba = pos (mba);
  struct pos pmt = pos (mt);

  struct construct ctb = construct (ptb);
  struct construct cbb = construct (pbb);
  struct construct cmba = construct (pmba);
  struct construct cmt = construct (pmt);

  if (ctb.fg == WALL) {
    collision_construct = WALL;
    return true;
  }

  int dy = 10;

  if (a.draw == draw_kid_couch) dy = 20;

  if (cmt.fg == DOOR && a.dir == LEFT
      && mt.y <= door_grid_tip_y (pmt) - dy
      && ! peq (pmt, ptb)) {
    collision_construct = DOOR;
    return true;
  }


  if (ctb.fg == DOOR && a.dir == RIGHT
      && tb.y <= door_grid_tip_y (ptb) - dy
      && ! peq (ptb, pmt)) {
    collision_construct = DOOR;
    return true;
  }

  return false;
}

int
dist_collision (struct anim a)
{
  int inc = (a.dir == LEFT) ? -1 : +1;
  int x = a.c.x;

  if (! is_colliding (a))
    while (! is_colliding (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x += inc;
  else
    while (is_colliding (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x -= inc;

  return inc * (a.c.x - x);
}

int
dist_back_collision (struct anim a)
{
  int inc = (a.dir == LEFT) ? +1 : -1;
  int x = a.c.x;

  if (! is_back_colliding (a))
    while (! is_back_colliding (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x += inc;
  else
    while (is_back_colliding (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x -= inc;

  return inc * (a.c.x - x);
}

bool
is_falling (struct anim a)
{
  if (! a.fall) return false;

  if (a.id == &kid && (is_kid_start_jump ())) return false;

  a.c.x += (a.dir == LEFT) ? +4 : -4;

  struct coord bf = coord_bf (a);
  struct pos pbf = pos (bf);
  struct construct cbf = construct (pbf);
  struct pos pbfn = pos (coord_bf (next_anim (a, a.frame, 0, 33)));

  if (cbf.fg == NO_FLOOR || pbf.floor == pbfn.floor) return true;

  /* needed because when hanging the kid's bottom front position
     coincides with the wall's */
  if (a.id == &kid && is_kid_hang ()
      && construct (hang_pos).fg == NO_FLOOR) return true;

  return false;
}

int
dist_fall (struct anim a)
{
  int inc = (a.dir == LEFT) ? -1 : +1;
  int x = a.c.x;

  if (! is_falling (a))
    while (! is_falling (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x += inc;
  else
    while (is_falling (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x -= inc;

  return inc * (a.c.x - x);
}

int
dist_next_place (struct anim a)
{
  struct pos p = pos (coord_tf (a));
  int inc = (a.dir == LEFT) ? -1 : +1;
  int x = a.c.x;

  while (p.place == pos (coord_tf (a)).place && abs (x - a.c.x) != PLACE_WIDTH)
    a.c.x += inc;

  return inc * (a.c.x - x);
}

int
dist_prev_place (struct anim a)
{
  struct pos p = pos (coord_tf (a));
  int inc = (a.dir == LEFT) ? +1 : -1;
  int x = a.c.x;

  while (p.place == pos (coord_tf (a)).place && abs (x - a.c.x) != PLACE_WIDTH)
    a.c.x += inc;

  return inc * (a.c.x - x);
}

bool
is_hitting_ceiling (struct anim a)
{
  if (! a.ceiling) return false;

  struct pos pt = (a.dir == LEFT)
    ? pos (coord_tb (a)) : pos (coord_tf (a));

  struct pos pbr = pos (coord_br (a));
  struct construct c = construct (pt);

  if (a.id == &kid && ! is_kid_vjump ()) return false;

  if (pt.floor != pbr.floor && c.fg != NO_FLOOR) return true;

  return false;
}

void
to_collision_edge (struct anim *a)
{
  int dc = dist_collision (*a);
  int dir = (a->dir == LEFT) ? -1 : +1;
  a->c.x += dir * ((abs (dc) < PLACE_WIDTH) ? dc - 1 : 0);
  printf ("dc = %i\n", dc);
}

void
to_back_collision_edge (struct anim *a)
{
  int dbc = dist_back_collision (*a);
  int dir = (a->dir == LEFT) ? +1 : -1;
  a->c.x += dir * ((abs (dbc) < PLACE_WIDTH) ? dbc - 1 : 0);
  printf ("dbc = %i\n", dbc);
}

void
to_fall_edge (struct anim *a)
{
  int df = dist_fall (*a);
  int dir = (a->dir == LEFT) ? -1 : +1;
  a->c.x += dir * ((abs (df) < PLACE_WIDTH) ? df - 1 : 0);
  printf ("df = %i\n", df);
}

bool
is_on_loose_floor (struct anim a)
{
  a.c.x += (a.dir == LEFT) ? 4 : -4;

  if (a.draw == draw_kid_misstep) return false;
  if (a.id == &kid && is_kid_stabilize ()) return false;
  if (a.id == &kid && is_kid_turn ()) return false;

  if (a.id == &kid && (is_kid_start_jump () ||
                       is_kid_jump ()))
    return false;

  struct pos p = pos (coord_tf (a));
  struct construct c = construct (p);

  if (c.fg == LOOSE_FLOOR) {
    loose_floor_pos = p;
    return true;
  }
  return false;
}

int
dist_loose_floor (struct anim a)
{
  int inc = (a.dir == LEFT) ? -1 : +1;
  int x = a.c.x;

  if (! is_on_loose_floor (a))
    while (! is_on_loose_floor (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x += inc;
  else
    while (is_on_loose_floor (a) && abs (x - a.c.x) != PLACE_WIDTH)
      a.c.x -= inc;

  return inc * (a.c.x - x);
}

void
to_loose_floor_edge (struct anim *a)
{
  int dl = dist_loose_floor (*a);
  int dir = (a->dir == LEFT) ? -1 : +1;
  a->c.x += dir * ((abs (dl) < PLACE_WIDTH) ? dl - 1 : 0);
  printf ("dl = %i\n", dl);
}

void
to_next_place_edge (struct anim *a)
{
  int dn = dist_next_place (*a);
  int dir = (a->dir == LEFT) ? -1 : +1;
  a->c.x += dir * ((abs (dn) < PLACE_WIDTH) ? dn - 1 : 0);
  printf ("dn = %i\n", dn);
}

void
to_prev_place_edge (struct anim *a)
{
  int dp = dist_prev_place (*a);
  int dir = (a->dir == LEFT) ? +1 : -1;
  a->c.x += dir * ((abs (dp) < PLACE_WIDTH) ? dp : 0);
  printf ("dp = %i\n", dp);
}

bool
is_hangable_pos (struct pos p, enum dir direction)
{
  int dir = (direction == LEFT) ? -1 : +1;
  enum construct_fg fg = construct_rel (p, -1, dir).fg;

  return (fg == FLOOR || fg == BROKEN_FLOOR
          || fg == LOOSE_FLOOR || fg == PILLAR)
    && construct_rel (p, -1, 0).fg == NO_FLOOR;
}

bool
is_hangable (struct anim a)
{
  if (a.id == &kid && is_kid_jump ()) return false;

  struct pos pmf = pos (coord_mf (a));
  struct pos pm = pos (coord_m (a));
  struct pos pmb = pos (coord_mba (a));
  bool hmf = is_hangable_pos (pmf, a.dir);
  bool hm = is_hangable_pos (pm, a.dir);
  bool hmb = is_hangable_pos (pmb, a.dir);

  if (! hmf && ! hm && ! hmb) return false;

  if (hmf) hang_pos = pmf;
  if (hm) hang_pos = pm;
  if (hmb) hang_pos = pmb;

  struct coord tf = coord_tf (a);
  struct coord ch;
  int dir = (a.dir == LEFT) ? 0 : 1;
  ch.x = PLACE_WIDTH * (hang_pos.place + dir) + 7 + 12 * dir;
  ch.y = PLACE_HEIGHT * hang_pos.floor - 6;

  /* printf ("dist_coord = %f\n", dist_coord (tf, ch)); */

  if (a.id == &kid && is_kid_fall ()
      && a.odraw != draw_kid_turn_run
      && dist_coord (tf, ch) > 16) return false;

  return true;
}

bool
is_visible (struct anim a)
{
  return nanim (a).room == room_view;
}

bool
is_pos_visible (struct pos p)
{
  p = npos (p);

  if (p.room == room_view) return true;
  if (p.room == roomd (room_view, LEFT) && p.place == PLACES - 1)
    return true;
  if (p.room == roomd (room_view, ABOVE) && p.floor == FLOORS - 1)
    return true;
  if (p.room == roomd (room_view, BELOW) && p.floor == 0)
    return true;

  return false;
}

void
apply_physics (struct anim *a, ALLEGRO_BITMAP *frame,
               int dx, int dy)
{
  struct anim na = next_anim (*a, frame, dx, dy);

  if (cutscene) {
    (*a) = na;
    return;
  }

  if (is_hitting_ceiling (na)) {
    na.odraw = na.draw;
    na.draw = na.ceiling;
  }

  if (is_colliding (na)) {
    na.odraw = na.draw;
    na.draw = na.collision;
    to_collision_edge (&na);
  }

  if (is_back_colliding (na)) {
    na.odraw = na.draw;
    na.draw = na.back_collision;
    to_back_collision_edge (&na);
  }

  if (is_on_loose_floor (na))
    release_loose_floor (loose_floor_pos);

  if (is_falling (na)) {
    na.odraw = na.draw;
    na.draw = na.fall;
  }

  na.c = nanim (na);

  (*a) = na;
}
