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
#include "loose-floor.h"
#include "opener-floor.h"
#include "pos.h"
#include "door.h"
#include "physics.h"

struct con
con (struct pos p)
{
  p = npos (p);
  return level->con[p.room][p.floor][p.place];
}

struct con
crel (struct pos p, int floor, int place)
{
  p.floor += floor;
  p.place += place;
  return con (p);
}

void
set_confg (struct pos p, enum confg fg)
{
  p = npos (p);
  level->con[p.room][p.floor][p.place].fg = fg;
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

struct anim
adjust_anim (struct anim a, int margin, bool reverse)
{
  if (reverse) a.dir = (a.dir == LEFT) ? RIGHT : LEFT;
  a.c.x += (a.dir == LEFT) ? +margin : -margin;
  return a;
}

int
dist_next_place (struct anim a,
                 struct coord (*coord_func) (struct anim a),
                 struct pos (*pos_func) (struct coord c),
                 int margin, bool reverse)
{
  a = adjust_anim (a, margin, reverse);
  struct pos p = pos_func (coord_func (a));
  int inc = (a.dir == LEFT) ? -1 : +1;
  int x = a.c.x;

  while (p.place == pos_func (coord_func (a)).place
         && abs (x - a.c.x) != PLACE_WIDTH)
    a.c.x += inc;

  return inc * (a.c.x - x);
}

bool
to_next_place_edge (struct anim *a,
                    struct coord (*coord_func) (struct anim a),
                    struct pos (*pos_func) (struct coord c),
                    int margin,
                    bool reverse)
{
  int dn = dist_next_place (*a, coord_func, pos_func, margin, reverse);
  struct anim na = adjust_anim (*a, margin, reverse);
  int dir = (na.dir == LEFT) ? -1 : +1;
  a->c.x += dir * ((abs (dn) < PLACE_WIDTH) ? dn - 1 : 0);
  return abs (dn) < PLACE_WIDTH;
}

bool
is_colliding (struct anim a,
              struct coord (*coord_func) (struct anim a),
              struct pos (*pos_func) (struct coord c),
              int margin, bool reverse, int min_dist)
{
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);
  a = adjust_anim (a, margin, reverse);
  int dir = (a.dir == LEFT) ? -1: +1;

  struct coord c = coord_func (a);
  struct pos p = pos_func (c);
  enum confg ct = con (p).fg;

  return dn <= min_dist
    && (/* wall */
        crel (p, 0, dir).fg == WALL

        /* door */
        || (a.dir == RIGHT
            && ct == DOOR
            && c.y <= door_grid_tip_y (p) - 10)
        || (a.dir == LEFT
            && crel (p, 0, -1).fg == DOOR
            && c.y <=
            door_grid_tip_y (prel (p, 0, -1)) - 10));
}

bool
is_on_con (struct anim a,
           struct coord (*coord_func) (struct anim a),
           struct pos (*pos_func) (struct coord c),
           int margin, bool reverse, int min_dist, enum confg ct)
{
  /* margin = 4 */
  /* coord_func = coord_mbo */
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);
  a = adjust_anim (a, margin, reverse);
  int dir = (a.dir == LEFT) ? -1: +1;

  return dn <= min_dist
    && crel (pos_func (coord_func (a)), 0, dir).fg == ct;
}
