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
#include "prince.h"
#include "kernel/random.h"
#include "level.h"
#include "kid/kid.h"
#include "anim.h"
#include "room.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"
#include "chopper.h"
#include "level-door.h"
#include "pos.h"
#include "door.h"
#include "physics.h"

struct con *
con (struct pos *p)
{
  struct pos np; npos (p, &np);
  return &level.con[np.room][np.floor][np.place];
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
  struct coord tl, tr, bl, br;

  struct frame nf;
  nf = *f;

  frame2room (&nf, room_view, &nf.c);

  _tl (&nf, &tl);
  _tr (&nf, &tr);
  _bl (&nf, &bl);
  _br (&nf, &br);

  if (tl.x >= ORIGINAL_WIDTH || tl.y >= ORIGINAL_HEIGHT
      || tr.x < 0 || tr.y >= ORIGINAL_HEIGHT
      || bl.x >= ORIGINAL_WIDTH || bl.y < 0
      || br.x < 0 || br.y < 0) return false;
  else return true;
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
strictly_traversable_confg_set (enum confg t)
{
  return t == NO_FLOOR
    || t == BIG_PILLAR_TOP
    || t == TCARPET;
}

bool
is_strictly_traversable (struct pos *p)
{
  enum confg t = con (p)->fg;
  return strictly_traversable_confg_set (t)
    || arch_top_confg_set (t);
}

bool
traversable_confg_set (enum confg t)
{
  return strictly_traversable_confg_set (t)
    || t == LOOSE_FLOOR;
}

bool
is_traversable (struct pos *p)
{
  return traversable_confg_set (con (p)->fg);
}

bool
is_rigid_con (struct pos *p)
{
  enum confg fg = con (p)->fg;
  return fg == PILLAR || fg == BIG_PILLAR_TOP
    || fg == WALL || fg == DOOR || fg == CHOPPER
    || is_arch_top (p) || is_carpet (p)
    || fg == MIRROR;
}

bool
is_carpet (struct pos *p)
{
  enum confg t = con (p)->fg;
  return t == CARPET
    || t == TCARPET;
}

bool
is_arch_top (struct pos *p)
{
  return arch_top_confg_set (con (p)->fg);
}

bool
arch_top_confg_set (enum confg t)
{
  return t == ARCH_TOP_MID
    || t == ARCH_TOP_SMALL
    || t == ARCH_TOP_LEFT
    || t == ARCH_TOP_RIGHT;
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
is_colliding (struct frame *f, struct frame_offset *fo, int dx,
              int reverse, struct collision_info *ci)
{
  return is_colliding_cf (f, fo, dx, reverse, ci, _bf)
    || is_colliding_cf (f, fo, dx, reverse, ci, _tf);
}

bool
is_colliding_cf (struct frame *f, struct frame_offset *fo, int dx,
                 int reverse, struct collision_info *ci,
                 coord_f cf)
{
  struct coord nc, tf; struct pos np, pcf, _pcf, pocf, p, pl, pr;

  struct frame _f = *f, nf;

  nframe (&_f, &_f.c);

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  next_frame_inv = (reverse == true);
  next_frame (&_f, &nf, fo);
  next_frame_inv = false;

  int dir = (nf.dir == LEFT) ? -1 : +1;
  nf.c.x += dir * dx;

  coord_f ocf = opposite_cf (cf);

  survey (cf, pos, &_f, &nc, &_pcf, &np);
  survey (cf, pos, &nf, &nc, &pcf, &np);
  survey (ocf, pos, &nf, &nc, &pocf, &np);
  survey (_tf, pos, &nf, &tf, &np, &np);

  if (pcf.room != _pcf.room) pos2room (&pcf, _f.c.room, &pcf);

  bool wall_collision = false;
  bool door_collision = false;
  bool carpet_collision = false;
  bool mirror_collision = false;

  /* printf ("_pcf: (%i,%i,%i); pcf: (%i,%i,%i)\n", */
  /*         _pcf.room, _pcf.floor, _pcf.place, */
  /*         pcf.room, pcf.floor, pcf.place); */

  /* wall */

  if (_f.dir == LEFT && pcf.place <= _pcf.place)
    for (p = _pcf; p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place >= _pcf.place)
    for (p = _pcf; p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      if (con (&p)->fg == WALL) {
        wall_collision = true;
        ci->p = p;
        break;
      }
    }

  if (con (&pcf)->fg == WALL) {
    wall_collision = true;
    ci->p = pcf;
  }


  /* door */

  if (_f.dir == LEFT && pcf.place < _pcf.place)
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (con (&p)->fg == DOOR
          && tf.y <= door_grid_tip_y (&p) - 10) {
        door_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place > _pcf.place)
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      prel (&p, &pl, +0, -1);
      if (con (&pl)->fg == DOOR
          && tf.y <= door_grid_tip_y (&pl) - 10) {
        door_collision = true;
        ci->p = p;
        break;
      }
    }

  if (con (&pcf)->fg == DOOR
      && pcf.place < pocf.place
      && tf.y <= door_grid_tip_y (&pcf) - 10) {
    door_collision = true;
    ci->p = pcf;
  }

  if (con (&pocf)->fg == DOOR
      && pocf.place < pcf.place
      && tf.y <= door_grid_tip_y (&pocf) - 10) {
    door_collision = true;
    ci->p = pcf;
  }


  /* carpet */

  if (_f.dir == LEFT && pcf.place < _pcf.place)
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1))
      if (is_carpet (&p)) {
        carpet_collision = true;
        ci->p = p;
        break;
      }

  if (_f.dir == RIGHT && pcf.place > _pcf.place)
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      prel (&p, &pl, +0, -1);
      if (is_carpet (&pl)) {
        carpet_collision = true;
        ci->p = p;
        break;
      }
    }

  if (is_carpet (&pcf)
      && pcf.place < pocf.place) {
    carpet_collision = true;
    ci->p = pcf;
  }

  if (is_carpet (&pocf)
      && pocf.place < pcf.place) {
    carpet_collision = true;
    ci->p = pcf;
  }


  /* mirror */

  if (_f.dir == LEFT && pcf.place < _pcf.place)
    for (prel (&_pcf, &p, +0, -1); p.place >= pcf.place; prel (&p, &p, +0, -1)) {
      prel (&p, &pr, +0, +1);
      if (con (&pr)->fg == MIRROR) {
        mirror_collision = true;
        ci->p = p;
        break;
      }
    }

  if (_f.dir == RIGHT && pcf.place > _pcf.place)
    for (prel (&_pcf, &p, +0, +1); p.place <= pcf.place; prel (&p, &p, +0, +1)) {
      if (con (&p)->fg == MIRROR) {
        mirror_collision = true;
        ci->p = p;
        break;
      }
    }

  if (con (&pocf)->fg == MIRROR
      && pcf.place < pocf.place) {
    mirror_collision = true;
    ci->p = pcf;
  }

  if (con (&pcf)->fg == MIRROR
      && pocf.place < pcf.place) {
    mirror_collision = true;
    ci->p = pcf;
  }

  ci->t = NO_FLOOR;
  if (wall_collision) ci->t = WALL;
  if (door_collision) ci->t = DOOR;
  if (carpet_collision) ci->t = CARPET;
  if (mirror_collision) ci->t = MIRROR;

  /* if (wall_collision || door_collision || carpet_collision || mirror_collision) */
  /*   pos2room (&ci->p, _f.c.room, &ci->p); */

  /* if (door_collision) printf ("DOOR COLLISION!\n"); */
  /* if (wall_collision) printf ("WALL COLLISION!\n"); */
  /* if (carpet_collision) printf ("CARPET COLLISION!\n"); */
  /* if (mirror_collision) printf ("MIRROR COLLISION!\n"); */

  return wall_collision || door_collision || carpet_collision || mirror_collision;
}

struct frame_offset *
uncollide (struct frame *f, struct frame_offset *fo,
           struct frame_offset *_fo, int dx,
           int reverse, struct collision_info *ci)
{
  int ow = al_get_bitmap_width (f->b);
  int w = al_get_bitmap_width (fo->b);

  *_fo = *fo;

  int inc = reverse ? -1 : +1;

  while (is_colliding (f, _fo, dx, reverse, ci)
         && abs (_fo->dx) <= abs (ow - w + 1))
    _fo->dx += inc;

  return _fo;
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
dist_collision (struct frame *f, int reverse,
                struct collision_info *ci)
{
  int i = 0, dx = +1;
  struct frame _f = *f;
  struct frame_offset _fo;

  _fo.b = _f.b;
  _fo.dx = _fo.dy = 0;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  int dir = (_f.dir == LEFT) ? -1 : +1;

  if (! is_colliding (&_f, &_fo, dx, reverse, ci))
    while (! is_colliding (&_f, &_fo, dx, reverse, ci)
           && i < PLACE_WIDTH + 1) {
      _f.c.x += dir;
      i++;
    }
  else
    while (is_colliding (&_f, &_fo, dx, reverse, ci)
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

int
dist_chopper (struct frame *f, bool reverse)
{
  struct coord nc; struct pos np, ptf, ptfr;

  struct frame _f = *f;
  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  enum confg ctf = survey (_tf, pos, &_f, &nc, &ptf, &np)->fg;
  prel (&ptf, &ptfr, +0, +1);
  enum confg ctfr = con (&ptfr)->fg;

  if (_f.dir == LEFT && ctf == CHOPPER)
    return dist_next_place (&_f, _tf, pos, +0, false);
  else if (_f.dir == RIGHT && ctfr == CHOPPER)
    return dist_next_place (&_f, _tf, pos, +0, false);

  return PLACE_WIDTH + 1;
}

int
dist_fall (struct frame *f, bool reverse)
{
  int dnf, dbpt, datm, dats, datl, datr, dtc;

  dnf = dist_con (f, _bf, pos, -4, reverse, NO_FLOOR);
  dbpt = dist_con (f, _bf, pos, -4, reverse, BIG_PILLAR_TOP);
  datm = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_MID);
  dats = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_SMALL);
  datl = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_LEFT);
  datr = dist_con (f, _bf, pos, -4, reverse, ARCH_TOP_RIGHT);
  dtc = dist_con (f, _bf, pos, -4, reverse, TCARPET);

  if (dnf <= PLACE_WIDTH) return dnf;
  if (dbpt <= PLACE_WIDTH) return dbpt;
  if (datm <= PLACE_WIDTH) return datm;
  if (dats <= PLACE_WIDTH) return dats;
  if (datl <= PLACE_WIDTH) return datl;
  if (datr <= PLACE_WIDTH) return datr;
  if (dtc <= PLACE_WIDTH) return dtc;

  return PLACE_WIDTH + 1;
}



bool
is_hangable_con (struct pos *p, enum dir d)
{
  enum confg t = con (p)->fg;
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == LOOSE_FLOOR
    /* || (t == LOOSE_FLOOR */
    /*     && loose_floor_at_pos (p)->action */
    /*     != RELEASE_LOOSE_FLOOR) */
    || t == SKELETON_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == DOOR
    || t == LEVEL_DOOR
    || (t == CHOPPER && d == LEFT)
    || t == ARCH_BOTTOM
    || (t == CARPET && d == RIGHT)
    || (t == MIRROR && d == LEFT);
}

bool
is_hangable_pos (struct pos *p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ph; prel (p, &ph, -1, dir);
  struct pos pa; prel (p, &pa, -1, 0);
  struct pos pr; prel (p, &pr, +0, +1);
  /* struct con *ca = con (&pa); */
  struct con *cr = con (&pr);

  return is_hangable_con (&ph, d)
    && is_strictly_traversable (&pa)
    && ! (d == RIGHT && cr->fg == CHOPPER)
    && ! (d == RIGHT && con (&pr)->fg == MIRROR)
    && ! (d == RIGHT && is_carpet (p))
    && ! (d == RIGHT && is_carpet (&pa));
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

  pos2room (hang_pos, _f.c.room, hang_pos);

  /* for fall */
  struct coord ch;
  int dir = (_f.dir == LEFT) ? 0 : 1;
  ch.x = PLACE_WIDTH * (hang_pos->place + dir) + 7 + 8 * dir;
  ch.y = PLACE_HEIGHT * hang_pos->floor - 6;

  double d = dist_coord (&tf, &ch);

  /* printf ("dist_coord = %f\n", d); */

  if (is_kid_fall (&_f) && d > 20) return false;

  return true;
}

struct pos *
get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p)
{
  int dir = (d == LEFT) ? -1 : +1;
  prel (hang_pos, p, -1, dir);
  pos2room (p, hang_pos->room, p);
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
  return (is_traversable (hang_pos));
}

bool
is_hang_pos_free (struct pos *hang_pos, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ptf; prel (hang_pos, &ptf, 0, dir);
  enum confg ctf = con (&ptf)->fg;
  return ! (ctf == WALL || (ctf == DOOR && d == LEFT)
            || (is_carpet (&ptf) && d == LEFT));
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
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  default: break;
  }
}

void
unhide_hidden_floor (struct pos *p)
{
  if (con (p)->fg != HIDDEN_FLOOR) return;
  else con (p)->fg = FLOOR;
}




void
activate_con (struct pos *p)
{
  struct door *d;
  struct closer_floor *c;
  struct opener_floor *o;

  switch (con (p)->fg) {
  case SPIKES_FLOOR:
    spikes_floor_at_pos (p)->activate = true; break;
  case DOOR:
    d = door_at_pos (p);
    d->action = (d->i) ? OPEN_DOOR : CLOSE_DOOR;
    break;
  case OPENER_FLOOR:
    o = opener_floor_at_pos (p);
    o->noise = true;
    press_opener_floor (p); break;
  case CLOSER_FLOOR:
    c = closer_floor_at_pos (p);
    c->noise = true;
    press_closer_floor (p); break;
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p); break;
  case CHOPPER: chopper_at_pos (p)->activate = true; break;
  case LEVEL_DOOR: level_door_at_pos (p)->action = OPEN_LEVEL_DOOR;
    break;
  default: break;
  }
}
