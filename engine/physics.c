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
#include "physics.h"

struct pos loose_floor_pos;
struct pos hang_pos;

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

  /* fix bug when vertically jumping near a wall edge */
  if (a.id == &kid && is_kid_vjump ()) return false;
  /* fix bug when vertically jumping near a wall face */
  if (a.id == &kid && is_kid_start_vjump ()) return false;

  struct pos p = pos (coord_tf (a));
  struct construct c = construct (p);
  if (c.fg == WALL) {
    return true;
  } else return false;
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

bool
is_falling (struct anim a)
{
  if (! a.fall) return false;

  if (a.id == &kid && (is_kid_start_jump ())) return false;

  a.c.x += (a.dir == LEFT) ? +4 : -4;

  struct coord cbf = coord_bf (a);
  struct pos pbf = pos (cbf);
  struct construct ctbf = construct (pbf);

  if (ctbf.fg == NO_FLOOR
      || cbf.y + 33 < PLACE_HEIGHT * (pbf.floor + 1) + 3) {
    return true;
  }

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
  if (a.id == &kid)
    return ncoord (coord_m (a)).room == room_view;
  else {
    struct coord ntl = ncoord (coord_tl (a));
    struct coord ntr = ncoord (coord_tr (a));
    struct coord nbl = ncoord (coord_bl (a));
    struct coord nbr = ncoord (coord_br (a));

    if (ntl.room == room_view
        || ntr.room == room_view
        || nbl.room == room_view
        || nbr.room == room_view) return true;
    else return false;
  }
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
  } else if (is_colliding (na)
             && na.draw != na.collision) {
    na.odraw = na.draw;
    na.draw = na.collision;
    to_collision_edge (&na);
  } else if (is_falling (na)
             && na.draw != na.fall
             && na.draw != na.collision) {
    na.odraw = na.draw;
    na.draw = na.fall;
  } else if (is_on_loose_floor (na))
    release_loose_floor (loose_floor_pos);

  na.c = nanim (na);

  (*a) = na;
}
