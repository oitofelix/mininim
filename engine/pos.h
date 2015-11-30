/*
  pos.h -- position module;

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

#ifndef FREEPOP_POSITION_H
#define FREEPOP_POSITION_H

#include "prince.h"

int roomd (int room, enum dir dir);

struct coord ncoord (struct coord c);
struct pos npos (struct pos p);
struct pos pos2view (struct pos p);

struct pos pos_gen (struct coord c, int dx, int dy);
struct pos pos (struct coord c);
struct pos posf (struct coord c);
struct dim dim (struct anim a);

bool peq (struct pos p0, struct pos p1);
struct pos prel (struct pos, int floor, int place);

struct coord nanim (struct anim a);

struct coord m (struct anim a);
struct coord mt (struct anim a);
struct coord mbo (struct anim a);
struct coord ml (struct anim a);
struct coord mr (struct anim a);
struct coord mf (struct anim a);
struct coord mba (struct anim a);

struct coord tl (struct anim a);
struct coord tr (struct anim a);
struct coord tf (struct anim a);
struct coord tb (struct anim a);

struct coord bl (struct anim a);
struct coord br (struct anim a);
struct coord bf (struct anim a);
struct coord bb (struct anim a);

double dist_coord (struct coord a, struct coord b);

#define pm(a) (pos (m (a)))
#define pmt(a) (pos (mt (a)))
#define pmbo(a) (pos (mbo (a)))
#define pml(a) (pos (ml (a)))
#define pmr(a) (pos (mr (a)))
#define pmf(a) (pos (mf (a)))
#define pmba(a) (pos (mba (a)))
#define ptl(a) (pos (tl (a)))
#define ptr(a) (pos (tr (a)))
#define ptf(a) (pos (tf (a)))
#define ptb(a) (pos (tb (a)))
#define pbl(a) (pos (bl (a)))
#define pbr(a) (pos (br (a)))
#define pbf(a) (pos (bf (a)))
#define pbb(a) (pos (bb (a)))

#define cm(a) (con (pm (a))->fg)
#define cmt(a) (con (pmt (a))->fg)
#define cmbo(a) (con (pmbo (a))->fg)
#define cml(a) (con (pml (a))->fg)
#define cmr(a) (con (pmr (a))->fg)
#define cmf(a) (con (pmf (a))->fg)
#define cmba(a) (con (pmba (a))->fg)
#define ctl(a) (con (ptl (a))->fg)
#define ctr(a) (con (ptr (a))->fg)
#define ctf(a) (con (ptf (a))->fg)
#define ctb(a) (con (ptb (a))->fg)
#define cbl(a) (con (pbl (a))->fg)
#define cbr(a) (con (pbr (a))->fg)
#define cbf(a) (con (pbf (a))->fg)
#define cbb(a) (con (pbb (a))->fg)

#define fpm(a) (posf (m (a)))
#define fpmt(a) (posf (mt (a)))
#define fpmbo(a) (posf (mbo (a)))
#define fpml(a) (posf (ml (a)))
#define fpmr(a) (posf (mr (a)))
#define fpmf(a) (posf (mf (a)))
#define fpmba(a) (posf (mba (a)))
#define fptl(a) (posf (tl (a)))
#define fptr(a) (posf (tr (a)))
#define fptf(a) (posf (tf (a)))
#define fptb(a) (posf (tb (a)))
#define fpbl(a) (posf (bl (a)))
#define fpbr(a) (posf (br (a)))
#define fpbf(a) (posf (bf (a)))
#define fpbb(a) (posf (bb (a)))

#define fcm(a) (con (fpm (a))->fg)
#define fcmt(a) (con (fpmt (a))->fg)
#define fcmbo(a) (con (fpmbo (a))->fg)
#define fcml(a) (con (fpml (a))->fg)
#define fcmr(a) (con (fpmr (a))->fg)
#define fcmf(a) (con (fpmf (a))->fg)
#define fcmba(a) (con (fpmba (a))->fg)
#define fctl(a) (con (fptl (a))->fg)
#define fctr(a) (con (fptr (a))->fg)
#define fctf(a) (con (fptf (a))->fg)
#define fctb(a) (con (fptb (a))->fg)
#define fcbl(a) (con (fpbl (a))->fg)
#define fcbr(a) (con (fpbr (a))->fg)
#define fcbf(a) (con (fpbf (a))->fg)
#define fcbb(a) (con (fpbb (a))->fg)

#endif	/* FREEPOP_POSITION_H */
