/*
  door.h -- door module;

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

#ifndef MININIM_DOOR_H
#define MININIM_DOOR_H

/* variables */
extern struct door *door;
extern size_t door_nmemb;

/* functions */
struct door *init_door (struct pos *p, struct door *d);
void register_door (struct pos *p);
int compare_doors (const void *d0, const void *d1);
struct door *copy_door (struct door *to,
                        struct door *from);
struct door *door_at_pos (struct pos *p);
void remove_door (struct door *d);
void open_door (struct level *l, int e, uint64_t priority, bool stay_open);
void close_door (struct level *l, int e, uint64_t priority);
void compute_doors (void);
int door_grid_tip_y (struct pos *p);
void draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f);

bool should_draw_door_grid (struct pos *p, struct frame *f);
struct coord *door_grid_tip_coord (struct pos *p, struct coord *c, int i);

#endif	/* MININIM_DOOR_H */
