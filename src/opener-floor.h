/*
  opener-floor.h -- opener floor module;

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

#ifndef MININIM_OPENER_FLOOR_H
#define MININIM_OPENER_FLOOR_H

/* variables */
extern struct opener_floor *opener_floor;
extern size_t opener_floor_nmemb;

/* functions */
struct opener_floor *init_opener_floor (struct pos *p,
                                        struct opener_floor *o);
void register_opener_floor (struct pos *p);
int compare_opener_floors (const void *o0, const void *o1);
struct opener_floor *copy_opener_floor (struct opener_floor *to,
                                        struct opener_floor *from);
struct opener_floor *opener_floor_at_pos (struct pos *p);
struct opener_floor *opener_floor_by_event (struct pos *p, int event,
                                            int dir);
void remove_opener_floor (struct opener_floor *o);
void press_opener_floor (struct pos *p, struct actor *a);
void break_opener_floor (struct pos *p);
void unpress_opener_floors (void);
void register_changed_opener_floors (void);
void compute_opener_floors (void);
void draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);

#endif	/* MININIM_OPENER_FLOOR_H */
