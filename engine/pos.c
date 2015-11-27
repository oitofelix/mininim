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
#include "kid.h"
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

struct coord
ncoord (struct coord c)
{
  int room;

  do {
    room = c.room;

    if (c.y < 0) {
      c.y += PLACE_HEIGHT * FLOORS;
      c.room = roomd (c.room, ABOVE);
    } else if (c.y >= PLACE_HEIGHT * FLOORS + 11) {
      c.y -= PLACE_HEIGHT * FLOORS;
      c.room = roomd (c.room, BELOW);
    } else if (c.x < 0) {
      c.x += PLACE_WIDTH * PLACES;
      c.room = roomd (c.room, LEFT);
    } else if (c.x >= PLACE_WIDTH * PLACES) {
      c.x -= PLACE_WIDTH * PLACES;
      c.room = roomd (c.room, RIGHT);
    }
  } while (room != c.room);

  do {
    room = c.room;

    if (c.y < 11 &&
        roomd (room_view, BELOW) == c.room
        && room_view != c.room) {
      c.y += PLACE_HEIGHT * FLOORS;
      c.room = room_view;
    } else if (c.y >= PLACE_HEIGHT * FLOORS
               && roomd (room_view, ABOVE) == c.room
               && room_view != c.room) {
      c.y -= PLACE_HEIGHT * FLOORS;
      c.room = room_view;
    }
  } while (room != c.room);

  return c;
}

struct pos
npos (struct pos p)
{
  int room;

  do {
    room = p.room;

    if (p.floor < 0) {
      p.floor += FLOORS;
      p.room = roomd (p.room, ABOVE);
    } else if (p.floor >= FLOORS) {
      p.floor -= FLOORS;
      p.room = roomd (p.room, BELOW);
    } else if (p.place < 0) {
      p.place += PLACES;
      p.room = roomd (p.room, LEFT);
    } else if (p.place >= PLACES) {
      p.place -= PLACES;
      p.room = roomd (p.room, RIGHT);
    }
  } while (room != p.room);

  return p;
}

struct pos
pos_gen (struct coord c, int dx, int dy)
{
  struct pos p;

  p.room = c.room;
  p.place = (c.x - dx) / PLACE_WIDTH;
  p.floor = (c.y - dy) / PLACE_HEIGHT;

  if (c.x < dx) p.place = -1;
  if (c.y < dy) p.floor = -1;

  return p;
}

struct pos
pos (struct coord c)
{
  return pos_gen (c, 15, 3);
}

struct pos
posf (struct coord c)
{
  return pos_gen (c, 0, 3);
}

struct dim
dim (struct anim a)
{
  struct dim d;
  d.w = al_get_bitmap_width (a.frame);
  d.h = al_get_bitmap_height (a.frame);
  d.x = a.c.x;
  d.y = a.c.y;
  d.fx = (a.dir == LEFT) ? d.x : d.x + d.w - 1;
  d.bx = (a.dir == LEFT) ? d.x + d.w - 1 : d.x;
  return d;
}

struct pos
prel (struct pos p, int floor, int place)
{
  p.floor += floor;
  p.place += place;
  return p;
}

bool
peq (struct pos p0, struct pos p1)
{
  p0 = npos (p0);
  p1 = npos (p1);

  return p0.room == p1.room && p0.floor == p1.floor
    && p0.place == p1.place;
}

struct coord
nanim (struct anim a)
{
  if (a.id == &kid) {
    struct dim d = dim (a);

    struct coord m = coord_m (a);
    struct coord ml = m;
    struct coord mr = m;

    ml.x += -3;
    mr.x += +3;

    struct coord nm = ncoord (m);
    struct coord nml = ncoord (ml);
    struct coord nmr = ncoord (mr);

    if (nml.room == nmr.room) {
      struct coord c = nm;
      c.x -= d.w / 2;
      c.y -= d.h / 2;
      return c;
    } else return a.c;
  } else {
    struct dim d = dim (a);
    return nbitmap_coord (a.c, d.w, d.h);
  }
}


/* <->
   000
   010
   000 */

struct coord
coord_m (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w / 2;
  c.y = d.y + d.h / 2;
  return c;
}

/* <->
   010
   000
   000 */

struct coord
coord_mt (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w / 2;
  c.y = d.y;
  return c;
}

/* <->
   000
   000
   010 */

struct coord
coord_mbo (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w / 2;
  c.y = d.y + d.h - 1;
  return c;
}

/* <->
   000
   100
   000 */

struct coord
coord_ml (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x;
  c.y = d.y + d.h / 2;
  return c;
}

/* <->
   000
   001
   000 */

struct coord
coord_mr (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w - 1;
  c.y = d.y + d.h / 2;
  return c;
}

/* <-- -->
   000 000
   100 001
   000 000 */

struct coord
coord_mf (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.fx;
  c.y = d.y + d.h / 2;
  return c;
}

/* <-- -->
   000 000
   001 100
   000 000 */

struct coord
coord_mba (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.bx;
  c.y = d.y + d.h / 2;
  return c;
}

/* <->
   100
   000
   000 */

struct coord
coord_tl (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x;
  c.y = d.y;
  return c;
}

/* <->
   001
   000
   000 */

struct coord
coord_tr (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w - 1;
  c.y = d.y;
  return c;
}

/* <-- -->
   100 001
   000 000
   000 000 */

struct coord
coord_tf (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.fx;
  c.y = d.y;
  return c;
}

/* <-- -->
   001 100
   000 000
   000 000 */

struct coord
coord_tb (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.bx;
  c.y = d.y;
  return c;
}

/* <->
   000
   000
   100 */

struct coord
coord_bl (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x;
  c.y = d.y + d.h - 1;
  return c;
}

/* <->
   000
   000
   001 */

struct coord
coord_br (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.x + d.w - 1;
  c.y = d.y + d.h - 1;
  return c;
}

/* <-- -->
   000 000
   000 000
   100 001 */

struct coord
coord_bf (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.fx;
  c.y = d.y + d.h -1;
  return c;
}

/* <-- -->
   000 000
   000 000
   001 100 */

struct coord
coord_bb (struct anim a)
{
  struct dim d = dim (a);
  struct coord c;
  c.room = a.c.room;
  c.x = d.bx;
  c.y = d.y + d.h -1;
  return c;
}

double
dist_coord (struct coord a, struct coord b)
{
  int dx = a.x - b.x;
  int dy = a.y - b.y;
  return sqrt (dx * dx + dy * dy);
}

struct survey
survey (struct anim a, struct pos (*pf) (struct coord c))
{
  struct survey s;

  s.m = coord_m (a);
  s.mt = coord_mt (a);
  s.mbo = coord_mbo (a);
  s.ml = coord_ml (a);
  s.mr = coord_mr (a);
  s.mf = coord_mf (a);
  s.mba = coord_mba (a);

  s.tl = coord_tl (a);
  s.tr = coord_tr (a);
  s.tf = coord_tf (a);
  s.tb = coord_tb (a);

  s.bl = coord_bl (a);
  s.br = coord_br (a);
  s.bf = coord_bf (a);
  s.bb = coord_bb (a);

  s.pm = pf (s.m);
  s.pmt = pf (s.mt);
  s.pmbo = pf (s.mbo);
  s.pml = pf (s.ml);
  s.pmr = pf (s.mr);
  s.pmf = pf (s.mf);
  s.pmba = pf (s.mba);

  s.ptl = pf (s.tl);
  s.ptr = pf (s.tr);
  s.ptf = pf (s.tf);
  s.ptb = pf (s.tb);

  s.pbl = pf (s.bl);
  s.pbr = pf (s.br);
  s.pbf = pf (s.bf);
  s.pbb = pf (s.bb);

  s.cm = con (s.pm)->fg;
  s.cmt = con (s.pmt)->fg;
  s.cmbo = con (s.pmbo)->fg;
  s.cml = con (s.pml)->fg;
  s.cmr = con (s.pmr)->fg;
  s.cmf = con (s.pmf)->fg;
  s.cmba = con (s.pmba)->fg;

  s.ctl = con (s.ptl)->fg;
  s.ctr = con (s.ptr)->fg;
  s.ctf = con (s.ptf)->fg;
  s.ctb = con (s.ptb)->fg;

  s.cbl = con (s.pbl)->fg;
  s.cbr = con (s.pbr)->fg;
  s.cbf = con (s.pbf)->fg;
  s.cbb = con (s.pbb)->fg;

  return s;
}
