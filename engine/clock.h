/*
  clock.h -- clock module;

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

#ifndef MININIM_CLOCK_H
#define MININIM_CLOCK_H

/* cga */
#define C_CLOCK_00 "data/clock/c-00.png"
#define C_CLOCK_01 "data/clock/c-01.png"
#define C_CLOCK_02 "data/clock/c-02.png"
#define C_CLOCK_03 "data/clock/c-03.png"
#define C_CLOCK_04 "data/clock/c-04.png"
#define C_CLOCK_05 "data/clock/c-05.png"
#define C_CLOCK_06 "data/clock/c-06.png"

/* ega */
#define E_CLOCK_00 "data/clock/e-00.png"
#define E_CLOCK_01 "data/clock/e-01.png"
#define E_CLOCK_02 "data/clock/e-02.png"
#define E_CLOCK_03 "data/clock/e-03.png"
#define E_CLOCK_04 "data/clock/e-04.png"
#define E_CLOCK_05 "data/clock/e-05.png"
#define E_CLOCK_06 "data/clock/e-06.png"

/* vga */
#define V_CLOCK_00 "data/clock/v-00.png"
#define V_CLOCK_01 "data/clock/v-01.png"
#define V_CLOCK_02 "data/clock/v-02.png"
#define V_CLOCK_03 "data/clock/v-03.png"
#define V_CLOCK_04 "data/clock/v-04.png"
#define V_CLOCK_05 "data/clock/v-05.png"
#define V_CLOCK_06 "data/clock/v-06.png"

/* palettable */
#define CLOCK_SAND_00 "data/clock/sand-00.png"
#define CLOCK_SAND_01 "data/clock/sand-01.png"
#define CLOCK_SAND_02 "data/clock/sand-02.png"

/* functions */
void load_clock (void);
void unload_clock (void);
ALLEGRO_COLOR c_clock_sand_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_clock_sand_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_clock_sand_palette (ALLEGRO_COLOR c);
void draw_clock (ALLEGRO_BITMAP *bitmap, int i, enum vm vm);
int get_clock_by_time_left (void);

/* variables */
extern ALLEGRO_BITMAP *princess_room_clock;

extern ALLEGRO_BITMAP *clock_00, *clock_01, *clock_02, *clock_03, *clock_04, *clock_05,
  *clock_06, *clock_sand_00, *clock_sand_01, *clock_sand_02;

#endif	/* MININIM_CLOCK_H */
