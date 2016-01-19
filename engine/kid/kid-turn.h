/*
  kid-turn.h -- kid module;

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

#ifndef MININIM_KID_TURN_H
#define MININIM_KID_TURN_H

/* bitmaps */
#define KID_TURN_00 "data/kid/turn/00.png"
#define KID_TURN_01 "data/kid/turn/01.png"
#define KID_TURN_02 "data/kid/turn/02.png"
#define KID_TURN_03 "data/kid/turn/03.png"

#define KID_TURN_FRAMESET_NMEMB 4
extern struct frameset kid_turn_frameset[KID_TURN_FRAMESET_NMEMB];

void load_kid_turn (void);
void unload_kid_turn (void);
void kid_turn (struct anim *k);
bool is_kid_turn (struct frame *f);

extern ALLEGRO_BITMAP *kid_turn_00, *kid_turn_01, *kid_turn_02, *kid_turn_03;

#endif	/* MININIM_KID_TURN_H */
