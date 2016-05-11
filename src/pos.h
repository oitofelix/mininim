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

extern bool coord_wa;

/* types */
int *xroomd_ptr (struct level *l, int room, enum dir dir);
int *roomd_ptr (int room, enum dir dir);
int roomd (int room, enum dir dir);
int xroomd (struct level *l, int room, enum dir dir);
int roomd_n0 (int room, enum dir dir);
bool is_room_adjacent (int room0, int room1);
void link_room (int room0, int room1, enum dir dir);
int room_dist (int r0, int r1, int max);
int min_room_dist (struct room_dist room[], int *dmax);
bool coord_eq (struct coord *_c0, struct coord *_c1);
struct coord *ncoord (struct coord *c, struct coord *nc);
bool is_valid_pos (struct pos *p);
struct pos *xnpos (struct level *l, struct pos *p, struct pos *np);
struct pos *npos (struct pos *p, struct pos *np);
struct coord *nframe (struct frame *f, struct coord *c);
struct pos *pos2room (struct pos *p, int room, struct pos *pv);
struct coord *coord2room (struct coord *c, int room, struct coord *cv);
struct coord *frame2room (struct frame *f, int room, struct coord *cv);
struct coord *rect2room (struct rect *r, int room, struct coord *cv);
bool coord4draw (struct coord *c, int room, struct coord *cv);
int pos_mod (struct pos *p0, struct pos *p1);
int coord_mod (struct coord *c);
struct pos *pos_gen (struct coord *c, struct pos *p, int dx, int dy);
struct pos *pos (struct coord *c, struct pos *p);
struct pos *posf (struct coord *c, struct pos *p);
struct pos *prel (struct pos *p, struct pos *pr, int floor, int place);
int cpos (struct pos *p0, struct pos *p1);
int ccoord (struct coord *c0, struct coord *c1);
bool peq (struct pos *p0, struct pos *p1);
bool peqr (struct pos *p0, struct pos *p1, int floor, int place);
struct pos *reflect_pos_h (struct pos *p0, struct pos *p1);
struct pos *reflect_pos_v (struct pos *p0, struct pos *p1);
struct pos *random_pos (struct pos *p);
double dist_coord (struct coord *a, struct coord *b);
struct frame *place_frame (struct frame *f, struct frame *nf,
                           ALLEGRO_BITMAP *b, struct pos *p,
                           int dx, int dy);
coord_f opposite_cf (coord_f cf);
enum dir opposite_dir (enum dir dir);
enum dir perpendicular_dir (enum dir dir, int n);

struct coord *con_m (struct pos *p, struct coord *c);

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
struct coord *place_on_the_ground (struct frame *f, struct coord *c);
struct coord *place_at_distance (struct frame *f0, coord_f cf0,
                                 struct frame *f1, coord_f cf1,
                                 int d, enum dir dir, struct coord *c);
struct coord *
place_at_pos (struct frame *f, coord_f cf, struct pos *p, struct coord *c);

#define survey(coord_f,pos_f,f,c,p,np) (con (npos ((pos_f) ((coord_f) ((f), (c)), (p)), (np))))

#endif	/* MININIM_POSITION_H */
