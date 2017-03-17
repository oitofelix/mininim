/*
  spikes-floor.h -- spikes floor module;

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

#ifndef MININIM_SPIKES_H
#define MININIM_SPIKES_H

/* variables */
extern struct spikes_floor *spikes_floor;
extern size_t spikes_floor_nmemb;

/* functions */
struct spikes_floor *init_spikes_floor (struct pos *p,
                                        struct spikes_floor *s);
void register_spikes_floor (struct pos *p);
void sort_spikes_floors (void);
int compare_spikes_floors (const void *s0, const void *s1);
struct spikes_floor *copy_spikes_floor (struct spikes_floor *to,
                                        struct spikes_floor *from);
struct spikes_floor * spikes_floor_at_pos (struct pos *p);
void remove_spikes_floor (struct spikes_floor *s);
void break_spikes_floor (struct pos *p);
void compute_spikes_floors (void);
bool should_spikes_raise_for_pos (struct pos *p, struct pos *pk);
bool should_spikes_raise (struct pos *p);
void draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_spikes_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);

#endif	/* MININIM_SPIKES_H */
