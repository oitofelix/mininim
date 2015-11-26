/*
  sword.h -- sword module;

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

#ifndef FREEPOP_SWORD_H
#define FREEPOP_SWORD_H

/* bitmap resources */
#define NORMAL_SWORD "dat/prince/sword/in the floor/normal.png"
#define SHINY_SWORD "dat/prince/sword/in the floor/bright.png"

void load_sword (void);
void unload_sword (void);
void draw_sword (ALLEGRO_BITMAP *bitmap, struct pos p, int i);
bool is_sword (struct pos p);

#endif	/* FREEPOP_SWORD_H */
