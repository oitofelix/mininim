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
#include "closer-floor.h"
#include "pos.h"
#include "door.h"
#include "physics.h"

struct pos hang_pos;
enum confg confg_collision;

struct con *
con (struct pos p)
{
  p = npos (p);
  return &level->con[p.room][p.floor][p.place];
}

struct con *
crel (struct pos p, int floor, int place)
{
  p.floor += floor;
  p.place += place;
  return con (p);
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




int
dist_next_place (struct anim a,
                 struct coord (*coord_func) (struct anim a),
                 struct pos (*pos_func) (struct coord c),
                 int margin, bool reverse)
{
  a.c.x += (a.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int inc = (a.dir == LEFT) ? r * -1 : r * +1;

  struct pos p = pos_func (coord_func (a));
  int x = a.c.x;
  while (p.place == pos_func (coord_func (a)).place
         && abs (x - a.c.x) < PLACE_WIDTH) a.c.x += inc;

  return abs (a.c.x - x);
}

void
to_next_place_edge (struct anim *a, ALLEGRO_BITMAP *frame,
                    struct coord (*coord_func) (struct anim a),
                    struct pos (*pos_func) (struct coord c),
                    int margin, bool reverse, int min_dist)
{
  *a = next_anim (*a, frame, +0, 0);
  int dn = dist_next_place (*a, coord_func, pos_func, margin, reverse);

  int r = reverse ? -1 : 1;
  int dir = (a->dir == LEFT) ? r * -1 : r * +1;

  a->c.x += dir * (dn - 1);
  a->c.x += -dir * min_dist;
}

bool
is_colliding (struct anim a,
              struct coord (*coord_func) (struct anim a),
              struct pos (*pos_func) (struct coord c),
              int margin, bool reverse, int min_dist)
{
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);

  min_dist += (reverse) ? +1 : 0;

  a.c.x += (a.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (a.dir == LEFT) ? r * -1: r * +1;

  struct coord c = coord_func (a);
  struct pos p = pos_func (c);
  enum confg ct = con (p)->fg;

  bool wall_collision = (crel (p, 0, dir)->fg == WALL);
  bool door_collision =
    (a.dir == RIGHT
     && ct == DOOR
     && c.y <= door_grid_tip_y (p) - 10)
    || (a.dir == LEFT
        && crel (p, 0, -1)->fg == DOOR
        && c.y <=
        door_grid_tip_y (prel (p, 0, -1)) - 10);

  if (wall_collision) confg_collision = WALL;
  if (door_collision) confg_collision = DOOR;

  return dn <= min_dist && (door_collision || wall_collision);
}

bool
is_on_con (struct anim a,
           struct coord (*coord_func) (struct anim a),
           struct pos (*pos_func) (struct coord c),
           int margin, bool reverse, int min_dist, enum confg ct)
{
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);

  a.c.x += (a.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (a.dir == LEFT) ? r * -1: r * +1;

  return dn <= min_dist
    && crel (pos_func (coord_func (a)), 0, dir)->fg == ct;
}

int
dist_collision (struct anim a,
                struct coord (*coord_func) (struct anim a),
                struct pos (*pos_func) (struct coord c),
                int margin, bool reverse)
{
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);
  if (is_colliding (a, coord_func, pos_func, margin, reverse, dn)) return dn;
  else return PLACE_WIDTH + 1;
}

int
dist_con (struct anim a,
          struct coord (*coord_func) (struct anim a),
          struct pos (*pos_func) (struct coord c),
          int margin, bool reverse, enum confg ct)
{
  int dn = dist_next_place (a, coord_func, pos_func, margin, reverse);
  if (is_on_con (a, coord_func, pos_func, margin, reverse, dn, ct)) return dn;
  else return PLACE_WIDTH + 1;
}

bool
to_collision_edge (struct anim *a, ALLEGRO_BITMAP *frame,
                   struct coord (*coord_func) (struct anim a),
                   struct pos (*pos_func) (struct coord c),
                   int margin, bool reverse, int min_dist)
{
  int dc = dist_collision (*a, coord_func, pos_func, margin, reverse);
  if (dc > PLACE_WIDTH) return false;
  else to_next_place_edge (a, frame, coord_func, pos_func, margin, reverse, min_dist);
  return true;
}

bool
to_con_edge (struct anim *a, ALLEGRO_BITMAP *frame,
             struct coord (*coord_func) (struct anim a),
             struct pos (*pos_func) (struct coord c),
             int margin, bool reverse, int min_dist, enum confg ct)
{
  int dc = dist_con (*a, coord_func, pos_func, margin, reverse, ct);
  if (dc > PLACE_WIDTH) return false;
  else to_next_place_edge (a, frame, coord_func, pos_func, margin, reverse, min_dist);
  return true;
}





bool
is_hangable_pos (struct pos p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  enum confg fg = crel (p, -1, dir)->fg;

  return (fg == FLOOR || fg == BROKEN_FLOOR || fg == LOOSE_FLOOR
          || fg == SKELETON_FLOOR || fg == SPIKES_FLOOR
          || fg == OPENER_FLOOR || fg == CLOSER_FLOOR
          || fg == PILLAR || fg == DOOR)
    && crel (p, -1, 0)->fg == NO_FLOOR;
}

bool
can_hang (struct anim a)
{
  struct coord tf = coord_tf (a);

  struct pos pmf = pos (coord_mf (a));
  struct pos pm = pos (coord_m (a));
  struct pos pmba = pos (coord_mba (a));

  bool hmf = is_hangable_pos (pmf, a.dir);
  bool hm = is_hangable_pos (pm, a.dir);
  bool hmba = is_hangable_pos (pmba, a.dir);

  if (! hmf && ! hm && ! hmba)
    return false;

  if (hmf) hang_pos = pmf;
  if (hm) hang_pos = pm;
  if (hmba) hang_pos = pmba;

  hang_pos = npos (hang_pos);

  /* for fall */
  struct coord ch;
  int dir = (a.dir == LEFT) ? 0 : 1;
  ch.x = PLACE_WIDTH * (hang_pos.place + dir) + 7 + 8 * dir;
  ch.y = PLACE_HEIGHT * hang_pos.floor - 6;

  printf ("dist_coord = %f\n", dist_coord (kids.tf, ch));

  if (is_kid_fall (a) &&
      dist_coord (tf, ch) > 19) return false;

  return true;
}




void
update_depressible_floor (struct anim *a, int dx0, int dx1)
{
  struct coord c0, c1;
  struct pos p0, p1;

  int dir = (a->dir == LEFT) ? -1 : 1;

  c0 = coord_bf (*a);
  c0.x += dir * dx0;
  p0 = pos (c0);

  c1 = coord_bf (*a);
  c1.x += dir * dx1;
  p1 = pos (c1);

  press_depressible_floor (p0);
  a->df_pos[0] = p0;

  press_depressible_floor (p1);
  a->df_pos[1] = p1;

  if (con (p0)->fg != OPENER_FLOOR
      && con (p0)->fg != CLOSER_FLOOR
      && con (p0)->fg != LOOSE_FLOOR)
    a->df_pos[0].room = -1;

  if (con (p1)->fg != OPENER_FLOOR
      && con (p1)->fg != CLOSER_FLOOR
      && con (p1)->fg != LOOSE_FLOOR)
    a->df_pos[1].room = -1;
}

void
keep_depressible_floor (struct anim *a)
{
  struct pos p0 = a->df_pos[0];
  struct pos p1 = a->df_pos[1];

  if (p0.room != -1) press_depressible_floor (p0);
  if (p1.room != -1) press_depressible_floor (p1);
}

void
clear_depressible_floor (struct anim *a)
{
  a->df_pos[0].room = -1;
  a->df_pos[1].room = -1;
}

void
save_depressible_floor (struct anim *a)
{
  a->df_posb[0] = a->df_pos[0];
  a->df_posb[1] = a->df_pos[1];
}

void
restore_depressible_floor (struct anim *a)
{
  a->df_pos[0] = a->df_posb[0];
  a->df_pos[1] = a->df_posb[1];
}

void
press_depressible_floor (struct pos p)
{
  switch (con (p)->fg) {
  case OPENER_FLOOR: press_opener_floor (p); break;
  case CLOSER_FLOOR: press_closer_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  default: break;
  }
}
