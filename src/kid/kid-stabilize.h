/*
  kid-stabilize.h -- kid stabilize module;

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

#ifndef MININIM_KID_STABILIZE_H
#define MININIM_KID_STABILIZE_H

#define KID_STABILIZE_00 "data/kid/stabilize/00.png"
#define KID_STABILIZE_01 "data/kid/stabilize/01.png"
#define KID_STABILIZE_02 "data/kid/stabilize/02.png"
#define KID_STABILIZE_03 "data/kid/stabilize/03.png"

#define KID_STABILIZE_FRAMESET_NMEMB 4
extern struct frameset kid_stabilize_frameset[KID_STABILIZE_FRAMESET_NMEMB];

void load_kid_stabilize (void);
void unload_kid_stabilize (void);
void kid_stabilize (struct anim *k);
void kid_stabilize_collision (struct anim *k);
void kid_stabilize_back_collision (struct anim *k);
bool is_kid_stabilize (struct frame *f);

extern ALLEGRO_BITMAP *kid_stabilize_00, *kid_stabilize_01,
  *kid_stabilize_02, *kid_stabilize_03;

#endif	/* MININIM_KID_STABILIZE_H */
