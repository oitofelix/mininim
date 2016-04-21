/*
  position.c -- position module;

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

#include "mininim.h"

bool coord_wa;

int *
roomd_ptr (int room, enum dir dir)
{
  switch (dir) {
  case LEFT: return &level.link[room].l;
  case RIGHT: return &level.link[room].r;
  case ABOVE: return &level.link[room].a;
  case BELOW: return &level.link[room].b;
  default:
    error (-1, 0, "%s: unknown direction (%i)", __func__, dir);
    return NULL;
  }
}

int
roomd (int room, enum dir dir)
{
  return *roomd_ptr (room, dir);
}

void
link_room (int room0, int room1, enum dir dir)
{
  if (room0) *roomd_ptr (room0, dir) = room1;
}

int
roomd_n0 (int room, enum dir dir)
{
  int r = roomd (room, dir);
  return r ? r : room;
}

bool
is_room_adjacent (int room0, int room1)
{
  return room0 == room1
    || roomd (room0, LEFT) == room1
    || roomd (room0, RIGHT) == room1
    || roomd (room0, ABOVE) == room1
    || roomd (room0, BELOW) == room1;
}

int
room_dist (int r0, int r1, int max)
{
  struct room_dist room[ROOMS];

  /* begin optimization block */
  if (r0 == r1) return 0;

  if (roomd (r0, LEFT) == r1
      || roomd (r0, RIGHT) == r1
      || roomd (r0, BELOW) == r1
      || roomd (r0, ABOVE) == r1)
    return 1;
  /* end optimization block */

  int i;
  for (i = 0; i < ROOMS; i++) {
    room[i].dist = INT_MAX;
    room[i].visited = false;
  }

  room[r0].dist = 0;
  int dmax = 0;

  int u;
  while ((u = min_room_dist (room, &dmax)) != -1
         && dmax <= max) {
    if (u == r1) break;
    room[u].visited = true;

    int l = roomd (u, LEFT);
    int r = roomd (u, RIGHT);
    int b = roomd (u, BELOW);
    int a = roomd (u, ABOVE);

    room[l].dist = min_int (room[l].dist, room[u].dist + 1);
    room[r].dist = min_int (room[r].dist, room[u].dist + 1);
    room[b].dist = min_int (room[b].dist, room[u].dist + 1);
    room[a].dist = min_int (room[a].dist, room[u].dist + 1);
  }

  return room[r1].dist;
}

int
min_room_dist (struct room_dist room[], int *dmax)
{
  int r = -1;
  int d = INT_MAX;

  int i;
  for (i = 0; i < ROOMS; i++)
    if (! room[i].visited) {
      if (room[i].dist < d) {
        d = room[i].dist;
        r = i;
      }
      if (room[i].dist > *dmax
          && room[i].dist < INT_MAX)
        *dmax = room[i].dist;
    }

  return r;
}

bool
coord_eq (struct coord *_c0, struct coord *_c1)
{
  struct coord c0, c1;
  ncoord (_c0, &c0);
  ncoord (_c1, &c1);

  return c0.room == c1.room && c0.x == c1.x
    && c0.y == c1.y;
}

struct coord *
ncoord (struct coord *c, struct coord *nc)
{
  if (nc != c) *nc = *c;

  bool m;

  do {
    m = false;

    int ra, rb, rl, rr;
    /* int rab, rba, rlr, rrl; */

    ra = roomd (nc->room, ABOVE);
    rb = roomd (nc->room, BELOW);
    rl = roomd (nc->room, LEFT);
    rr = roomd (nc->room, RIGHT);

    /* rab = roomd (ra, BELOW); */
    /* rba = roomd (rb, ABOVE); */
    /* rlr = roomd (rl, RIGHT); */
    /* rrl = roomd (rr, LEFT); */

    if (nc->x < 0
               /* && (rl != nc->room || coord_wa) */
               /* && rlr == nc->room */
               ) {
      nc->x += PLACE_WIDTH * PLACES;
      nc->prev_room = nc->room;
      nc->room = rl;
      nc->xd = LEFT;
      m = true;
    } else if (nc->x >= PLACE_WIDTH * PLACES
               /* && (rr != nc->room || coord_wa) */
               /* && rrl == nc->room */
               ) {
      nc->x -= PLACE_WIDTH * PLACES;
      nc->prev_room = nc->room;
      nc->room = rr;
      nc->xd = RIGHT;
      m = true;
    } else if (nc->y < 0
        /* && (ra != nc->room || coord_wa) */
        /* && rab == nc->room */
        ) {
      nc->y += PLACE_HEIGHT * FLOORS;
      nc->prev_room = nc->room;
      nc->room = ra;
      nc->xd = ABOVE;
      m = true;
    } else if (nc->y >= PLACE_HEIGHT * FLOORS + 11
               /* && (rb != nc->room || coord_wa) */
               /* && rba == nc->room */
               ) {
      nc->y -= PLACE_HEIGHT * FLOORS;
      nc->prev_room = nc->room;
      nc->room = rb;
      nc->xd = BELOW;
      m = true;
    }
  } while (m);

  return nc;
}

bool
is_valid_pos (struct pos *p)
{
  return p->room >= 0 && p->room < ROOMS;
}

struct pos *
npos (struct pos *p, struct pos *np)
{
  if (np != p) *np = *p;

  bool m;

  do {
    m = false;

    if (np->floor < 0) {
      np->floor += FLOORS;
      np->room = roomd (np->room, ABOVE);
      m = true;
    } else if (np->floor >= FLOORS) {
      np->floor -= FLOORS;
      np->room = roomd (np->room, BELOW);
      m = true;
    } else if (np->place < 0) {
      np->place += PLACES;
      np->room = roomd (np->room, LEFT);
      m = true;
    } else if (np->place >= PLACES) {
      np->place -= PLACES;
      np->room = roomd (np->room, RIGHT);
      m = true;
    }
  } while (m);

  return np;
}

struct coord *
nframe (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  struct coord ml; _m (f, &ml);
  struct coord mr = ml;

  ml.x += -3;
  mr.x += +3;

  struct coord nml; ncoord (&ml, &nml);
  struct coord nmr; ncoord (&mr, &nmr);

  if (nml.room == nmr.room
      || (f->c.room != nml.room
          && f->c.room != nmr.room)) {
    *c = (f->dir == LEFT) ? nml : nmr;
    int dx = (f->dir == LEFT) ? +3 : -3;
    c->x -= d.w / 2 - dx;
    c->y -= d.h / 2;
  } else *c = f->c;

  return c;
}

struct pos *
pos2room (struct pos *p, int room, struct pos *pv)
{
  *pv = *p;
  /* npos (pv, pv); */

  if (pv->room == room) return pv;

  struct pos pb, pa, pl, pr;

  pb = pa = pl = pr = *pv;

  int mpb, mpa, mpr, mpl;
  mpb = mpa = mpr = mpl = INT_MAX;

  int ra, rb, rl, rr;

  ra = roomd (room, ABOVE);
  rb = roomd (room, BELOW);
  rl = roomd (room, LEFT);
  rr = roomd (room, RIGHT);

  if (rb == pv->room) {
    pb.floor += FLOORS;
    pb.room = room;
    mpb = pos_mod (&pb, p);
  }

  if (ra == pv->room) {
    pa.floor -= FLOORS;
    pa.room = room;
    mpa = pos_mod (&pa, p);
  }

  if (rr == pv->room) {
    pr.place += PLACES;
    pr.room = room;
    mpr = pos_mod (&pr, p);
  }

  if (rl == pv->room) {
    pl.place -= PLACES;
    pl.room = room;
    mpl = pos_mod (&pl, p);
  }

  int lm = mpb;
  lm = min_int (lm, mpa);
  lm = min_int (lm, mpr);
  lm = min_int (lm, mpl);

  if (lm == mpb) *pv = pb;
  else if (lm == mpa) *pv = pa;
  else if (lm == mpr) *pv = pr;
  else if (lm == mpl) *pv = pl;

  return pv;
}

struct coord *
coord2room (struct coord *c, int room, struct coord *cv)
{
  *cv = *c;
  ncoord (cv, cv);

  if (cv->room == room) return cv;

  struct coord cb, ca, cl, cr;

  cb = ca = cl = cr = *cv;

  int mcb, mca, mcr, mcl;
  mcb = mca = mcr = mcl = INT_MAX;

  int ra, rb, rl, rr;
  int rab, rba, rlr, rrl;

  ra = roomd (room, ABOVE);
  rb = roomd (room, BELOW);
  rl = roomd (room, LEFT);
  rr = roomd (room, RIGHT);

  rab = roomd (ra, BELOW);
  rba = roomd (rb, ABOVE);
  rlr = roomd (rl, RIGHT);
  rrl = roomd (rr, LEFT);

  if (rb == cv->room
      && rba == room) {
    cb.y += PLACE_HEIGHT * FLOORS;
    cb.room = room;
    mcb = coord_mod (&cb);
  }

  if (ra == cv->room
      && rab == room) {
    ca.y -= PLACE_HEIGHT * FLOORS;
    ca.room = room;
    mca = coord_mod (&ca);
  }

  if (rr == cv->room
      && rrl == room) {
    cr.x += PLACE_WIDTH * PLACES;
    cr.room = room;
    mcr = coord_mod (&cr);
  }

  if (rl == cv->room
      && rlr == room) {
    cl.x -= PLACE_WIDTH * PLACES;
    cl.room = room;
    mcl = coord_mod (&cl);
  }

  int lm = mcb;
  lm = min_int (lm, mca);
  lm = min_int (lm, mcr);
  lm = min_int (lm, mcl);

  if (lm == mcb) *cv = cb;
  else if (lm == mca) *cv = ca;
  else if (lm == mcr) *cv = cr;
  else if (lm == mcl) *cv = cl;

  return cv;
}

struct coord *
frame2room (struct frame *f, int room, struct coord *cv)
{
  *cv = f->c;
  ncoord (cv, cv);

  if (cv->room == room) return cv;

  struct coord tl = *cv;
  struct coord tr = *cv;
  struct coord bl = *cv;
  struct coord br = *cv;

  int w = al_get_bitmap_width (f->b);
  int h = al_get_bitmap_height (f->b);

  tr.x = tl.x + w - 1;
  bl.y = tl.y + h - 1;
  br.x = tl.x + w - 1;
  br.y = tl.y + h - 1;

  struct coord ntl; coord2room (&tl, room, &ntl);
  struct coord ntr; coord2room (&tr, room, &ntr);
  struct coord nbl; coord2room (&bl, room, &nbl);
  struct coord nbr; coord2room (&br, room, &nbr);

  if (ntl.room == room) *cv = ntl;
  else if (ntr.room == room) {
    *cv = ntr;
    cv->x = ntr.x - w + 1;
  } else if (nbl.room == room) {
    *cv = nbl;
    cv->y = nbl.y - h + 1;
  } else if (nbr.room == room) {
    *cv = nbr;
    cv->x = nbr.x - w + 1;
    cv->y = nbr.y - h + 1;
  }

  return cv;
}

struct coord *
rect2room (struct rect *r, int room, struct coord *cv)
{
  *cv = r->c;
  ncoord (cv, cv);

  if (cv->room == room) return cv;

  struct coord tl = *cv;
  struct coord tr = *cv;
  struct coord bl = *cv;
  struct coord br = *cv;

  tr.x = tl.x + r->w - 1;
  bl.y = tl.y + r->h - 1;
  br.x = tl.x + r->w - 1;
  br.y = tl.y + r->h - 1;

  struct coord ntl; coord2room (&tl, room, &ntl);
  struct coord ntr; coord2room (&tr, room, &ntr);
  struct coord nbl; coord2room (&bl, room, &nbl);
  struct coord nbr; coord2room (&br, room, &nbr);

  if (ntl.room == room) *cv = ntl;
  else if (ntr.room == room) {
    *cv = ntr;
    cv->x = ntr.x - r->w + 1;
  } else if (nbl.room == room) {
    *cv = nbl;
    cv->y = nbl.y - r->h + 1;
  } else if (nbr.room == room) {
    *cv = nbr;
    cv->x = nbr.x - r->w + 1;
    cv->y = nbr.y - r->h + 1;
  }

  return cv;
}

bool
coord4draw (struct coord *c, int room, struct coord *cv)
{
  if (cv != c) *cv = *c;
  ncoord (cv, cv);

  bool m = false;

  if (cv->y < 11 &&
      roomd (room, BELOW) == cv->room
      && room != cv->room) {
    cv->y += PLACE_HEIGHT * FLOORS;
    cv->room = room;
    m = true;
  } else if (cv->y >= PLACE_HEIGHT * FLOORS
             && roomd (room, ABOVE) == cv->room
             && room != cv->room) {
    cv->y -= PLACE_HEIGHT * FLOORS;
    cv->room = room;
    m = true;
  }

  return m;
}

int
pos_mod (struct pos *p0, struct pos *p1)
{
  return (p0->floor - p1->floor) * (p0->floor - p1->floor)
    + (p0->place - p1->place) * (p0->place - p1->place);
}

int
coord_mod (struct coord *c)
{
  return c->y * c->y + c->x * c->x;
}

struct pos *
pos_gen (struct coord *c, struct pos *p, int dx, int dy)
{
  p->room = c->room;
  /* p->place = (c->x - dx) / PLACE_WIDTH; */
  /* p->floor = (c->y - dy) / PLACE_HEIGHT; */

  /* if (c->x < dx) p->place = -1; */
  /* if (c->y < dy) p->floor = -1; */

  if (c->x >= dx) p->place = (c->x - dx) / PLACE_WIDTH;
  else if (0 <= c->x && c->x < dx) p->place = -1;
  else p->place = -(((dx - c->x - 1) / PLACE_WIDTH) + 1);

  if (c->y >= dy) p->floor = (c->y - dy) / PLACE_HEIGHT;
  else if (0 <= c->y && c->y < dy) p->floor = -1;
  else p->floor = -(((dy - c->y - 1) / PLACE_HEIGHT) + 1);

  return p;
}

struct pos *
pos (struct coord *c, struct pos *p)
{
  return pos_gen (c, p, 15, 3);
}

struct pos *
posf (struct coord *c, struct pos *p)
{
  return pos_gen (c, p, 0, 3);
}

struct pos *
prel (struct pos *p, struct pos *pr, int floor, int place)
{
  *pr = *p;
  pr->floor += floor;
  pr->place += place;
  return pr;
}

int
cpos (struct pos *p0, struct pos *p1)
{
  struct pos np0, np1;
  npos (p0, &np0);
  npos (p1, &np1);

  /* if (np0.room < np1.room) return -1; */
  /* else if (np0.room > np1.room) return 1; */
  /* else if (np0.floor < np1.floor) return 1; */
  /* else if (np0.floor > np1.floor) return -1; */
  /* else if (np0.place < np1.place) return -1; */
  /* else if (np0.place > np1.place) return 1; */
  /* else return 0; */

  if (np0.room < np1.room) return -1;
  else if (np0.room > np1.room) return 1;
  else if (np0.place < np1.place) return -1;
  else if (np0.place > np1.place) return 1;
  else if (np0.floor < np1.floor) return 1;
  else if (np0.floor > np1.floor) return -1;
  else return 0;
}

int
ccoord (struct coord *c0, struct coord *c1)
{
  struct coord nc0, nc1;
  ncoord (c0, &nc0);
  ncoord (c1, &nc1);

  if (nc0.room < nc1.room) return -1;
  else if (nc0.room > nc1.room) return 1;
  else if (nc0.y < nc1.y) return 1;
  else if (nc0.y > nc1.y) return -1;
  else if (nc0.x < nc1.x) return -1;
  else if (nc0.x > nc1.x) return 1;
  else return 0;
}

bool
peq (struct pos *p0, struct pos *p1)
{
  if (p0->room < 0 || p1->room < 0)
    return false;

  /* optimization: sufficient condition */
  if (p0->room == p1->room
      && p0->floor == p1->floor
      && p0->place == p1->place) return true;

  struct pos np0, np1;

  npos (p0, &np0);
  npos (p1, &np1);

  return np0.room == np1.room && np0.floor == np1.floor
    && np0.place == np1.place;
}

bool
peqr (struct pos *p0, struct pos *p1, int floor, int place)
{
  struct pos np;
  return peq (p0, prel (p1, &np, floor, place));
}

struct pos *
reflect_pos_h (struct pos *p0, struct pos *p1)
{
  p1->room = p0->room;
  p1->floor = p0->floor;
  p1->place = (PLACES - 1) - p0->place;
  return p1;
}

struct pos *
reflect_pos_v (struct pos *p0, struct pos *p1)
{
  p1->room = p0->room;
  p1->floor = (FLOORS - 1) - p0->floor;
  p1->place = p0->place;
  return p1;
}

struct pos *
random_pos (struct pos *p)
{
  p->room = prandom (ROOMS - 1);
  p->floor = prandom (FLOORS - 1);
  p->place = prandom (PLACES - 1);
  return p;
}

double
dist_coord (struct coord *a, struct coord *b)
{
  int dx = a->x - b->x;
  int dy = a->y - b->y;
  return sqrt (dx * dx + dy * dy);
}

struct frame *
place_frame (struct frame *f, struct frame *nf, ALLEGRO_BITMAP *b,
             struct pos *p, int dx, int dy)
{
  struct pos pv;
  *nf = *f;
  pos2room (p, f->c.room, &pv);
  nf->b = b;
  nf->c.room = pv.room;
  nf->c.x = PLACE_WIDTH * pv.place + dx;
  nf->c.y = PLACE_HEIGHT * pv.floor + dy;
  return nf;
}

coord_f
opposite_cf (coord_f cf)
{
  if (cf == _ml) return _mr;
  if (cf == _mr) return _ml;
  if (cf == _mf) return _mba;
  if (cf == _mba) return _mf;
  if (cf == _tl) return _tr;
  if (cf == _tr) return _tl;
  if (cf == _tf) return _tb;
  if (cf == _tb) return _tf;
  if (cf == _bl) return _br;
  if (cf == _br) return _bl;
  if (cf == _bf) return _bb;
  if (cf == _bb) return _bf;
  return cf;
}

enum dir
opposite_dir (enum dir dir)
{
  switch (dir) {
  case LEFT: return RIGHT;
  case RIGHT: return LEFT;
  case ABOVE: return BELOW;
  case BELOW: return ABOVE;
  }

  return LEFT;
}

enum dir
perpendicular_dir (enum dir dir, int n)
{
  switch (dir) {
  case LEFT: return n ? ABOVE : BELOW;
  case RIGHT: return n ? ABOVE : BELOW;
  case ABOVE: return n ? LEFT : RIGHT;
  case BELOW: return n ? LEFT : RIGHT;
  }

  return LEFT;
}

struct dim *
dim (struct frame *f, struct dim *d)
{
  d->w = al_get_bitmap_width (f->b);
  d->h = al_get_bitmap_height (f->b);
  d->x = f->c.x;
  d->y = f->c.y;
  d->fx = (f->dir == LEFT) ? d->x : d->x + d->w - 1;
  d->bx = (f->dir == LEFT) ? d->x + d->w - 1 : d->x;
  return d;
}

/* cons coordinates */

struct coord *
con_m (struct pos *p, struct coord *c)
{
  c->room = p->room;
  c->x = PLACE_WIDTH * p->place + 15 + PLACE_WIDTH / 2;
  c->y = PLACE_HEIGHT * p->floor + 3 + PLACE_HEIGHT / 2;
  return c;
}


/* <->
   000
   010
   000 */

struct coord *
_m (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w / 2;
  c->y = d.y + d.h / 2;
  return c;
}

/* <->
   010
   000
   000 */

struct coord *
_mt (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w / 2;
  c->y = d.y;
  return c;
}

/* <->
   000
   000
   010 */

struct coord *
_mbo (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w / 2;
  c->y = d.y + d.h - 1;
  return c;
}

/* <->
   000
   100
   000 */

struct coord *
_ml (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x;
  c->y = d.y + d.h / 2;
  return c;
}

/* <->
   000
   001
   000 */

struct coord *
_mr (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w - 1;
  c->y = d.y + d.h / 2;
  return c;
}

/* <-- -->
   000 000
   100 001
   000 000 */

struct coord *
_mf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.fx;
  c->y = d.y + d.h / 2;
  return c;
}

/* <-- -->
   000 000
   001 100
   000 000 */

struct coord *
_mba (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.bx;
  c->y = d.y + d.h / 2;
  return c;
}

/* <->
   100
   000
   000 */

struct coord *
_tl (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x;
  c->y = d.y;
  return c;
}

/* <->
   001
   000
   000 */

struct coord *
_tr (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w - 1;
  c->y = d.y;
  return c;
}

/* <-- -->
   100 001
   000 000
   000 000 */

struct coord *
_tf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.fx;
  c->y = d.y;
  return c;
}

/* <-- -->
   001 100
   000 000
   000 000 */

struct coord *
_tb (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.bx;
  c->y = d.y;
  return c;
}

/* <->
   000
   000
   100 */

struct coord *
_bl (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x;
  c->y = d.y + d.h - 1;
  return c;
}

/* <->
   000
   000
   001 */

struct coord *
_br (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.x + d.w - 1;
  c->y = d.y + d.h - 1;
  return c;
}

/* <-- -->
   000 000
   000 000
   100 001 */

struct coord *
_bf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.fx;
  c->y = d.y + d.h -1;
  return c;
}

/* <-- -->
   000 000
   000 000
   001 100 */

struct coord *
_bb (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  c->room = f->c.room;
  c->x = d.bx;
  c->y = d.y + d.h -1;
  return c;
}

struct con *
surveyo (coord_f cf, int dx, int dy, pos_f pf, struct frame *f,
         struct coord *c, struct pos *p, struct pos *np)
{
  int dir = (f->dir == LEFT) ? +1 : -1;

  struct coord _c;
  cf (f, &_c);
  _c.x += dir * dx;
  _c.y += dy;
  *c = _c;
  pf (c, p);
  npos (p, np);
  return con (np);
}

struct coord *
place_on_the_ground (struct frame *f, struct coord *c)
{
  struct coord mbo; struct pos np, pmbo;
  *c = f->c;
  survey (_mbo, pos, f, &mbo, &pmbo, &np);
  f->c.y += (PLACE_HEIGHT * pmbo.floor + 55) - mbo.y;
  return c;
}

struct coord *
place_at_distance (struct frame *f0, coord_f cf0,
                   struct frame *f1, coord_f cf1,
                   int d, enum dir dir, struct coord *c)
{
  struct coord c0; cf0 (f0, &c0);
  struct coord c1; cf1 (f1, &c1);
  struct coord tl; _tl (f1, &tl);

  *c = c1;
  coord2room (&c0, c1.room, &c0);

  c->x = (c0.x - c1.x) + ((dir == RIGHT) ? d : -d) + tl.x;

  return c;
}

struct coord *
place_at_pos (struct frame *f, coord_f cf, struct pos *p, struct coord *c)
{
  struct coord _c; cf (f, &_c);
  struct coord tl; _tl (f, &tl);

  int dx = _c.x - tl.x;
  int dy = _c.y - tl.y;

  c->room = p->room;
  c->x = PLACE_WIDTH * p->place + 30 - dx;
  c->y = PLACE_HEIGHT * p->floor + 34 - dy;

  return c;
}
