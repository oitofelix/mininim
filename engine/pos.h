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

struct pos pos (struct coord c);
struct pos posf (struct coord c);
struct dim dim (struct anim a);

bool peq (struct pos p0, struct pos p1);
struct pos prel (struct pos, int floor, int place);

struct coord nanim (struct anim a);

struct coord coord_m (struct anim a);
struct coord coord_mt (struct anim a);
struct coord coord_mbo (struct anim a);
struct coord coord_ml (struct anim a);
struct coord coord_mr (struct anim a);
struct coord coord_mf (struct anim a);
struct coord coord_mba (struct anim a);

struct coord coord_tl (struct anim a);
struct coord coord_tr (struct anim a);
struct coord coord_tf (struct anim a);
struct coord coord_tb (struct anim a);

struct coord coord_bl (struct anim a);
struct coord coord_br (struct anim a);
struct coord coord_bf (struct anim a);
struct coord coord_bb (struct anim a);

double dist_coord (struct coord a, struct coord b);
int prandom_pos (struct pos pos, int i, int max);

#endif	/* FREEPOP_POSITION_H */
