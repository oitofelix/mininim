/*
  wall.h -- wall module;

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

#ifndef MININIM_WALL_H
#define MININIM_WALL_H

/* dungeon cga */
#define DC_WALL_FACE "data/wall/dc-face.png"
#define DC_WALL_FACE_TOP "data/wall/dc-face-top.png"

/* palace cga */
#define PC_WALL_FACE "data/wall/pc-face.png"
#define PC_WALL_FACE_TOP "data/wall/pc-face-top.png"

/* dungeon ega */
#define DE_WALL_FACE "data/wall/de-face.png"
#define DE_WALL_FACE_TOP "data/wall/de-face-top.png"

/* palace ega */
#define PE_WALL_FACE "data/wall/pe-face.png"
#define PE_WALL_FACE_TOP "data/wall/pe-face-top.png"

/* dungeon vga */
#define DV_WALL_FACE "data/wall/dv-face.png"
#define DV_WALL_FACE_TOP "data/wall/dv-face-top.png"

/* palace vga */
#define PV_WALL_FACE "data/wall/pv-face.png"
#define PV_WALL_FACE_TOP "data/wall/pv-face-top.png"

void load_wall (void);
void unload_wall (void);
void draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em, enum vm vm);
void draw_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_wall_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_wall_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    enum em em, enum vm vm);
void draw_wall_face (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em, enum vm vm);
void draw_wall_face_top (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         enum em em, enum vm vm);

void draw_wall_left_cache (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_wall_base_cache (ALLEGRO_BITMAP *bitmap, struct pos *p);

enum wall_correlation wall_correlation (struct pos *p);
struct coord *wall_base_coord (struct pos *p, struct coord *c);
struct coord *wall_coord (struct pos *p, struct coord *c);
struct coord *wall_face_coord (struct pos *p, struct coord *c);
struct coord *dc_wall_face_top_coord (struct pos *p, struct coord *c);
struct coord *d_wall_face_top_coord (struct pos *p, struct coord *c);
struct coord *p_wall_face_top_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_WALL_H */
