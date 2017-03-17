/*
  level-door.h -- level door module;

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

#ifndef MININIM_LEVEL_DOOR_H
#define MININIM_LEVEL_DOOR_H

/* variables */
extern struct level_door *level_door;
extern size_t level_door_nmemb;

/* functions */
struct level_door *init_level_door (struct pos *p, struct level_door *d);
void register_level_door (struct pos *p);
int compare_level_doors (const void *d0, const void *d1);
struct level_door *copy_level_door (struct level_door *to,
                                    struct level_door *from);
struct level_door *level_door_at_pos (struct pos *p);
void remove_level_door (struct level_door *d);
int fake_door_step (struct pos *p);
bool fake_door_stairs (struct pos *p);
void break_level_door (struct pos *p);
void compute_level_doors (void);
struct level_door *get_exit_level_door (struct level *l, int n);
struct pos *first_level_door_in_room_pos (int room, struct pos *p_ret);
void draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_level_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                         struct actor *a);
void draw_level_door_floor (ALLEGRO_BITMAP *bitmap, struct pos *p);

#endif	/* MININIM_LEVEL_DOOR_H */
