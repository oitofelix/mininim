/*
  wall-pv.h -- wall palace vga module;

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

#ifndef MININIM_WALL_PV_H
#define MININIM_WALL_PV_H

/* palace vga */
#define PV_WALL_MARK_00 "data/wall/pv-mark-00.png"
#define PV_WALL_MARK_01 "data/wall/pv-mark-01.png"
#define PV_WALL_MARK_02 "data/wall/pv-mark-02.png"
#define PV_WALL_MARK_03 "data/wall/pv-mark-03.png"
#define PV_WALL_MARK_04 "data/wall/pv-mark-04.png"
#define PV_WALL_MARK_05 "data/wall/pv-mark-05.png"
#define PV_WALL_MARK_06 "data/wall/pv-mark-06.png"
#define PV_WALL_MARK_07 "data/wall/pv-mark-07.png"
#define PV_WALL_MARK_08 "data/wall/pv-mark-08.png"
#define PV_WALL_MARK_09 "data/wall/pv-mark-09.png"
#define PV_WALL_MARK_10 "data/wall/pv-mark-10.png"
#define PV_WALL_MARK_11 "data/wall/pv-mark-11.png"
#define PV_WALL_MARK_12 "data/wall/pv-mark-12.png"
#define PV_WALL_MARK_13 "data/wall/pv-mark-13.png"
#define PV_WALL_MARK_14 "data/wall/pv-mark-14.png"

void load_wall_pv (void);
void unload_wall_pv (void);
void draw_wall_pv (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_wall_base_pv (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_wall_left_pv (ALLEGRO_BITMAP *bitmap, struct pos *p);
ALLEGRO_COLOR compute_wall_color (struct pos *p, int row, int col);
void generate_wall_colors_for_room (int room, ALLEGRO_COLOR wall_color[3][4][11]);
void generate_wall_colors (void);

#endif	/* MININIM_WALL_PV_H */
