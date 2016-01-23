/*
  kid-sword-walkb.h -- kid sword walk backward module;

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

#ifndef MININIM_KID_SWORD_WALKB_H
#define MININIM_KID_SWORD_WALKB_H

/* bitmaps */
#define KID_SWORD_WALKB_00 "data/kid/sword-walkb/00.png"
#define KID_SWORD_WALKB_01 "data/kid/sword-walkb/01.png"

#define KID_SWORD_WALKB_FRAMESET_NMEMB 2
extern struct frameset kid_sword_walkb_frameset[KID_SWORD_WALKB_FRAMESET_NMEMB];

void load_kid_sword_walkb (void);
void unload_kid_sword_walkb (void);
void kid_sword_walkb (struct anim *k);

ALLEGRO_BITMAP *kid_sword_walkb_00, *kid_sword_walkb_01;

#endif	/* MININIM_KID_SWORD_WALKB_H */
