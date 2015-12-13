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
#include "kid/kid.h"
#include "anim.h"
#include "room.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "pos.h"
#include "door.h"
#include "physics.h"

enum confg confg_collision;
struct pos collision_pos;

struct con *
con (struct pos *p)
{
  struct pos np; npos (p, &np);
  return &level->con[np.room][np.floor][np.place];
}

struct con *
crel (struct pos *p, int floor, int place)
{
  struct pos pr;
  return con (prel (p, &pr, floor, place));
}

bool
is_frame_visible (struct frame *f)
{
  struct coord c; nframe (f, &c);
  return c.room == room_view;
}

bool
is_pos_visible (struct pos *p)
{
  struct pos np; npos (p, &np);

  if (np.room == room_view) return true;
  if (np.room == roomd (room_view, LEFT) && np.place == PLACES - 1)
    return true;
  if (np.room == roomd (room_view, ABOVE) && np.floor == FLOORS - 1)
    return true;
  if (np.room == roomd (room_view, BELOW) && np.floor == 0)
    return true;

  return false;
}

bool
is_strictly_traversable (struct pos *p)
{
  if (con (p)->fg == NO_FLOOR) return true;
  if (con (p)->fg == LOOSE_FLOOR) {
    struct loose_floor *l = loose_floor_at_pos (p);
    if (l->action == FALL_LOOSE_FLOOR) return true;
  }
  return false;
}



int
dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                 int margin, bool reverse)
{
  struct frame _f = *f;

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int inc = (_f.dir == LEFT) ? r * -1 : r * +1;

  int i = 0;

  struct coord nc;
  struct pos np, _np;

  survey (cf, pf, &_f, &nc, &np, &np);

  do {
    i += inc;
    _f.c.x += inc;
    nframe (&_f, &_f.c);
    survey (cf, pf, &_f, &nc, &_np, &_np);
  } while (np.place == _np.place
           && abs (i) < PLACE_WIDTH);

  return abs (i);
}

bool
is_colliding (struct frame *f, struct frame_offset *fo, int dx, bool reverse)
{
  struct coord nc, tf; struct pos np, ptf, _ptf, p, pl;

  struct frame _f = *f, nf;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  next_frame (&_f, &nf, fo);

  int dir = (nf.dir == LEFT) ? -1 : +1;
  nf.c.x += dir * dx;

  survey (_tf, pos, &_f, &nc, &_ptf, &np);
  survey (_tf, pos, &nf, &tf, &ptf, &np);

  pos2view (&_ptf, &_ptf);
  pos2view (&ptf, &ptf);

  bool wall_collision = false;
  bool door_collision = false;

  /* wall */

  if (_f.dir == LEFT && ptf.place <= _ptf.place)
    for (p = _ptf; p.place >= ptf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        collision_pos = p;
        break;
      }

  if (_f.dir == RIGHT && ptf.place >= _ptf.place)
    for (p = _ptf; p.place <= ptf.place; prel (&p, &p, +0, +1)) {
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        collision_pos = p;
        break;
      }
    }

  /* door */

  if (_f.dir == LEFT && ptf.place < _ptf.place)
    for (prel (&_ptf, &p, +0, -1); p.place >= ptf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == DOOR
          && tf.y <= door_grid_tip_y (&p) - 10) {
        door_collision = true;
        collision_pos = p;
        break;
      }

  if (_f.dir == RIGHT && ptf.place > _ptf.place)
    for (prel (&_ptf, &p, +0, +1); p.place <= ptf.place; prel (&p, &p, +0, +1)) {
      prel (&p, &pl, +0, -1);
      if (con (&pl)->fg == DOOR
          && tf.y <= door_grid_tip_y (&pl) - 10) {
        door_collision = true;
        collision_pos = p;
        break;
      }
    }

  confg_collision = NO_FLOOR;
  if (wall_collision) confg_collision = WALL;
  if (door_collision) confg_collision = DOOR;

  pos2view (&collision_pos, &collision_pos);

  /* if (door_collision) printf ("DOOR COLLISION!\n"); */
  /* if (wall_collision) printf ("WALL COLLISION!\n"); */

  return wall_collision || door_collision;
}

bool
is_on_con (struct frame *f, coord_f cf, pos_f pf,
           int margin, bool reverse, int min_dist, enum confg t)
{
  struct frame _f = *f;
  int dn = dist_next_place (&_f, cf, pf, margin, reverse);

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (_f.dir == LEFT) ? r * -1: r * +1;

  struct coord c; struct pos p, np;
  survey (cf, pf, &_f, &c, &p, &np);

  return dn <= min_dist && crel (&p, 0, dir)->fg == t;
}

int
dist_collision (struct frame *f, bool reverse)
{
  int i = 0, dx = +1;
  struct frame _f = *f;
  struct frame_offset _fo;

  _fo.b = _f.b;
  _fo.dx = _fo.dy = 0;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  int dir = (_f.dir == LEFT) ? -1 : +1;

  if (! is_colliding (&_f, &_fo, dx, reverse))
    while (! is_colliding (&_f, &_fo, dx, reverse)
           && i < PLACE_WIDTH + 1) {
      _f.c.x += dir;
      i++;
    }
  else
    while (is_colliding (&_f, &_fo, dx, reverse)
           && -i < PLACE_WIDTH + 1) {
      _f.c.x -= dir;
      i--;
    }

  return i;
}

int
dist_con (struct frame *f, coord_f cf, pos_f pf,
          int margin, bool reverse, enum confg t)
{
  int dn = dist_next_place (f, cf, pf, margin, reverse);
  if (is_on_con (f, cf, pf, margin, reverse, dn, t)) return dn;
  else return PLACE_WIDTH + 1;
}




bool
is_hangable_con (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == FLOOR
    || t == BROKEN_FLOOR
    || (t == LOOSE_FLOOR
        && loose_floor_at_pos (p)->action
        != RELEASE_LOOSE_FLOOR)
    || t == SKELETON_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == PILLAR || t == DOOR;
}

bool
is_hangable_pos (struct pos *p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ph; prel (p, &ph, -1, dir);
  struct pos pa; prel (p, &pa, -1, 0);
  struct con *ca = con (&pa);

  return is_hangable_con (&ph)
    && (ca->fg == NO_FLOOR
        || (ca->fg == LOOSE_FLOOR
            && loose_floor_at_pos (&pa)->action
            == RELEASE_LOOSE_FLOOR));
}

bool
can_hang (struct frame *f, bool reverse, struct pos *hang_pos)
{
  struct frame _f = *f;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  nframe (&_f, &_f.c);

  struct coord tf; _tf (&_f, &tf);

  struct coord mf, m, mba;
  struct pos pmf, npmf, pm, npm, pmba, npmba;
  survey (_mf, pos, &_f, &mf, &pmf, &npmf);
  survey (_m, pos, &_f, &m, &pm, &npm);
  survey (_mba, pos, &_f, &mba, &pmba, &npmba);

  bool hmf = is_hangable_pos (&pmf, _f.dir);
  bool hm = is_hangable_pos (&pm, _f.dir);
  bool hmba = is_hangable_pos (&pmba, _f.dir);

  if (! hmf && ! hm && ! hmba)
    return false;

  if (hmf) *hang_pos = pmf;
  if (hm) *hang_pos = pm;
  if (hmba) *hang_pos = pmba;

  pos2view (hang_pos, hang_pos);

  /* for fall */
  struct coord ch;
  int dir = (_f.dir == LEFT) ? 0 : 1;
  ch.x = PLACE_WIDTH * (hang_pos->place + dir) + 7 + 8 * dir;
  ch.y = PLACE_HEIGHT * hang_pos->floor - 6;

  double d = dist_coord (&tf, &ch);

  printf ("dist_coord = %f\n", d);

  if (is_kid_fall (&_f) && d > 20) return false;

  return true;
}

struct pos *
get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p)
{
  int dir = (d == LEFT) ? -1 : +1;
  prel (hang_pos, p, -1, dir);
  pos2view (p, p);
  return p;
}

enum confg
get_hanged_con (struct pos *hang_pos, enum dir d)
{
  struct pos p;
  return con (get_hanged_pos (hang_pos, d, &p))->fg;
}

bool
is_hang_pos_critical (struct pos *hang_pos)
{
  return (con (hang_pos)->fg == NO_FLOOR);
}

bool
is_hang_pos_free (struct pos *hang_pos, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  enum confg t = crel (hang_pos, 0, dir)->fg;
  return ! (t == WALL || (t == DOOR && d == LEFT));
}



void
update_depressible_floor (struct anim *a, int dx0, int dx1)
{
  struct coord c0, c1;
  struct pos p0, p1;

  int dir = (a->f.dir == LEFT) ? -1 : 1;

  _bf (&a->f, &c0);
  c0.x += dir * dx0;
  pos (&c0, &p0);

  _bf (&a->f, &c1);
  c1.x += dir * dx1;
  pos (&c1, &p1);

  press_depressible_floor (&p0);
  a->df_pos[0] = p0;

  press_depressible_floor (&p1);
  a->df_pos[1] = p1;
}

void
keep_depressible_floor (struct anim *a)
{
  struct pos *p0 = &a->df_pos[0];
  struct pos *p1 = &a->df_pos[1];

  press_depressible_floor (p0);
  press_depressible_floor (p1);
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
press_depressible_floor (struct pos *p)
{
  if (p->room == -1) return;

  switch (con (p)->fg) {
  case OPENER_FLOOR: press_opener_floor (p); break;
  case CLOSER_FLOOR: press_closer_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  default: break;
  }
}
