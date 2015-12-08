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

typedef struct coord *(*coord_f) (struct frame *, struct coord *);
typedef struct pos *(*pos_f) (struct coord *, struct pos *);

int roomd (int room, enum dir dir);
struct coord *ncoord (struct coord *c, struct coord *nc);
struct pos *npos (struct pos *p, struct pos *np);
struct pos *pos2view (struct pos *p, struct pos *pv);
struct pos *pos_gen (struct coord *c, struct pos *p, int dx, int dy);
inline struct pos *pos (struct coord *c, struct pos *p);
inline struct pos *posf (struct coord *c, struct pos *p);
struct pos *prel (struct pos *p, struct pos *pr, int floor, int place);
int cpos (struct pos *p0, struct pos *p1);
bool peq (struct pos *p0, struct pos *p1);
bool peqr (struct pos *p0, struct pos *p1, int floor, int place);
struct coord *nbitmap_coord (struct coord *c, struct coord *nc, int w, int h);
struct coord *nframe (struct frame *f, struct coord *c);
double dist_coord (struct coord *a, struct coord *b);
struct frame *
place_frame (struct frame *f, struct frame *nf, ALLEGRO_BITMAP *b,
             struct pos *p, int dx, int dy);
struct dim *dim (struct frame *f, struct dim *d);
struct coord *_m (struct frame *f, struct coord *c);
struct coord *_mt (struct frame *f, struct coord *c);
struct coord *_mbo (struct frame *f, struct coord *c);
struct coord *_ml (struct frame *f, struct coord *c);
struct coord *_mr (struct frame *f, struct coord *c);
struct coord *_mf (struct frame *f, struct coord *c);
struct coord *_mba (struct frame *f, struct coord *c);
struct coord *_tl (struct frame *f, struct coord *c);
struct coord *_tr (struct frame *f, struct coord *c);
struct coord *_tf (struct frame *f, struct coord *c);
struct coord *_tb (struct frame *f, struct coord *c);
struct coord *_bl (struct frame *f, struct coord *c);
struct coord *_br (struct frame *f, struct coord *c);
struct coord *_bf (struct frame *f, struct coord *c);
struct coord *_bb (struct frame *f, struct coord *c);

struct con *
surveyo (coord_f cf, int dx, int dy, pos_f pf, struct frame *f,
         struct coord *c, struct pos *p, struct pos *np);

#define survey(coord_f,pos_f,f,c,p,np) (con (npos ((pos_f) ((coord_f) ((f), (c)), (p)), (np))))

/* #define pmt(f) (pos (mt (f))) */
/* #define pmbo(f) (pos (mbo (f))) */
/* #define pml(f) (pos (ml (f))) */
/* #define pmr(f) (pos (mr (f))) */
/* #define pmf(f) (pos (mf (f))) */
/* #define pmba(f) (pos (mba (f))) */
/* #define ptl(f) (pos (tl (f))) */
/* #define ptr(f) (pos (tr (f))) */
/* #define ptf(f) (pos (tf (f))) */
/* #define ptb(f) (pos (tb (f))) */
/* #define pbl(f) (pos (bl (f))) */
/* #define pbr(f) (pos (br (f))) */
/* #define pbf(f) (pos (bf (f))) */
/* #define pbb(f) (pos (bb (f))) */

/* #define cm(f) (con (pm (f))->fg) */
/* #define cmt(f) (con (pmt (f))->fg) */
/* #define cmbo(f) (con (pmbo (f))->fg) */
/* #define cml(f) (con (pml (f))->fg) */
/* #define cmr(f) (con (pmr (f))->fg) */
/* #define cmf(f) (con (pmf (f))->fg) */
/* #define cmba(f) (con (pmba (f))->fg) */
/* #define ctl(f) (con (ptl (f))->fg) */
/* #define ctr(f) (con (ptr (f))->fg) */
/* #define ctf(f) (con (ptf (f))->fg) */
/* #define ctb(f) (con (ptb (f))->fg) */
/* #define cbl(f) (con (pbl (f))->fg) */
/* #define cbr(f) (con (pbr (f))->fg) */
/* #define cbf(f) (con (pbf (f))->fg) */
/* #define cbb(f) (con (pbb (f))->fg) */

/* #define fpm(f) (posf (m (f))) */
/* #define fpmt(f) (posf (mt (f))) */
/* #define fpmbo(f) (posf (mbo (f))) */
/* #define fpml(f) (posf (ml (f))) */
/* #define fpmr(f) (posf (mr (f))) */
/* #define fpmf(f) (posf (mf (f))) */
/* #define fpmba(f) (posf (mba (f))) */
/* #define fptl(f) (posf (tl (f))) */
/* #define fptr(f) (posf (tr (f))) */
/* #define fptf(f) (posf (tf (f))) */
/* #define fptb(f) (posf (tb (f))) */
/* #define fpbl(f) (posf (bl (f))) */
/* #define fpbr(f) (posf (br (f))) */
/* #define fpbf(f) (posf (bf (f))) */
/* #define fpbb(f) (posf (bb (f))) */

/* #define fcm(f) (con (fpm (f))->fg) */
/* #define fcmt(f) (con (fpmt (f))->fg) */
/* #define fcmbo(f) (con (fpmbo (f))->fg) */
/* #define fcml(f) (con (fpml (f))->fg) */
/* #define fcmr(f) (con (fpmr (f))->fg) */
/* #define fcmf(f) (con (fpmf (f))->fg) */
/* #define fcmba(f) (con (fpmba (f))->fg) */
/* #define fctl(f) (con (fptl (f))->fg) */
/* #define fctr(f) (con (fptr (f))->fg) */
/* #define fctf(f) (con (fptf (f))->fg) */
/* #define fctb(f) (con (fptb (f))->fg) */
/* #define fcbl(f) (con (fpbl (f))->fg) */
/* #define fcbr(f) (con (fpbr (f))->fg) */
/* #define fcbf(f) (con (fpbf (f))->fg) */
/* #define fcbb(f) (con (fpbb (f))->fg) */

#endif	/* FREEPOP_POSITION_H */
