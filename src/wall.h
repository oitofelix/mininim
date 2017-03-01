/*
  wall.h -- wall module;

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

#ifndef MININIM_WALL_H
#define MININIM_WALL_H

void draw_wall_part (ALLEGRO_BITMAP *bitmap, struct pos *p, char *part);
void draw_wall_part_width (ALLEGRO_BITMAP *bitmap, struct pos *p, char *part,
                           int width);
void draw_wall (ALLEGRO_BITMAP *bitmap, struct pos *p);

enum should_draw should_draw_face (struct pos *p, struct frame *f);

void draw_wall_left_cache (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_wall_base_cache (ALLEGRO_BITMAP *bitmap, struct pos *p);

void draw_wall_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                   enum em em, enum vm vm);

enum wall_correlation wall_correlation (struct pos *p);

struct coord *wall_base_coord (struct pos *p, struct coord *c);
struct coord *wall_left_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_WALL_H */
