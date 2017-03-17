/*
  sword.h -- sword module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

#ifndef MININIM_SWORD_H
#define MININIM_SWORD_H

/* cga */
#define C_NORMAL_SWORD "data/sword/c-normal.png"
#define C_SHINY_SWORD "data/sword/c-shiny.png"

/* ega */
#define E_NORMAL_SWORD "data/sword/e-normal.png"
#define E_SHINY_SWORD "data/sword/e-shiny.png"

/* vga */
#define V_NORMAL_SWORD "data/sword/v-normal.png"
#define V_SHINY_SWORD "data/sword/v-shiny.png"

void load_sword (void);
void unload_sword (void);
ALLEGRO_BITMAP *sword_bitmap (void);
ALLEGRO_BITMAP *shiny_sword_bitmap (void);
void draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p, bool start_pos);
bool is_sword_frame (struct frame *f);

#endif	/* MININIM_SWORD_H */
