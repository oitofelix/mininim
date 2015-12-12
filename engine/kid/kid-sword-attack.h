/*
  kid-sword-attack.h -- kid sword attack module;

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

#ifndef FREEPOP_KID_SWORD_ATTACK_H
#define FREEPOP_KID_SWORD_ATTACK_H

/* bitmaps */
#define KID_SWORD_ATTACK_01 "dat/kid/sword attacking/frame01.png"
#define KID_SWORD_ATTACK_02 "dat/kid/sword attacking/frame02.png"
#define KID_SWORD_ATTACK_03 "dat/kid/sword attacking/frame03.png"
#define KID_SWORD_ATTACK_04 "dat/kid/sword attacking/frame04.png"
#define KID_SWORD_ATTACK_05 "dat/kid/sword attacking/frame05.png"
#define KID_SWORD_ATTACK_06 "dat/kid/sword attacking/frame06.png"
#define KID_SWORD_ATTACK_07 "dat/kid/sword attacking/frame07.png"

#define KID_SWORD_ATTACK_FRAMESET_NMEMB 7
extern struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];

void load_kid_sword_attack (void);
void unload_kid_sword_attack (void);
void kid_sword_attack (struct anim *kid);

extern ALLEGRO_BITMAP *kid_sword_attack_01, *kid_sword_attack_02,
  *kid_sword_attack_03, *kid_sword_attack_04, *kid_sword_attack_05,
  *kid_sword_attack_06, *kid_sword_attack_07;

#endif	/* FREEPOP_KID_SWORD_ATTACK_H */
