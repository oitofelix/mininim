/*
  wall-dcpc.h -- wall dungeon ega, palace ega, dungeon vga module;

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

#ifndef MININIM_WALL_DCPC_H
#define MININIM_WALL_DCPC_H

/* dungeon cga */
#define DC_WALL_BASE "data/wall/dc-base.png"
#define DC_WALL_LEFT "data/wall/dc-left.png"

/* palace cga */
#define PC_WALL_BASE "data/wall/pc-base.png"
#define PC_WALL_LEFT "data/wall/pc-left.png"

void load_wall_dcpc (void);
void unload_wall_dcpc (void);
void draw_wall_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em);
void draw_wall_base_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em);
void draw_wall_left_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em);

#endif	/* MININIM_WALL_DCPC_H */
