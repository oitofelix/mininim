/*
  potion.h -- potion module;

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

#ifndef FREEPOP_POTION_H
#define FREEPOP_POTION_H

/* bitmap resources */
#define VDUNGEON_SMALL_POTION "dat/prince/potions/base/small dungeon.png"
#define VDUNGEON_BIG_POTION "dat/prince/potions/base/big dungeon.png"
#define BUBBLE_01 "dat/prince/potions/bubble animation/frame01.png"
#define BUBBLE_02 "dat/prince/potions/bubble animation/frame02.png"
#define BUBBLE_03 "dat/prince/potions/bubble animation/frame03.png"
#define BUBBLE_04 "dat/prince/potions/bubble animation/frame04.png"
#define BUBBLE_05 "dat/prince/potions/bubble animation/frame05.png"
#define BUBBLE_06 "dat/prince/potions/bubble animation/frame06.png"
#define BUBBLE_07 "dat/prince/potions/bubble animation/frame07.png"

void load_potion (void);
void unload_potion (void);
void draw_potion (ALLEGRO_BITMAP *bitmap, struct pos p, int i);
bool is_potion (struct pos p);

#endif	/* FREEPOP_POTION_H */
