/*
  closer-floor.h -- closer floor module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#ifndef MININIM_CLOSER_FLOOR_H
#define MININIM_CLOSER_FLOOR_H

/* variables */
extern struct closer_floor *closer_floor;
extern size_t closer_floor_nmemb;

/* functions */
struct closer_floor *init_closer_floor (struct pos *p,
                                        struct closer_floor *c);
void register_closer_floor (struct pos *p);
int compare_closer_floors (const void *c0, const void *c1);
struct closer_floor *copy_closer_floor (struct closer_floor *to,
                                        struct closer_floor *from);
struct closer_floor * closer_floor_at_pos (struct pos *p);
struct closer_floor *closer_floor_by_event (struct pos *p, int event,
                                            int dir);
void remove_closer_floor (struct closer_floor *c);
void press_closer_floor (struct pos *p, struct actor *a);
void break_closer_floor (struct pos *p);
void unpress_closer_floors (void);
void register_changed_closer_floors (void);
void compute_closer_floors (void);
void draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);

#endif	/* MININIM_CLOSER_FLOOR_H */
