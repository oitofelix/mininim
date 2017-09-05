/*
  position.c -- position module;

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

static struct bitmap_rcoord *bitmap_rcoord_cache;
static size_t bitmap_rcoord_cache_nmemb;

bool coord_wa;

int
typed_int (int i, int n, int f, int *nr, int *nf)
{
  int u = abs (i) % (n * f);
  int t = (i >= 0 || u == 0) ? u : n * f - u;

  if (nr) *nr = t % n;
  if (nf) *nf = (t / n) % f;

  return t;
}

int
room_val (int r)
{
  return typed_int (r, ROOMS, 1, NULL, NULL);
}

int *
roomd_ptr (struct level *l, int room, enum dir dir)
{
  switch (dir) {
  case LEFT: return &llink (l, room)->l;
  case RIGHT: return &llink (l, room)->r;
  case ABOVE: return &llink (l, room)->a;
  case BELOW: return &llink (l, room)->b;
  default: assert (false); return NULL;
  }
}

int
roomd (struct level *l, int room, enum dir dir)
{
  return room_val (*roomd_ptr (l, room, dir));
}

void
link_room (struct level *l, int room0, int room1, enum dir dir)
{
  if (room0) *roomd_ptr (l, room0, dir) = room_val (room1);
}

void
mirror_link (struct level *l, int room, enum dir dir0, enum dir dir1)
{
  int r0 = roomd (l, room, dir0);
  int r1 = roomd (l, room, dir1);
  link_room (l, room, r0, dir1);
  link_room (l, room, r1, dir0);
}

int
roomd_n0 (struct level *l, int room, enum dir dir)
{
  int r = roomd (l, room, dir);
  return r ? r : room;
}

bool
is_room_adjacent (struct level *l, int room0, int room1)
{
  return room0 == room1
    || roomd (l, room0, LEFT) == room1
    || roomd (l, room0, RIGHT) == room1
    || roomd (l, room0, ABOVE) == room1
    || roomd (l, room0, BELOW) == room1;
}

int
room_dist (struct level *lv, int r0, int r1, int max)
{
  struct room_dist room[ROOMS];

  /* begin optimization block */
  if (r0 == r1) return 0;

  if (roomd (lv, r0, LEFT) == r1
      || roomd (lv, r0, RIGHT) == r1
      || roomd (lv, r0, BELOW) == r1
      || roomd (lv, r0, ABOVE) == r1)
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

    int l = roomd (lv, u, LEFT);
    int r = roomd (lv, u, RIGHT);
    int b = roomd (lv, u, BELOW);
    int a = roomd (lv, u, ABOVE);

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
coord_eq (struct coord *c0, struct coord *c1)
{
  if (! is_valid_coord (c0) || ! is_valid_coord (c1))
    return false;

  struct coord nc0, nc1;
  ncoord (c0, &nc0);
  ncoord (c1, &nc1);

  return nc0.l == nc1.l
    && nc0.room == nc1.room
    && nc0.x == nc1.x
    && nc0.y == nc1.y;
}

struct coord *
new_coord (struct coord *c, struct level *l, int room, lua_Number x, lua_Number y)
{
  c->l = l;
  c->room = room;
  c->x = x;
  c->y = y;
  return c;
}

struct coord *
invalid_coord (struct coord *c)
{
  return new_coord (c, NULL, -1, -1, -1);
}

bool
is_valid_coord (struct coord *c)
{
  return cutscene || (c->l && c->room >= 0
                      /* && c->room < ROOMS */
                      );
}

struct coord *
ncoord (struct coord *c, struct coord *nc)
{
  /* assert (is_valid_coord (c)); */

  if (nc != c) *nc = *c;

  bool m, allow_weak, allow_zero;

  do {
    bool nl = nc->x < 0;
    bool nr = nc->x >= PLACE_WIDTH * PLACES;
    bool na = nc->y < 0;
    bool nb = nc->y >= PLACE_HEIGHT * FLOORS + 11;

    if (! nl && ! nr && ! na && ! nb) break;

    m = allow_weak = allow_zero = false;

    int ra, rb, rl, rr;
    ra = roomd (nc->l, nc->room, ABOVE);
    rb = roomd (nc->l, nc->room, BELOW);
    rl = roomd (nc->l, nc->room, LEFT);
    rr = roomd (nc->l, nc->room, RIGHT);

    int rab, rba, rlr, rrl;
    rab = roomd (nc->l, ra, BELOW);
    rba = roomd (nc->l, rb, ABOVE);
    rlr = roomd (nc->l, rl, RIGHT);
    rrl = roomd (nc->l, rr, LEFT);

  retry:
    if (nl && (rlr == nc->room || allow_weak)
        && (rl != 0 || allow_zero)) {
      nc->x += PLACE_WIDTH * PLACES;
      nc->prev_room = nc->room;
      nc->room = rl;
      nc->xd = LEFT;
      m = true;
    } else if (nr && (rrl == nc->room || allow_weak)
               && (rr != 0 || allow_zero)) {
      nc->x -= PLACE_WIDTH * PLACES;
      nc->prev_room = nc->room;
      nc->room = rr;
      nc->xd = RIGHT;
      m = true;
    } else if (na && (rab == nc->room || allow_weak)
               && (ra != 0 || allow_zero)) {
      nc->y += PLACE_HEIGHT * FLOORS;
      nc->prev_room = nc->room;
      nc->room = ra;
      nc->xd = ABOVE;
      m = true;
    } else if (nb && (rba == nc->room || allow_weak)
               && (rb != 0 || allow_zero)) {
      nc->y -= PLACE_HEIGHT * FLOORS;
      nc->prev_room = nc->room;
      nc->room = rb;
      nc->xd = BELOW;
      m = true;
    }

    if (! m && ! allow_weak) {
      allow_weak = true;
      goto retry;
    }

    if (! m && ! allow_zero) {
      allow_zero = true;
      goto retry;
    }
  } while (m);

  return nc;
}

struct pos *
new_pos (struct pos *p, struct level *l, int room, int floor, int place)
{
  p->l = l;
  p->room = room;
  p->floor = floor;
  p->place = place;
  return p;
}

struct pos *
invalid_pos (struct pos *p)
{
  return new_pos (p, &global_level, -1, -1, -1);
}

bool
is_valid_pos (struct pos *p)
{
  return cutscene || (p->l && p->room >= 0
                      /* && p->room < ROOMS */
                      );
}

struct pos *
npos (struct pos *p, struct pos *np)
{
  /* assert (is_valid_pos (p)); */

  if (np != p) *np = *p;

  bool m;

  np->room = room_val (np->room);

  do {
    m = false;

    if (np->floor < 0) {
      np->floor += FLOORS;
      np->room = roomd (np->l, np->room, ABOVE);
      m = true;
    } else if (np->floor >= FLOORS) {
      np->floor -= FLOORS;
      np->room = roomd (np->l, np->room, BELOW);
      m = true;
    } else if (np->place < 0) {
      np->place += PLACES;
      np->room = roomd (np->l, np->room, LEFT);
      m = true;
    } else if (np->place >= PLACES) {
      np->place -= PLACES;
      np->room = roomd (np->l, np->room, RIGHT);
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
    if ((f->dir == LEFT && ! nml.room)
        || (f->dir == RIGHT && ! nmr.room))
        return c;

    *c = (f->dir == LEFT) ? nml : nmr;
    lua_Number dx = (f->dir == LEFT) ? +3 : -3;
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

  ra = roomd (pv->l, room, ABOVE);
  rb = roomd (pv->l, room, BELOW);
  rl = roomd (pv->l, room, LEFT);
  rr = roomd (pv->l, room, RIGHT);

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

  lua_Number mcb, mca, mcr, mcl;
  mcb = mca = mcr = mcl = DBL_MAX;

  int ra, rb, rl, rr;
  int rab, rba, rlr, rrl;

  ra = roomd (cv->l, room, ABOVE);
  rb = roomd (cv->l, room, BELOW);
  rl = roomd (cv->l, room, LEFT);
  rr = roomd (cv->l, room, RIGHT);

  rab = roomd (cv->l, ra, BELOW);
  rba = roomd (cv->l, rb, ABOVE);
  rlr = roomd (cv->l, rl, RIGHT);
  rrl = roomd (cv->l, rr, LEFT);

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

  lua_Number lm = mcb;
  lm = fmin (lm, mca);
  lm = fmin (lm, mcr);
  lm = fmin (lm, mcl);

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

  if (cv->room == room) return cv;

  /* ncoord (cv, cv); */

  struct coord tl = *cv;
  struct coord tr = *cv;
  struct coord bl = *cv;
  struct coord br = *cv;

  lua_Number w = IW (get_bitmap_width (f->b));
  lua_Number h = IH (get_bitmap_height (f->b));

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

struct rect *
new_rect (struct rect *r, int room, lua_Number x, lua_Number y,
          lua_Number w, lua_Number h)
{
  new_coord (&r->c, &global_level, room, x, y);
  r->w = w;
  r->h = h;
  return r;
}

struct rect *
coord2rect (struct coord *c, struct rect *r)
{
  r->c = *c;
  r->w = 0;
  r->h = 0;
  return r;
}

bool
rect_eq (struct rect *r0, struct rect *r1)
{
  return coord_eq (&r0->c, &r1->c)
    && r0->w == r1->w && r0->h == r1->h;
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
      roomd (cv->l, room, BELOW) == cv->room
      && room != cv->room) {
    cv->y += PLACE_HEIGHT * FLOORS;
    cv->room = room;
    m = true;
  } else if (cv->y >= PLACE_HEIGHT * FLOORS
             && roomd (cv->l, room, ABOVE) == cv->room
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
pos_gen (struct coord *c, struct pos *p, lua_Number dx, lua_Number dy)
{
  int floor, place;

  if (c->x >= dx) place = (c->x - dx) / PLACE_WIDTH;
  else if (0 <= c->x && c->x < dx) place = -1;
  else place = -(((dx - c->x - 1) / PLACE_WIDTH) + 1);

  if (c->y >= dy) floor = (c->y - dy) / PLACE_HEIGHT;
  else if (0 <= c->y && c->y < dy) floor = -1;
  else floor = -(((dy - c->y - 1) / PLACE_HEIGHT) + 1);

  return new_pos (p, c->l, c->room, floor, place);
}

struct pos *
posb (struct coord *c, struct pos *p)
{
  return pos_gen (c, p, 23, 3);
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

  if (np0.l < np1.l) return -1;
  else if (np0.l > np1.l) return 1;
  else if (np0.room < np1.room) return -1;
  else if (np0.room > np1.room) return 1;
  else if (np0.place < np1.place) return -1;
  else if (np0.place > np1.place) return 1;
  else if (np0.floor < np1.floor) return 1;
  else if (np0.floor > np1.floor) return -1;
  else return 0;
}

int
cpos_by_room (struct pos *p0, struct pos *p1)
{
  return p0->room - p1->room;
}

int
ccoord (struct coord *c0, struct coord *c1)
{
  struct coord nc0, nc1;
  ncoord (c0, &nc0);
  ncoord (c1, &nc1);

  if (nc0.l < nc1.l) return -1;
  else if (nc0.l > nc1.l) return 1;
  else if (nc0.room < nc1.room) return -1;
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
  struct pos np0, np1;

  if (! is_valid_pos (p0) || ! is_valid_pos (p1))
    return false;

  npos (p0, &np0);
  npos (p1, &np1);

  return
    /* np0.l == np1.l && */
    np0.room == np1.room
    && np0.floor == np1.floor
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
  return new_pos (p1, p0->l, p0->room, p0->floor, (PLACES - 1) - p0->place);
}

struct pos *
reflect_pos_v (struct pos *p0, struct pos *p1)
{
  return new_pos (p1, p0->l, p0->room, (FLOORS - 1) - p0->floor, p0->place);
}

struct pos *
random_pos (struct level *l, struct pos *p)
{
  return new_pos (p, l, prandom (ROOMS - 2) + 1, prandom (FLOORS - 1),
                  prandom (PLACES - 1));
}

struct pos *
get_new_rel_pos (struct pos *old_src, struct pos *old_dest,
                 struct pos *new_src, struct pos *new_dest)
{

  struct pos nold_src; npos (old_src, &nold_src);
  struct pos nold_dest; npos (old_dest, &nold_dest);
  struct pos nnew_src; npos (new_src, &nnew_src);

  pos2room (&nold_dest, nold_src.room, &nold_dest);
  if (nold_dest.room != nold_src.room)
    return invalid_pos (new_dest);

  new_pos (new_dest, nnew_src.l, nnew_src.room,
           nnew_src.floor + (nold_dest.floor - nold_src.floor),
           nnew_src.place + (nold_dest.place - nold_src.place));

  return npos (new_dest, new_dest);
}

enum dir
random_dir (void)
{
  switch (prandom (3)) {
  case 0: return LEFT;
  case 1: return RIGHT;
  case 2: return ABOVE;
  case 3: return BELOW;
  default: assert (false);
    return LEFT;
  }
}

lua_Number
dist_coord (struct coord *a, struct coord *b)
{
  lua_Number dx = a->x - b->x;
  lua_Number dy = a->y - b->y;
  return sqrt (dx * dx + dy * dy);
}

struct frame *
place_frame (struct frame *f, struct frame *nf, ALLEGRO_BITMAP *b,
             struct pos *p, lua_Number dx, lua_Number dy)
{
  struct pos pv;
  *nf = *f;
  pos2room (p, f->c.room, &pv);
  nf->b = b;
  new_coord (&nf->c, pv.l, pv.room,
             dx == MIGNORE ? f->c.x : PLACE_WIDTH * pv.place + dx,
             dy == MIGNORE ? f->c.y : PLACE_HEIGHT * pv.floor + dy);
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
  default: assert (false);
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
  d->w = IW (get_bitmap_width (f->b));
  d->h = IH (get_bitmap_height (f->b));
  d->x = f->c.x;
  d->y = f->c.y;
  d->fx = (f->dir == LEFT) ? d->x : d->x + d->w - 1;
  d->bx = (f->dir == LEFT) ? d->x + d->w - 1 : d->x;
  return d;
}

/* tiles coordinates */

struct coord *
tile_coord (struct pos *p, coord_f cf, struct coord *c)
{
  static ALLEGRO_BITMAP *tile_bitmap = NULL;

  if (! tile_bitmap)
    tile_bitmap = create_bitmap (PLACE_WIDTH, PLACE_HEIGHT);
  struct frame f;

  f.dir = LEFT;
  f.b = tile_bitmap;
  new_coord (&f.c, p->l, p->room,
             PLACE_WIDTH * p->place + 15,
             PLACE_HEIGHT * p->floor + 3);
  cf (&f, c);
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
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w / 2,
               d.y + d.h / 2);
}

/* <->
   010
   000
   000 */

struct coord *
_mt (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w / 2,
               d.y);
}

/* <->
   000
   000
   010 */

struct coord *
_mbo (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w / 2,
               d.y + d.h - 1);
}

/* <->
   000
   100
   000 */

struct coord *
_ml (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x,
               d.y + d.h / 2);
}

/* <->
   000
   001
   000 */

struct coord *
_mr (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w - 1,
               d.y + d.h / 2);
}

/* <-- -->
   000 000
   100 001
   000 000 */

struct coord *
_mf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.fx,
               d.y + d.h / 2);
}

/* <-- -->
   000 000
   001 100
   000 000 */

struct coord *
_mba (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.bx,
               d.y + d.h / 2);
}

/* <->
   100
   000
   000 */

struct coord *
_tl (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x,
               d.y);
}

/* <->
   001
   000
   000 */

struct coord *
_tr (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w - 1,
               d.y);
}

/* <-- -->
   100 001
   000 000
   000 000 */

struct coord *
_tf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.fx,
               d.y);
}

/* <-- -->
   001 100
   000 000
   000 000 */

struct coord *
_tb (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.bx,
               d.y);
}

/* <->
   000
   000
   100 */

struct coord *
_bl (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x,
               d.y + d.h - 1);
}

/* <->
   000
   000
   001 */

struct coord *
_br (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.x + d.w - 1,
               d.y + d.h - 1);
}

/* <-- -->
   000 000
   000 000
   100 001 */

struct coord *
_bf (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.fx,
               d.y + d.h - 1);
}

/* <-- -->
   000 000
   000 000
   001 100 */

struct coord *
_bb (struct frame *f, struct coord *c)
{
  struct dim d; dim (f, &d);
  return
    new_coord (c, f->c.l, f->c.room,
               d.bx,
               d.y + d.h - 1);
}

coord_f
str2coord_f (const char *name)
{
  if (! name) return NULL;

  if (! strcasecmp (name, "M")) return _m;
  else if (! strcasecmp (name, "MT")) return _mt;
  else if (! strcasecmp (name, "MBO")) return _mbo;
  else if (! strcasecmp (name, "ML")) return _ml;
  else if (! strcasecmp (name, "MR")) return _mr;
  else if (! strcasecmp (name, "MF")) return _mf;
  else if (! strcasecmp (name, "MBA")) return _mba;
  else if (! strcasecmp (name, "TL")) return _tl;
  else if (! strcasecmp (name, "TR")) return _tr;
  else if (! strcasecmp (name, "TF")) return _tf;
  else if (! strcasecmp (name, "TB")) return _tb;
  else if (! strcasecmp (name, "BL")) return _bl;
  else if (! strcasecmp (name, "BR")) return _br;
  else if (! strcasecmp (name, "BF")) return _bf;
  else if (! strcasecmp (name, "BB")) return _bb;

  return NULL;
}

struct coord *
cf_rel (coord_f cf, struct frame *f, struct coord *c, lua_Number dx,
        lua_Number dy)
{
  int dir;

  if (cf == _mf || cf == _tf || cf == _bf)
    dir = (f->dir == LEFT) ? -1 : +1;
  else if (cf == _mba || cf == _tb || cf == _bb)
    dir = (f->dir == LEFT) ? +1 : -1;
  else if (cf == _ml || cf == _tl || cf == _bl)
    dir = -1;
  else if (cf == _mr || cf == _tr || cf == _br)
    dir = +1;
  else if (f->dir == LEFT)
    dir = -1;
  else dir = +1;

  cf (f, c);
  c->x += dir * dx;
  c->y += dy;

  return c;
}

int
cf_inc (struct frame *f, coord_f cf)
{
  if (cf == _mf || cf == _tf || cf == _bf)
    return -1;
  else if (cf == _mba || cf == _tb || cf == _bb)
    return +1;
  else if (cf == _ml || cf == _tl || cf == _bl)
    return f->dir == LEFT ? -1 : +1;
  else if (cf == _mr || cf == _tr || cf == _br)
    return f->dir == LEFT ? +1 : -1;
  else assert (false);
  return 0;
}

void
survey (coord_f cf, pos_f pf, struct frame *f,
        struct coord *c, struct pos *p, struct pos *np)
{
  assert (cf && f && (c || (pf && p) || (pf && np)));

  struct coord c0; struct pos p0, np0;

  if (! c) c = &c0;
  if (! p) p = &p0;
  if (! np) np = &np0;

  if (! pf) pf = pos;

  npos (pf (cf (f, c), p), np);
}

void
surveyo (coord_f cf, lua_Number dx, lua_Number dy, pos_f pf, struct frame *f,
         struct coord *c, struct pos *p, struct pos *np)
{
  assert (cf && f && (c || (pf && p) || (pf && np)));

  struct coord c0; struct pos p0, np0;

  if (! c) c = &c0;
  if (! p) p = &p0;
  if (! np) np = &np0;

  if (! pf) pf = pos;

  cf_rel (cf, f, c, dx, dy);

  npos (pf (c, p), np);
}

struct link_survey *
link_survey (struct link_survey *ls, struct level *level, int room)
{
  ls->s = room;

  ls->l = roomd (level, ls->s, LEFT);
  ls->r = roomd (level, ls->s, RIGHT);
  ls->a = roomd (level, ls->s, ABOVE);
  ls->b = roomd (level, ls->s, BELOW);

  ls->lr = roomd (level, ls->l, RIGHT);
  ls->rl = roomd (level, ls->r, LEFT);
  ls->ab = roomd (level, ls->a, BELOW);
  ls->ba = roomd (level, ls->b, ABOVE);

  ls->al = roomd (level, ls->a, LEFT);
  ls->ar = roomd (level, ls->a, RIGHT);
  ls->bl = roomd (level, ls->b, LEFT);
  ls->br = roomd (level, ls->b, RIGHT);

  ls->la = roomd (level, ls->l, ABOVE);
  ls->ra = roomd (level, ls->r, ABOVE);
  ls->lb = roomd (level, ls->l, BELOW);
  ls->rb = roomd (level, ls->r, BELOW);

  return ls;
}

struct pos_survey *
pos_survey (struct pos_survey *ps, struct pos *p)
{
  npos (p, &ps->s);

  prel (p, &ps->l, +0, -1); npos (&ps->l, &ps->l);
  prel (p, &ps->r, +0, +1); npos (&ps->r, &ps->r);
  prel (p, &ps->a, -1, +0); npos (&ps->a, &ps->a);
  prel (p, &ps->b, +1, +0); npos (&ps->b, &ps->b);

  prel (&ps->a, &ps->al, +0, -1); npos (&ps->al, &ps->al);
  prel (&ps->a, &ps->ar, +0, +1); npos (&ps->ar, &ps->ar);
  prel (&ps->b, &ps->bl, +0, -1); npos (&ps->bl, &ps->bl);
  prel (&ps->b, &ps->br, +0, +1); npos (&ps->br, &ps->br);

  prel (&ps->l, &ps->la, -1, +0); npos (&ps->la, &ps->la);
  prel (&ps->l, &ps->lb, +1, +0); npos (&ps->lb, &ps->lb);
  prel (&ps->r, &ps->ra, -1, +0); npos (&ps->ra, &ps->ra);
  prel (&ps->r, &ps->rb, +1, +0); npos (&ps->rb, &ps->rb);

  return ps;
}

bool
is_pixel_transparent (ALLEGRO_BITMAP *bitmap, int x, int y)
{
  ALLEGRO_COLOR c = al_get_pixel (bitmap, x, y);
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  return a < 255;
}

int
compare_bitmap_rcoords (const void *_c0, const void *_c1)
{
  struct bitmap_rcoord *c0 = (struct bitmap_rcoord *) _c0;
  struct bitmap_rcoord *c1 = (struct bitmap_rcoord *) _c1;

  if (c0->b < c1->b) return -1;
  else if (c0->b > c1->b) return 1;
  else return 0;
}

struct bitmap_rcoord *
get_cached_bitmap_rcoord (ALLEGRO_BITMAP *b, struct bitmap_rcoord *c)
{
  struct bitmap_rcoord cs, *csr;
  cs.b = b;

  csr = bsearch (&cs, bitmap_rcoord_cache, bitmap_rcoord_cache_nmemb, sizeof (cs),
                 compare_bitmap_rcoords);

  if (csr) {
    *c = *csr;
    return c;
  } else return NULL;
}

struct bitmap_rcoord *
bitmap_rcoord (ALLEGRO_BITMAP *b, struct bitmap_rcoord *c)
{
  if (! b) return NULL;

  struct bitmap_rcoord *cached = get_cached_bitmap_rcoord (b, c);
  if (cached) return c;

  c->b = b;
  lua_Number w = IW (get_bitmap_width (b));
  lua_Number h = IH (get_bitmap_height (b));
  al_lock_bitmap (b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);

  /* top left */
  for (c->tl.y = 0; c->tl.y < h; c->tl.y++)
    for (c->tl.x = 0; c->tl.x < w; c->tl.x++)
      if (! is_pixel_transparent (b, c->tl.x, c->tl.y)) goto end_tl;
 end_tl:
  c->tl.x = (c->tl.x < w) ? c->tl.x : 0;
  c->tl.y = (c->tl.y < h) ? c->tl.y : 0;

  /* top right */
  for (c->tr.y = 0; c->tr.y < h; c->tr.y++)
    for (c->tr.x = w - 1; c->tr.x >= 0; c->tr.x--)
      if (! is_pixel_transparent (b, c->tr.x, c->tr.y)) goto end_tr;
 end_tr:
  c->tr.x = (c->tr.x >= 0) ? c->tr.x : 0;
  c->tr.y = (c->tr.y < h) ? c->tr.y : 0;

  /* bottom left */
  for (c->bl.y = h - 1; c->bl.y >= 0; c->bl.y--)
    for (c->bl.x = 0; c->bl.x < w; c->bl.x++)
      if (! is_pixel_transparent (b, c->bl.x, c->bl.y)) goto end_bl;
 end_bl:
  c->bl.x = (c->bl.x < w) ? c->bl.x : 0;
  c->bl.y = (c->bl.y >= 0) ? c->bl.y : 0;

  /* bottom right */
  for (c->br.y = h - 1; c->br.y >= 0; c->br.y--)
    for (c->br.x = w - 1; c->br.x >= 0; c->br.x--)
      if (! is_pixel_transparent (b, c->br.x, c->br.y)) goto end_br;
 end_br:
  c->br.x = (c->br.x >= 0) ? c->br.x : 0;
  c->br.y = (c->br.y >= 0) ? c->br.y : 0;

  /* mid top */
  c->mt.x = (c->tl.x + c->tr.x) / 2;
  c->mt.y = (c->tl.y + c->tr.y) / 2;

  /* mid bottom */
  c->mbo.x = (c->bl.x + c->br.x) / 2;
  c->mbo.y = (c->bl.y + c->br.y) / 2;

  /* mid left */
  c->ml.x = (c->tl.x + c->bl.x) / 2;
  c->ml.y = (c->tl.y + c->bl.y) / 2;

  /* mid right */
  c->mr.x = (c->tr.x + c->br.x) / 2;
  c->mr.y = (c->tr.y + c->br.y) / 2;

  /* mid */
  c->m.x = (c->ml.x + c->mr.x) / 2;
  c->m.y = (c->ml.y + c->mr.y) / 2;

  al_unlock_bitmap (b);

  bitmap_rcoord_cache =
    add_to_array (c, 1, bitmap_rcoord_cache, &bitmap_rcoord_cache_nmemb,
                  bitmap_rcoord_cache_nmemb, sizeof (*c));

  qsort (bitmap_rcoord_cache, bitmap_rcoord_cache_nmemb, sizeof (*c),
         compare_bitmap_rcoords);

  return c;
}

struct coord *
place_on_the_ground_alternative (struct frame *f, struct coord *c,
                                 lua_Number dy)
{
  struct pos pm;
  *c = f->c;
  survey (_m, pos, f, NULL, &pm, NULL);
  c->y = (PLACE_HEIGHT * pm.floor + dy) - IH (get_bitmap_height (f->b));
  return c;
}

struct coord *
place_on_the_ground (struct frame *f, struct coord *c)
{
  return place_on_the_ground_alternative (f, c, 56);
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

  lua_Number dx = _c.x - tl.x;
  lua_Number dy = _c.y - tl.y;

  c->l = p->l;
  c->room = p->room;
  c->x = PLACE_WIDTH * p->place + 30 - dx;
  c->y = PLACE_HEIGHT * p->floor + 34 - dy;

  return c;
}
