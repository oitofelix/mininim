/*
  undo.h -- undo module;

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

#ifndef MININIM_UNDO_H
#define MININIM_UNDO_H

void register_undo (struct undo *u, void *data, undo_f f,
                    destroy_undo_f df, char *desc);
void free_undo_tail (struct undo *u);
void free_undo (struct undo *u);
bool can_undo (struct undo *u, int dir);
bool undo_pass (struct undo *u, int dir, char **desc);
void end_undo_set (struct undo *u, char *desc);
int current_macro_undo (struct undo *u);

/* TILE */
void register_tile_undo (struct undo *u, struct pos *p,
                         int f, int b, int e, int ff,
                         union tile_state *cs,
                         intptr_t ignore_id,
                         char *desc);
void tile_undo (struct tile_undo *d, int dir);

/* MIRROR POS */
void register_mirror_pos_undo (struct undo *u, struct pos *p0, struct pos *p1,
                               bool invert_dir, char *desc);
void mirror_pos_undo (struct mirror_pos_undo *d, int dir);

/* LEVEL */
void register_level_undo (struct undo *u, struct level *l,
                          char *desc);
void level_undo (struct level_undo *d, int dir);

/* LEVEL EXCHANGE */
void register_level_exchange_undo (struct undo *u, int n,
                                   char *desc);
void level_exchange_undo (int *d, int dir);


/* EVENT */
void
register_event_undo (struct undo *u, int e, struct pos *p, bool next,
                     char *desc);
void event_undo (struct event_undo *d, int dir);

/* HORIZONTAL ROOM CONS MIRROR */
void register_h_room_mirror_tile_undo (struct undo *u, int _room, char *desc);
void h_room_mirror_tile_undo (int *room, int dir);

/* VERTICAL ROOM CONS MIRROR */
void register_v_room_mirror_tile_undo (struct undo *u, int _room, char *desc);
void v_room_mirror_tile_undo (int *room, int dir);

/* RANDOM ROOM TILE MIRROR */
void register_random_room_mirror_tile_undo (struct undo *u, int _room,
                                           bool invert_dir,
                                           char *desc);
void random_room_mirror_tile_undo (struct random_room_mirror_tile_undo *d,
                                   int dir);

/* LINK */
void register_link_undo (struct undo *u, struct room_linking *l,
                         char *desc);
void link_undo (struct link_undo *d, int dir);
void destroy_link_undo (struct link_undo *d);

/* START POSITION */
void register_start_pos_undo (struct undo *u, struct pos *p, char *desc);
void start_pos_undo (struct start_pos_undo *d, int dir);

/* TOGGLE START POSITION */
void register_toggle_start_dir_undo (struct undo *u, char *desc);
void toggle_start_dir_undo (struct start_pos_undo *d, int dir);

/* TOGGLE HAS SWORD */
void register_toggle_has_sword_undo (struct undo *u, char *desc);
void toggle_has_sword_undo (struct start_pos_undo *d, int dir);

/* GUARD START POSITION */
void register_guard_start_pos_undo (struct undo *u, int i, struct pos *p,
                                    char *desc);
void guard_start_pos_undo (struct guard_start_pos_undo *d, int dir);

/* TOGGLE GUARD START DIRECTION */
void register_toggle_guard_start_dir_undo (struct undo *u, int i, char *desc);
void toggle_guard_start_dir_undo (int *d, int dir);

/* GUARD SKILL */
void register_guard_skill_undo (struct undo *u, int i, struct skill *s,
                                char *desc);
void guard_skill_undo (struct guard_skill_undo *d, int dir);

/* GUARD HP */
void register_guard_hp_undo (struct undo *u, int i, int l, char *desc);
void guard_hp_undo (struct indexed_int_undo *d, int dir);

/* GUARD TYPE */
void register_guard_type_undo (struct undo *u, int i, enum actor_type t,
                               char *desc);
void guard_type_undo (struct indexed_int_undo *d, int dir);

/* GUARD STYLE */
void register_guard_style_undo (struct undo *u, int i, int t, char *desc);
void guard_style_undo (struct indexed_int_undo *d, int dir);

/* INT UNDO */
void register_int_undo (struct undo *u, int *f, int b, undo_f func, char *desc);
void int_undo (struct int_undo *d, int dir);

/* LEVEL ENVIRONMENT UNDO */
void level_environment_undo (struct int_undo *d, int dir);

/* LEVEL HUE UNDO */
void level_hue_undo (struct int_undo *d, int dir);

#endif	/* MININIM_UNDO_H */
