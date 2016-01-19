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

#ifndef MININIM_KID_SWORD_ATTACK_H
#define MININIM_KID_SWORD_ATTACK_H

/* bitmaps */
#define KID_SWORD_ATTACK_00 "data/kid/sword-attack/00.png"
#define KID_SWORD_ATTACK_01 "data/kid/sword-attack/01.png"
#define KID_SWORD_ATTACK_02 "data/kid/sword-attack/02.png"
#define KID_SWORD_ATTACK_03 "data/kid/sword-attack/03.png"
#define KID_SWORD_ATTACK_04 "data/kid/sword-attack/04.png"
#define KID_SWORD_ATTACK_05 "data/kid/sword-attack/05.png"
#define KID_SWORD_ATTACK_06 "data/kid/sword-attack/06.png"
#define KID_SWORD_ATTACK_COUNTER_DEFENSE "data/kid/sword-attack/counter-defense.png"
#define KID_SWORD_ATTACK_DEFENDED "data/kid/sword-attack/defended.png"

#define KID_SWORD_ATTACK_FRAMESET_NMEMB 8
extern struct frameset kid_sword_attack_frameset[KID_SWORD_ATTACK_FRAMESET_NMEMB];

void load_kid_sword_attack (void);
void unload_kid_sword_attack (void);
void kid_sword_attack (struct anim *k);

extern ALLEGRO_BITMAP *kid_sword_attack_00, *kid_sword_attack_01,
  *kid_sword_attack_02, *kid_sword_attack_03, *kid_sword_attack_04,
  *kid_sword_attack_05, *kid_sword_attack_06, *kid_sword_attack_counter_defense,
  *kid_sword_attack_defended;

#endif	/* MININIM_KID_SWORD_ATTACK_H */
