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

struct frame *
to_next_place_edge (struct frame *f, struct frame *nf,
                    ALLEGRO_BITMAP *b, coord_f cf, pos_f pf,
                    int margin, bool reverse, int min_dist)
{
  next_frame (f, nf, b, +0, +0);
  int dn = dist_next_place (nf, cf, pf, margin, reverse);

  int r = reverse ? -1 : 1;
  int dir = (nf->dir == LEFT) ? r * -1 : r * +1;

  nf->c.x += dir * (dn - 1);
  nf->c.x += -dir * min_dist;

  return nf;
}

bool
is_colliding (struct frame *f, coord_f cf, pos_f pf,
              int margin, bool reverse, int min_dist)
{
  struct frame _f = *f;

  int dn = dist_next_place (&_f, cf, pf, margin, reverse);
  min_dist += (reverse) ? +1 : 0;

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (_f.dir == LEFT) ? r * -1: r * +1;

  struct coord c; struct pos p, np;
  struct con *t = survey (cf, pf, &_f, &c, &p, &np);
  struct pos pr; prel (&p, &pr, 0, -1);

  bool wall_collision = (crel (&p, 0, dir)->fg == WALL);
  bool door_collision =
    (_f.dir == RIGHT
     && t->fg == DOOR
     && c.y <= door_grid_tip_y (&p) - 10)
    || (_f.dir == LEFT
        && crel (&p, 0, -1)->fg == DOOR
        && c.y <=
        door_grid_tip_y (&pr) - 10);

  if (wall_collision) confg_collision = WALL;
  if (door_collision) confg_collision = DOOR;

  return dn <= min_dist && (door_collision || wall_collision);
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
dist_collision (struct frame *f, coord_f cf, pos_f pf,
                int margin, bool reverse)
{
  int dn = dist_next_place (f, cf, pf, margin, reverse);
  if (is_colliding (f, cf, pf, margin, reverse, dn)) return dn;
  else return PLACE_WIDTH + 1;
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
to_collision_edge (struct frame *f, ALLEGRO_BITMAP *b,
                   coord_f cf, pos_f pf,
                   int margin, bool reverse, int min_dist)
{
  int dc = dist_collision (f, cf, pf, margin, reverse);
  if (dc > PLACE_WIDTH) return false;
  else to_next_place_edge (f, f, b, cf, pf, margin, reverse, min_dist);
  return true;
}

bool
to_con_edge (struct frame *f, ALLEGRO_BITMAP *b,
             coord_f cf, pos_f pf,
             int margin, bool reverse, int min_dist, enum confg t)
{
  int dc = dist_con (f, cf, pf, margin, reverse, t);
  if (dc > PLACE_WIDTH) return false;
  else to_next_place_edge (f, f, b, cf, pf, margin, reverse, min_dist);
  return true;
}





bool
is_hangable_pos (struct pos *p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ph; prel (p, &ph, -1, dir);
  struct con *ch = con (&ph);
  struct pos pa; prel (p, &pa, -1, 0);
  struct con *ca = con (&pa);

  return (ch->fg == FLOOR
          || ch->fg == BROKEN_FLOOR
          || (ch->fg == LOOSE_FLOOR
              && loose_floor_at_pos (&ph)->action
              != RELEASE_LOOSE_FLOOR)
          || ch->fg == SKELETON_FLOOR
          || ch->fg == SPIKES_FLOOR
          || ch->fg == OPENER_FLOOR
          || ch->fg == CLOSER_FLOOR
          || ch->fg == PILLAR || ch->fg == DOOR)
    && (ca->fg == NO_FLOOR
        || (ca->fg == LOOSE_FLOOR
            && loose_floor_at_pos (&pa)->action
            == RELEASE_LOOSE_FLOOR));
}

bool
can_hang (struct frame *f)
{
  struct frame _f = *f;
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

  if (hmf) hang_pos = pmf;
  if (hm) hang_pos = pm;
  if (hmba) hang_pos = pmba;

  pos2view (&hang_pos, &hang_pos);

  /* for fall */
  struct coord ch;
  int dir = (_f.dir == LEFT) ? 0 : 1;
  ch.x = PLACE_WIDTH * (hang_pos.place + dir) + 7 + 8 * dir;
  ch.y = PLACE_HEIGHT * hang_pos.floor - 6;

  double d = dist_coord (&tf, &ch);

  printf ("dist_coord = %f\n", d);

  if (is_kid_fall (&_f) && d > 19) return false;

  return true;
}




void
update_depressible_floor (struct anim *a, int dx0, int dx1)
{
  struct coord c0, c1;
  struct pos p0, p1;
  struct con *t0, *t1;

  int dir = (a->f.dir == LEFT) ? -1 : 1;

  _bf (&a->f, &c0);
  c0.x += dir * dx0;
  pos (&c0, &p0);
  t0 = con (&p0);

  _bf (&a->f, &c1);
  c1.x += dir * dx1;
  pos (&c1, &p1);
  t1 = con (&p1);

  press_depressible_floor (&p0);
  a->df_pos[0] = p0;

  press_depressible_floor (&p1);
  a->df_pos[1] = p1;

  if (t0->fg != OPENER_FLOOR
      && t0->fg != CLOSER_FLOOR
      && t0->fg != LOOSE_FLOOR)
    a->df_pos[0].room = -1;

  if (t1->fg != OPENER_FLOOR
      && t1->fg != CLOSER_FLOOR
      && t1->fg != LOOSE_FLOOR)
    a->df_pos[1].room = -1;
}

void
keep_depressible_floor (struct anim *a)
{
  struct pos *p0 = &a->df_pos[0];
  struct pos *p1 = &a->df_pos[1];

  if (p0->room != -1) press_depressible_floor (p0);
  if (p1->room != -1) press_depressible_floor (p1);
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
  switch (con (p)->fg) {
  case OPENER_FLOOR: press_opener_floor (p); break;
  case CLOSER_FLOOR: press_closer_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  default: break;
  }
}
