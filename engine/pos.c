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

#include <math.h>
#include "kernel/random.h"
#include "level.h"
#include "kid.h"
#include "physics.h"
#include "pos.h"

int
roomd (int room, enum dir dir)
{
  switch (dir) {
  case LEFT: return level->link[room].l;
  case RIGHT: return level->link[room].r;
  case ABOVE: return level->link[room].a;
  case BELOW: return level->link[room].b;
  default: return room;
  }
}

struct coord
ncoord (struct coord c)
{
  if (c.x < 0) {
    c.x += PLACE_WIDTH * PLACES;
    c.room = roomd (c.room, LEFT);
  }

  if (c.x >= PLACE_WIDTH * PLACES) {
    c.x -= PLACE_WIDTH * PLACES;
    c.room = roomd (c.room, RIGHT);
  }

  if (c.y < 0) {
    c.y += PLACE_HEIGHT * FLOORS;
    c.room = roomd (c.room, ABOVE);
  }

  if (c.y >= PLACE_HEIGHT * FLOORS + 11) {
    c.y -= PLACE_HEIGHT * FLOORS;
    c.room = roomd (c.room, BELOW);
  }

  if (c.y < 8 && roomd (room_view, BELOW) == c.room) {
    c.y += PLACE_HEIGHT * FLOORS;
    c.room = room_view;
  }

  if (c.y > PLACE_HEIGHT * FLOORS + 3
             && roomd (room_view, ABOVE) == c.room) {
    c.y -= PLACE_HEIGHT * FLOORS;
    c.room = room_view;
  }

  return c;
}

struct pos
npos (struct pos p)
{
  if (p.place < 0) {
    p.place += PLACES;
    p.room = roomd (p.room, LEFT);
  }

  if (p.place >= PLACES) {
    p.place -= PLACES;
    p.room = roomd (p.room, RIGHT);
  }

  if (p.floor < 0) {
    p.floor += FLOORS;
    p.room = roomd (p.room, ABOVE);
  }

  if (p.floor >= FLOORS) {
    p.floor -= FLOORS;
    p.room = roomd (p.room, BELOW);
  }

  return p;
}

struct pos
pos (struct coord c)
{
  /* c = ncoord (c); */
  struct pos p;

  p.room = c.room;
  p.place = (c.x - 15) / PLACE_WIDTH;
  p.floor = (c.y - 3) / PLACE_HEIGHT;

  if (c.x < 15) p.place = -1;
  if (c.y < 3) p.floor = -1;

  return p;
}

struct pos
posf (struct coord c)
{
  /* c = ncoord (c); */
  struct pos p;

  p.room = c.room;
  p.place = c.x / PLACE_WIDTH;
  p.floor = (c.y - 3) / PLACE_HEIGHT;

  if (c.x < 0) p.place = -1;
  if (c.y < 3) p.floor = -1;

  return p;
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
    struct coord c = ncoord (coord_m (a));
    c.x -= d.w / 2;
    c.y -= d.h / 2;
    return c;
  } else {
    struct dim d = dim (a);

    struct coord ntl = ncoord (coord_tl (a));
    struct coord ntr = ncoord (coord_tr (a));
    struct coord nbl = ncoord (coord_bl (a));
    struct coord nbr = ncoord (coord_br (a));

    if (ntl.room == room_view) return ntl;
    else if (ntr.room == room_view) {
      a.c.x = ntr.x - d.w + 1;
      a.c.y = ntr.y;
      a.c.room = room_view;
    } else if (nbl.room == room_view) {
      a.c.x = nbl.x;
      a.c.y = nbl.y - d.h + 1;
      a.c.room = room_view;
    } else if (nbr.room == room_view) {
      a.c.x = nbr.x - d.w + 1;
      a.c.y = nbr.y - d.h + 1;
      a.c.room = room_view;
    } else return ncoord (a.c);

    return a.c;
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

int
prandom_pos (struct pos p, int i, int max)
{
  return
    prandom_uniq (p.room + p.floor * PLACES + p.place + i, max);
}
