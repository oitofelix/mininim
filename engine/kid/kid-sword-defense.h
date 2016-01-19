/*
  kid-sword-defense.h -- kid sword defense module;

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

#ifndef MININIM_KID_SWORD_DEFENSE_H
#define MININIM_KID_SWORD_DEFENSE_H

/* bitmaps */
#define KID_SWORD_DEFENSE_00 "data/kid/sword-defense/00.png"
#define KID_SWORD_DEFENSE_01 "data/kid/sword-defense/01.png"
#define KID_SWORD_DEFENSE_02 "data/kid/sword-defense/02.png"

#define KID_SWORD_DEFENSE_FRAMESET_NMEMB 3
extern struct frameset kid_sword_defense_frameset[KID_SWORD_DEFENSE_FRAMESET_NMEMB];

void load_kid_sword_defense (void);
void unload_kid_sword_defense (void);
void kid_sword_defense (struct anim *k);

extern ALLEGRO_BITMAP *kid_sword_defense_00, *kid_sword_defense_01;

#endif	/* MININIM_KID_SWORD_DEFENSE_H */
