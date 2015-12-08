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

#include <error.h>
#include <math.h>
#include "kernel/random.h"
#include "level.h"
#include "kid/kid.h"
#include "physics.h"
#include "room.h"
#include "pos.h"

int
roomd (int room, enum dir dir)
{
  switch (dir) {
  case LEFT: return level->link[room].l;
  case RIGHT: return level->link[room].r;
  case ABOVE: return level->link[room].a;
  case BELOW: return level->link[room].b;
  default:
    error (-1, 0, "%s: unknown direction (%i)", __func__, dir);
    return room;
  }
}

struct coord *
ncoord (struct coord *c, struct coord *nc)
{
  *nc = *c;

  int room;

  do {
    room = nc->room;

    if (nc->y < 0) {
      nc->y += PLACE_HEIGHT * FLOORS;
      nc->room = roomd (nc->room, ABOVE);
    } else if (nc->y >= PLACE_HEIGHT * FLOORS + 11) {
      nc->y -= PLACE_HEIGHT * FLOORS;
      nc->room = roomd (nc->room, BELOW);
    } else if (nc->x < 0) {
      nc->x += PLACE_WIDTH * PLACES;
      nc->room = roomd (nc->room, LEFT);
    } else if (nc->x >= PLACE_WIDTH * PLACES) {
      nc->x -= PLACE_WIDTH * PLACES;
      nc->room = roomd (nc->room, RIGHT);
    }
  } while (room != nc->room);

  do {
    room = nc->room;

    if (nc->y < 11 &&
        roomd (room_view, BELOW) == nc->room
        && room_view != nc->room) {
      nc->y += PLACE_HEIGHT * FLOORS;
      nc->room = room_view;
    } else if (nc->y >= PLACE_HEIGHT * FLOORS
               && roomd (room_view, ABOVE) == nc->room
               && room_view != nc->room) {
      nc->y -= PLACE_HEIGHT * FLOORS;
      nc->room = room_view;
    }
  } while (room != nc->room);

  return nc;
}

struct pos *
npos (struct pos *p, struct pos *np)
{
  if (np != p) *np = *p;

  int room;

  do {
    room = np->room;

    if (np->floor < 0) {
      np->floor += FLOORS;
      np->room = roomd (np->room, ABOVE);
    } else if (np->floor >= FLOORS) {
      np->floor -= FLOORS;
      np->room = roomd (np->room, BELOW);
    } else if (np->place < 0) {
      np->place += PLACES;
      np->room = roomd (np->room, LEFT);
    } else if (np->place >= PLACES) {
      np->place -= PLACES;
      np->room = roomd (np->room, RIGHT);
    }
  } while (room != np->room);

  return np;
}

/* struct pos * */
/* pos2view (struct pos *p, struct pos *pv) */
/* { */
/*   *pv = *p; */

/*   int room; */
/*   npos (pv, pv); */

/*   do { */
/*     room = pv->room; */

/*     if (pv->floor == 0 */
/*         && roomd (room_view, BELOW) == pv->room */
/*         && room_view != pv->room) { */
/*       pv->floor += FLOORS; */
/*       pv->room = room_view; */
/*     } else if (pv->floor == FLOORS - 1 */
/*                && roomd (room_view, ABOVE) == pv->room */
/*                && room_view != pv->room) { */
/*       pv->floor -= FLOORS; */
/*       pv->room = room_view; */
/*     } else if (pv->place == 0 */
/*                && roomd (room_view, RIGHT) == pv->room */
/*                && room_view != pv->room) { */
/*       pv->place += PLACES; */
/*       pv->room = room_view; */
/*     } else if (pv->place == PLACES - 1 */
/*                && roomd (room_view, LEFT) == pv->room */
/*                && room_view != pv->room) { */
/*       pv->place -= PLACES; */
/*       pv->room = room_view; */
/*     } */
/*   } while (room != pv->room); */

/*   return pv; */
/* } */

struct pos *
pos2view (struct pos *p, struct pos *pv)
{
  *pv = *p;

  int room;

  do {
    room = pv->room;

    if (roomd (room_view, BELOW) == pv->room
        && room_view != pv->room) {
      pv->floor += FLOORS;
      pv->room = room_view;
    } else if (roomd (room_view, ABOVE) == pv->room
               && room_view != pv->room) {
      pv->floor -= FLOORS;
      pv->room = room_view;
    } else if (roomd (room_view, RIGHT) == pv->room
               && room_view != pv->room) {
      pv->place += PLACES;
      pv->room = room_view;
    } else if (roomd (room_view, LEFT) == pv->room
               && room_view != pv->room) {
      pv->place -= PLACES;
      pv->room = room_view;
    }
  } while (room != pv->room);

  return pv;
}

struct pos *
pos_gen (struct coord *c, struct pos *p, int dx, int dy)
{
  p->room = c->room;
  p->place = (c->x - dx) / PLACE_WIDTH;
  p->floor = (c->y - dy) / PLACE_HEIGHT;

  if (c->x < dx) p->place = -1;
  if (c->y < dy) p->floor = -1;

  return p;
}

inline struct pos *
pos (struct coord *c, struct pos *p)
{
  return pos_gen (c, p, 15, 3);
}

inline struct pos *
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

  if (np0.room < np1.room) return -1;
  else if (np0.room > np1.room) return 1;
  else if (np0.floor < np1.floor) return -1;
  else if (np0.floor > np1.floor) return 1;
  else if (np0.place < np1.place) return -1;
  else if (np0.place > np1.place) return 1;
  else return 0;
}

bool
peq (struct pos *p0, struct pos *p1)
{
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

struct coord *
nbitmap_coord (struct coord *c, struct coord *nc, int w, int h)
{
  *nc = *c;

  if (nc->room == room_view) return nc;

  struct coord tl = *nc;
  struct coord tr = *nc;
  struct coord bl = *nc;
  struct coord br = *nc;

  tr.x = tl.x + w - 1;
  bl.y = tl.y + h - 1;
  br.x = tl.x + w - 1;
  br.y = tl.y + h - 1;

  struct coord ntl; ncoord (&tl, &ntl);
  struct coord ntr; ncoord (&tr, &ntr);
  struct coord nbl; ncoord (&bl, &nbl);
  struct coord nbr; ncoord (&br, &nbr);

  if (ntl.room == room_view) *nc = ntl;
  else if (ntr.room == room_view) {
    *nc = ntr;
    nc->x = ntr.x - w + 1;
  } else if (nbl.room == room_view) {
    *nc = nbl;
    nc->y = nbl.y - h + 1;
  } else if (nbr.room == room_view) {
    *nc = nbr;
    nc->x = nbr.x - w + 1;
    nc->y = nbr.y - h + 1;
  }

  return nc;
}

struct coord *
nframe (struct frame *f, struct coord *c)
{
  if (f->id == &kid) {
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

      pos2view (&hang_pos, &hang_pos);
    } else *c = f->c;
  } else {
    struct dim d; dim (f, &d);
    nbitmap_coord (&f->c, c, d.w, d.h);
  }

  return c;
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
  pos2view (p, &pv);
  nf->b = b;
  nf->c.room = pv.room;
  nf->c.x = PLACE_WIDTH * pv.place + dx;
  nf->c.y = PLACE_HEIGHT * pv.floor + dy;
  return nf;
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
