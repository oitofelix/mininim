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

#ifndef MININIM_POSITION_H
#define MININIM_POSITION_H

#include "prince.h"

extern bool coord_wa;

typedef struct coord *(*coord_f) (struct frame *, struct coord *);
typedef struct pos *(*pos_f) (struct coord *, struct pos *);
typedef struct coord *(*pos2coord_f) (struct pos *, struct coord *);

int roomd (int room, enum dir dir);
struct coord *ncoord (struct coord *c, struct coord *nc);
struct pos *npos (struct pos *p, struct pos *np);
struct coord *nframe (struct frame *f, struct coord *c);
struct pos *pos2room (struct pos *p, int room, struct pos *pv);
struct coord *coord2room (struct coord *c, int room, struct coord *cv);
struct coord *frame2room (struct frame *f, int room, struct coord *cv);
struct coord *rect2room (struct rect *r, int room, struct coord *cv);
bool coord4draw (struct coord *c, int room, struct coord *cv);
int pos_mod (struct pos *p);
int coord_mod (struct coord *c);
struct pos *pos_gen (struct coord *c, struct pos *p, int dx, int dy);
inline struct pos *pos (struct coord *c, struct pos *p);
inline struct pos *posf (struct coord *c, struct pos *p);
struct pos *prel (struct pos *p, struct pos *pr, int floor, int place);
int cpos (struct pos *p0, struct pos *p1);
int ccoord (struct coord *c0, struct coord *c1);
bool peq (struct pos *p0, struct pos *p1);
bool peqr (struct pos *p0, struct pos *p1, int floor, int place);
double dist_coord (struct coord *a, struct coord *b);
struct frame *place_frame (struct frame *f, struct frame *nf,
                           ALLEGRO_BITMAP *b, struct pos *p,
                           int dx, int dy);
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

#endif	/* MININIM_POSITION_H */
