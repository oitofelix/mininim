/*
  fire.h -- fire module;

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

#ifndef MININIM_FIRE_H
#define MININIM_FIRE_H

/* vga */
#define V_FIRE_00 "data/fire/v-00.png"
#define V_FIRE_01 "data/fire/v-01.png"
#define V_FIRE_02 "data/fire/v-02.png"
#define V_FIRE_03 "data/fire/v-03.png"
#define V_FIRE_04 "data/fire/v-04.png"
#define V_FIRE_05 "data/fire/v-05.png"
#define V_FIRE_06 "data/fire/v-06.png"
#define V_FIRE_07 "data/fire/v-07.png"
#define V_FIRE_08 "data/fire/v-08.png"

void load_fire (void);
void unload_fire (void);
void draw_fire (ALLEGRO_BITMAP* bitmap, struct pos *p, enum vm vm);
void draw_princess_room_fire (ALLEGRO_BITMAP *bitmap, enum vm vm);
struct coord *fire_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_FIRE_H */
