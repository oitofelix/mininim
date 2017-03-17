/*
  loose-floor.h -- loose floor module;

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

#ifndef MININIM_LOOSE_FLOOR_H
#define MININIM_LOOSE_FLOOR_H

/* variables */
extern struct loose_floor *loose_floor;
extern size_t loose_floor_nmemb;

/* functions */
struct loose_floor *init_loose_floor (struct pos *p, struct loose_floor *l);
void register_loose_floor (struct pos *p);
void sort_loose_floors (void);
int compare_loose_floors (const void *l0, const void *l1);
struct loose_floor *copy_loose_floor (struct loose_floor *to,
                                      struct loose_floor *from);
struct loose_floor *loose_floor_at_pos (struct pos *p);
struct loose_floor *falling_loose_floor_at_pos (struct pos *p);
bool should_remove_loose_floor (struct loose_floor *l);
void remove_loose_floor (struct loose_floor *l);
void no_action_loose_floor (struct pos *p);
void shake_loose_floor (struct pos *p);
void release_loose_floor (struct pos *p, struct actor *a);
void fall_loose_floor (struct pos *p);
void break_loose_floor (struct pos *p);
void compute_loose_floors (void);
void compute_loose_floor_shake (struct loose_floor *l);
void compute_loose_floor_release (struct loose_floor *l);
void compute_loose_floor_fall (struct loose_floor *l);
void shake_loose_floor_row (struct pos *p);
void sample_random_loose_floor (struct pos *p);
ALLEGRO_BITMAP *get_correct_falling_loose_floor_bitmap (bool broken);
void draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);
void loose_floor_fall_debug (void);

#endif	/* MININIM_LOOSE_FLOOR_H */
